#include "new-gui/brat/stdafx.h"

#include "new-gui/Common/QtUtilsIO.h"

#include "new-gui/brat/DataModels/Workspaces/Workspace.h"
#include "new-gui/brat/DataModels/Workspaces/Display.h"
#include "new-gui/brat/DataModels/DisplayFilesProcessor.h"
#include "new-gui/brat/DataModels/PlotData/WorldPlot.h"

#include "new-gui/brat/GUI/ActionsTable.h"
#include "new-gui/brat/GUI/TabbedDock.h"
#include "new-gui/brat/GUI/ControlsPanel.h"
#include "new-gui/brat/GUI/DatasetBrowserControls.h"
#include "new-gui/brat/GUI/ViewControlsPanel.h"
#include "new-gui/brat/Views/BratViews.h"
#include "new-gui/brat/Views/GlobeWidget.h"
#include "new-gui/brat/Views/TextWidget.h"

#include "ViewEditors.h"


/////////////////////////////////////////////////////////////////
//	TODO: move to proper place after using real projections
/////////////////////////////////////////////////////////////////

QActionGroup* ProjectionsActions( QWidget *parent )
{
	std::vector<EActionTag> ActionTags =
	{
		eAction_Projection1,
		eAction_Projection2,
		eAction_Projection3,
	};

	return CActionInfo::CreateActionsGroup( parent, ActionTags, true );
}







#define DE_NOT_IMPLEMENTED( checked )     FNOT_IMPLEMENTED( n2s<std::string>( checked ) + __func__ );


///////////////////////////////////////////////////////////////////////////////////////////
//	Base
///////////////////////////////////////////////////////////////////////////////////////////

void CAbstractViewEditor::FilterDisplays( bool with_maps )
{
	auto &displays = *mWksd->GetDisplays();
	std::string errors;
	for ( auto &display_entry : displays )
	{
		DisplayFilesProcessor *p = new DisplayFilesProcessor;
		CDisplay *display = dynamic_cast<CDisplay*>( display_entry.second );		assert__( display );
		try
		{
			p->Process( display->GetCmdFilePath() );

			auto &plots = p->plots();
			for ( auto *plot : plots )
				if ( plot )
					plot->GetInfo();

			if ( with_maps ? p->isZFLatLon() : !p->isZFLatLon() )
			{
				mCmdLineProcessors.push_back( p );
				continue;
			}
		}
		catch ( const CException &e )
		{
			errors += ( e.Message() + "\n" );
		}
		delete p;
	}

	if ( !errors.empty() )
		SimpleWarnBox( errors );
}



void CAbstractViewEditor::CreateWorkingDock()
{
	setDockOptions( dockOptions() & ~AnimatedDocks );	//TODO: DELETE AFTER CHECKING WINDOWS BLUE SCREENS

	//dock
	mWorkingDock = new CWorkspaceTabbedDock( QString( "View Panel" ) + "[*]", this );
	mWorkingDock->setMinimumSize( min_editor_dock_width, min_editor_dock_height );
	auto PreventActions = QDockWidget::DockWidgetClosable;							//QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable | 
	mWorkingDock->setFeatures( mWorkingDock->features() & ~PreventActions );

	addDockWidget( Qt::DockWidgetArea::BottomDockWidgetArea, mWorkingDock );

	CActionInfo::SetDockActionProperties( mWorkingDock, eAction_DisplayEditorDock );		//TODO: Remove this if the action is not added to the DisplayEditor tool-bar
}


void CAbstractViewEditor::CreateGraphicsBar()
{
    mToolBar = new QToolBar( "View ToolBar", this );
    mToolBar->setIconSize({icon_size,icon_size});

	// add button group

	QActionGroup *group = CActionInfo::CreateActionsGroup( this, { eAction_DisplayEditor2D, eAction_DisplayEditor3D, eAction_DisplayEditorLog }, false );

	mAction2D = group->actions()[0];														assert__( mAction2D->isCheckable() );
	connect( mAction2D, SIGNAL( toggled( bool ) ), this, SLOT( Handle2D_slot( bool ) ) );

	mAction3D = group->actions()[1];														assert__( mAction3D->isCheckable() );
	connect( mAction3D, SIGNAL( toggled( bool ) ), this, SLOT( Handle3D_slot( bool ) ) );

	mLogAction = group->actions()[2];														assert__( mLogAction->isCheckable() );
	connect( mLogAction, SIGNAL( toggled( bool ) ), this, SLOT( HandleLog_slot( bool ) ) );

	mToolBar->addActions( group->actions() );

	// add the bar

	mToolBar->setAllowedAreas( Qt::RightToolBarArea );
    addToolBar( Qt::RightToolBarArea, mToolBar );
}

void CAbstractViewEditor::CreateStatusBar()
{
    mStatusBar = new QStatusBar( this );
    mStatusBar->setObjectName(QString::fromUtf8("mStatusBar"));
    setStatusBar(mStatusBar);
}

void CAbstractViewEditor::CreateWidgets()
{
	setAttribute( Qt::WA_DeleteOnClose );

	CreateWorkingDock();
	CreateGraphicsBar();
	CreateStatusBar();

	mMainSplitter = createSplitterIn( this, Qt::Vertical );
	setCentralWidget( mMainSplitter );
	mGraphicsSplitter = createSplitterIn( mMainSplitter, Qt::Horizontal );
	mLogText = new CTextWidget( this );
	mMainSplitter->addWidget( mLogText );
	mLogText->hide();

	mTabGeneral = new CViewControlsPanelGeneral( mCmdLineProcessors, this );
	AddTab( mTabGeneral, "General" );

	CreateStatusBar();

	Wire();
}

void CAbstractViewEditor::Wire()
{
    connect( mTabGeneral, SIGNAL( CurrentDisplayIndexChanged(int) ), this, SLOT( PlotChanged(int) ) );
    connect( mTabGeneral, SIGNAL( RunButtonClicked(int) ), this, SLOT( PlotChanged(int) ) );

	connect( mTabGeneral, SIGNAL( NewButtonClicked() ), this, SLOT( NewButtonClicked() ) );
}


CAbstractViewEditor::CAbstractViewEditor( bool map_editor, DisplayFilesProcessor *proc, QWidget *parent )	//parent = nullptr, QWidget *view = nullptr ) 
	: base_t( parent )
	, mWksd( nullptr )
{
    assert__( !map_editor || proc->isZFLatLon() );              Q_UNUSED( map_editor );     //release versions
	assert__( map_editor || proc->isYFX() || proc->isZFXY() );

	mCmdLineProcessors.push_back( proc );
	CreateWidgets();
}

CAbstractViewEditor::CAbstractViewEditor( bool map_editor, CWorkspaceDisplay *wksd, QWidget *parent )	//parent = nullptr, QWidget *view = nullptr ) 
	: base_t( parent )
	, mWksd( wksd )
{
	FilterDisplays( map_editor );
	CreateWidgets();
}


QWidget* CAbstractViewEditor::AddTab( QWidget *tab , const QString title )		// title = "" 
{
	return mWorkingDock->AddTab( tab, title );
}


bool CAbstractViewEditor::AddView( QWidget *view, bool view3D )
{
	if ( mGraphicsSplitter->count() == 2 )
		return false;

	if ( view3D )
	{
		mAction3D->setChecked( true );
		mGraphicsSplitter->insertWidget( 1, view );
	}
	else
	{
		mAction2D->setChecked( true );
		mGraphicsSplitter->insertWidget( 0, view );
	}

	return true;
}

bool CAbstractViewEditor::RemoveView( QWidget *view, bool view3D )
{
	const int count = mGraphicsSplitter->count();
	if ( count == 0 )
		return false;

	delete view;				assert__( mGraphicsSplitter->count() == count - 1 );

	if ( view3D )
	{
		mAction3D->blockSignals( true );
		mAction3D->setChecked( false );
		mAction3D->blockSignals( false );
	}
	else
	{
		mAction2D->blockSignals( true );
		mAction2D->setChecked( false );
		mAction2D->blockSignals( false );
	}

	return true;
}



QAction* CAbstractViewEditor::AddToolBarAction( QObject *parent, EActionTag tag )
{
	QAction *a = CActionInfo::CreateAction( parent, tag );
	mToolBar->addAction( a );
	return a;
}


QAction* CAbstractViewEditor::AddToolBarSeparator()
{
	return mToolBar->addSeparator();
}

QToolButton* CAbstractViewEditor::AddMenuButton( EActionTag button_tag, const QList<QAction*> &actions )
{
	QToolButton *b = CActionInfo::CreateMenuButton( button_tag, actions );
	mToolBar->addWidget( b );
	return b;
}



void CAbstractViewEditor::Handle2D_slot( bool checked )
{
	Handle2D( checked );
}
void CAbstractViewEditor::Handle3D_slot( bool checked )
{
	Handle3D( checked );
}
void CAbstractViewEditor::HandleLog_slot( bool checked )
{
	mLogText->setVisible( checked );
}




///////////////////////////////////////////////////////////////////////////////////////////
//	Maps
///////////////////////////////////////////////////////////////////////////////////////////

void CMapEditor::Wire()
{
	connect( mActionStatisticsMean, SIGNAL( triggered() ), this, SLOT( HandleStatisticsMean() ) );
	connect( mActionStatisticsStDev, SIGNAL( triggered() ), this, SLOT( StatisticsStDev() ) );
	connect( mActionStatisticsLinearRegression, SIGNAL( triggered() ), this, SLOT( HandleStatisticsLinearRegression() ) );

	for ( auto a : mProjectionsGroup->actions() )
	{
		assert__( a->isCheckable() );
		connect( a, SIGNAL( triggered() ), this, SLOT( HandleProjection() ) );
	}
}
void CMapEditor::CreateMapActions()
{
	mMap->AddMapMeasureActions( mToolBar, true );		// true -> separator
	mMap->AddGridActions( mToolBar, false );

	AddToolBarSeparator();
	mActionStatisticsMean = AddToolBarAction( this, eAction_MapEditorMean );
	mActionStatisticsStDev = AddToolBarAction( this, eAction_MapEditorStDev );
	mActionStatisticsLinearRegression = AddToolBarAction( this, eAction_MapEditorLinearRegression );

	// add menu button for projections

	AddToolBarSeparator();
	mProjectionsGroup = ProjectionsActions( this );
	mToolProjection = AddMenuButton( eActionGroup_Projections, mProjectionsGroup->actions() );
}


void CMapEditor::CreateWidgets()
{
	mTabData = new CMapControlsPanelEdit( this );
	mTabView = new CMapControlsPanelOptions( this );
	mTabPlots = new CViewControlsPanelPlots( this );

	AddTab( mTabData, "Edit" );
	AddTab( mTabView, "Options" );
	AddTab( mTabPlots, "Plots" );


    //    Set Header Label Texts
    mTabPlots->mPlotInfoTable->setRowCount(1);
    mTabPlots->mPlotInfoTable->setColumnCount(5);
    mTabPlots->mPlotInfoTable->setHorizontalHeaderLabels( QString("Plot name;Expression name;Unit;Operation name;Dataset").split(";") );

	mMap = new CBratMapView( this );
	AddView( mMap, false );

	mMap->InsertStatusBarMapWidgets( statusBar(), 0 );

	CreateMapActions();

	// According to the selected projection index (instead of hard-coded 0):
	//
	mProjectionsGroup->actions()[ 0 ]->setChecked( true );

	Wire();
}


CMapEditor::CMapEditor( DisplayFilesProcessor *proc, CWPlot* wplot, QWidget *parent )		//parent = nullptr
	: base_t( true, proc, parent )
{
	CreateWidgets();

	mMap->CreatePlot( proc->GetWorldPlotProperties( 0 ), wplot );
}


CMapEditor::CMapEditor( CWorkspaceDisplay *wksd, QWidget *parent )		//parent = nullptr
	: base_t( true, wksd, parent )
{
	CreateWidgets();
}


//virtual 
CMapEditor::~CMapEditor()
{
	delete mGlobe;
	delete mMap;
}


//virtual 
void CMapEditor::Handle2D( bool checked )
{
	mMap->setVisible( checked );
}
//virtual 
void CMapEditor::Handle3D( bool checked )
{
	if ( checked && !mGlobe )
	{
		WaitCursor wait;

		mGlobe = new CGlobeWidget( this, CMapWidget::GlobeDir(), mMap );
		AddView( mGlobe, true );
	}
	mGlobe->setVisible( checked );
}
//virtual 
void CMapEditor::HandleLog( bool checked )
{
	mLogText->setVisible( checked );
}


void CMapEditor::HandlePlotChanged( int index )
{
	if ( index < 0 )
		return;

	WaitCursor wait;

	try
	{
		DisplayFilesProcessor *proc = mCmdLineProcessors[ index ];		assert__( proc->isZFLatLon() );
		auto &plots = proc->plots();
		for ( auto *plot : plots )
		{
			CWPlot* wplot = dynamic_cast<CWPlot*>( plot );
			if ( wplot == nullptr )
				continue;

			mMap->CreatePlot( proc->GetWorldPlotProperties( 0 ), wplot );
		}
	}
	catch ( CException &e )
	{
		SimpleErrorBox( e.what() );
		throw;
	}
	catch ( std::exception &e )
	{
		SimpleErrorBox( e.what() );
		throw;
	}
	catch ( ... )
	{
		SimpleErrorBox( "Unexpected error trying to create a map plot." );
		throw;
	}
}




void CMapEditor::HandleStatisticsMean()
{
	NOT_IMPLEMENTED
}


void CMapEditor::StatisticsStDev()
{
	NOT_IMPLEMENTED
}


void CMapEditor::HandleStatisticsLinearRegression()
{
	NOT_IMPLEMENTED
}


void CMapEditor::HandleProjection()
{
	auto a = qobject_cast<QAction*>( sender() );				assert__( a );
	auto index = mProjectionsGroup->actions().indexOf( a );		assert__( index >= 0 );

	SimpleMsgBox( "Not implemented: projection # " + n2s<std::string>( index ) );
}


//virtual 
void CMapEditor::HandleNewButtonClicked()
{
	NOT_IMPLEMENTED
}




///////////////////////////////////////////////////////////////////////////////////////////
//	Plots
///////////////////////////////////////////////////////////////////////////////////////////

void CPlotEditor::CreateWidgets() 	//parent = nullptr
{
	mTabData = new CPlotControlsPanelEdit( this );
	mTabCurveOptions = new CPlotControlsPanelCurveOptions( this );
	mTabAxisOptions = new CPlotControlsPanelAxisOptions( this );
	mTabPlots = new CViewControlsPanelPlots( this );

	AddTab( mTabData, "Edit" );
	AddTab( mTabCurveOptions, "Curve Options" );
	AddTab( mTabAxisOptions, "Axis Options" );
	AddTab( mTabPlots, "Plots" );

    //    Set Header Label Texts
    mTabPlots->mPlotInfoTable->setRowCount(1);
    mTabPlots->mPlotInfoTable->setColumnCount(8);
    mTabPlots->mPlotInfoTable->setHorizontalHeaderLabels(
                QString("Plot name;Expression name;View type;Unit;dim.1;dim.2;Operation name;Dataset").split(";"));

	mPlot2D = new CBrat2DPlotView( this );
	mPlot3D = new CBrat3DPlotView( this );
	AddView( mPlot2D, false );
	AddView( mPlot3D, true );

	Wire();
}

void CPlotEditor::Wire()
{
	connect( mTabAxisOptions, SIGNAL( LogarithmicScaleX( bool ) ), this, SLOT( HandleLogarithmicScaleX( bool ) ) );
	connect( mTabAxisOptions, SIGNAL( LogarithmicScaleY( bool ) ), this, SLOT( HandleLogarithmicScaleY( bool ) ) );
	connect( mTabAxisOptions, SIGNAL( LogarithmicScaleZ( bool ) ), this, SLOT( HandleLogarithmicScaleZ( bool ) ) );

	connect( mTabCurveOptions, SIGNAL( LineOptionsChecked( bool ) ), this, SLOT( HandleLineOptionsChecked( bool ) ) );
	connect( mTabCurveOptions, SIGNAL( PointOptionsChecked( bool ) ), this, SLOT( HandlePointOptionsChecked( bool ) ) );
}

CPlotEditor::CPlotEditor( CWorkspaceDisplay *wksd, const DisplayFilesProcessor *proc, CPlot* plot, QWidget *parent ) 	//parent = nullptr
	: base_t( false, wksd, parent )
{
    Q_UNUSED(proc);     Q_UNUSED(plot);         //mPlot2D->CreatePlot( proc, plot );

    CreateWidgets();

	//FilterDisplays( false );

	//for ( auto *cmd : mCmdLineProcessors )
	//{
	//	mTabGeneral->mOpenAplot->addItem( t2q( cmd->ParamFile() ) );
	//}
	//mTabGeneral->mOpenAplot->setCurrentIndex( 0 );

}

CPlotEditor::CPlotEditor( CWorkspaceDisplay *wksd, const DisplayFilesProcessor *proc, CZFXYPlot* plot, QWidget *parent ) 	//parent = nullptr
	: base_t( false, wksd, parent )

{
    CreateWidgets();

    Q_UNUSED(proc);     Q_UNUSED(plot);     //mPlot3D->CreatePlot( proc, plot );

	//FilterDisplays( false );

	//for ( auto *cmd : mCmdLineProcessors )
	//{
	//	mTabGeneral->mOpenAplot->addItem( t2q( cmd->ParamFile() ) );
	//}
	//mTabGeneral->mOpenAplot->setCurrentIndex( 0 );
}




//virtual 
CPlotEditor::~CPlotEditor()
{}

//virtual 
void CPlotEditor::Handle2D( bool checked )
{
	assert__( mPlot2D );

	mPlot2D->setVisible( checked );
}
//virtual 
void CPlotEditor::Handle3D( bool checked )
{
	assert__( mPlot3D );

	mPlot3D->setVisible( checked );
}
//virtual 
void CPlotEditor::HandleLog( bool checked )
{
	assert__( mLogText );

	mLogText->setVisible( checked );
}

//virtual 
void CPlotEditor::HandlePlotChanged( int index )
{
	if ( index < 0 )
		return;

	WaitCursor wait;

	try
	{
		HandleNewButtonClicked();

		DisplayFilesProcessor *proc = mCmdLineProcessors[ index ];		assert__( !proc->isZFLatLon() );
		auto &plots = proc->plots();
		if ( proc->isYFX() )			// =================================== XYPlot();
		{
			for ( auto *plot : plots )
			{
				CPlot* yfxplot = dynamic_cast<CPlot*>( plot );
				if ( yfxplot == nullptr )
					continue;

				mPlot2D->CreatePlot( proc->GetXYPlotProperties( 0 ), yfxplot );
			}
		}
		else if ( proc->isZFXY() )		// =================================== ZFXYPlot();
		{
			for ( auto *plot : plots )
			{
				CZFXYPlot* zfxyplot = dynamic_cast<CZFXYPlot*>( plot );
				if ( zfxyplot == nullptr )
					continue;

				mPlot3D->CreatePlot( proc->GetZFXYPlotProperties( 0 ), zfxyplot );
			}
		}
		else
			assert__( false );
	}
	catch ( CException &e )
	{
		SimpleErrorBox( e.what() );
		throw;
	}
	catch ( std::exception &e )
	{
		SimpleErrorBox( e.what() );
		throw;
	}
	catch ( ... )
	{
		SimpleErrorBox( "Unexpected error encountered" );
		throw;
	}
}


//virtual 
void CPlotEditor::HandleNewButtonClicked()
{
	// TODO delete this after testing 2D plots clear method
	//if ( mPlot2D )
	//	mPlot2D->Clear();
	if ( mPlot2D )
	{
		auto *p = mPlot2D;
		mPlot2D = nullptr;
		RemoveView( p, false );
		mPlot2D = new CBrat2DPlotView( this );
		AddView( mPlot2D, false );
	}

	NOT_IMPLEMENTED
}


void CPlotEditor::HandleLogarithmicScaleX( bool log )
{
	assert__( mPlot3D );

	WaitCursor wait;

	mPlot3D->SetXLogarithmicScale( log );
}

void CPlotEditor::HandleLogarithmicScaleY( bool log )
{
	assert__( mPlot3D );

	WaitCursor wait;

	mPlot3D->SetYLogarithmicScale( log );
}

void CPlotEditor::HandleLogarithmicScaleZ( bool log )
{
	assert__( mPlot3D );

	WaitCursor wait;

	mPlot3D->SetZLogarithmicScale( log );
}


void CPlotEditor::HandleLineOptionsChecked( bool checked )
{
	if ( mPlot2D && checked )
	{
		mPlot2D->SetLineCurves();
	}
	// TODO 3D

}
void CPlotEditor::HandlePointOptionsChecked( bool checked )
{
	if ( mPlot2D && checked )
	{
		mPlot2D->SetPointCurves();
	}
	// TODO 3D

}




///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_ViewEditors.cpp"