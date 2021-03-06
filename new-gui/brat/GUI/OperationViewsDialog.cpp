/*
* This file is part of BRAT
*
* BRAT is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* BRAT is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#include "new-gui/brat/stdafx.h"


#include "new-gui/Common/QtUtils.h"

#include "DataModels/Model.h"
#include "DataModels/Workspaces/Workspace.h"
#include "DataModels/Workspaces/Display.h"

#include "new-gui/Common/GUI/TextWidget.h"

#include "OperationViewsDialog.h"



void COperationViewsDialog::CreateWidgets()
{
	static const QStringList header_labels = QStringList() << tr( "Name" ) << tr( "Type" ) << tr( "Value" );

	//	Create

	//	... Tree

	mViewsListWidget = new QListWidget( this );
	mViewsListWidget->setSelectionBehavior( QAbstractItemView::SelectRows );
	mViewsListWidget->setSelectionMode( QAbstractItemView::SingleSelection );
	mViewsListWidget->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::MinimumExpanding );

	mMapsAsPlotsCheckBox = new QCheckBox( "Display maps as 3D plots" );


	//	... Help

	auto help = new CTextWidget;
	help->SetHelpProperties( "All views generated by the operation " + t2q( mOperation.GetName() ), 0, 6, Qt::AlignCenter );
    auto help_group = CreateGroupBox( ELayoutType::Grid, { help }, "", nullptr, 6, 6, 6, 6, 6 );
    help_group->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Maximum );


	//	... Buttons

	mButtonBox = new QDialogButtonBox( this );
	mButtonBox->setObjectName( QString::fromUtf8( "mButtonBox" ) );
	mButtonBox->setOrientation( Qt::Horizontal );
	mButtonBox->setStandardButtons( QDialogButtonBox::Discard | QDialogButtonBox::Open | QDialogButtonBox::Cancel );
    mButtonBox->button( QDialogButtonBox::Open )->setDefault( false );
    mButtonBox->button( QDialogButtonBox::Discard )->setDefault( false );

    QBoxLayout *main_l =
            LayoutWidgets( Qt::Vertical,
                            {
                                mViewsListWidget,
								mMapsAsPlotsCheckBox,
								help_group,
                                mButtonBox

                            }, this, 6, 6, 6, 6, 6 );                       Q_UNUSED( main_l );


    setWindowTitle( t2q( mOperation.GetName() ) + " Views" );

    //	Wrap up dimensions

    adjustSize();
	setMinimumWidth( width() );

	Wire();
}

void COperationViewsDialog::Wire()
{
    //	Setup things

	connect( mButtonBox->button( QDialogButtonBox::Discard ), SIGNAL( clicked() ), this, SLOT( HandleDeleteView() ) );
	connect( mButtonBox->button( QDialogButtonBox::Open ), SIGNAL( clicked() ), this, SLOT( accept() ) );

	connect( mButtonBox, SIGNAL( rejected() ), this, SLOT( reject() ) );

	auto displays = mModel.OperationDisplays( mOperation.GetName() );	assert__( displays.size() > 0 );
	for ( auto const *display : displays )
	{
		QListWidgetItem *item = new QListWidgetItem( display->GetName().c_str(), mViewsListWidget );
		std::string tip = display->IsZLatLonType() ? "Type: map" : ( display->IsZYFXType() ? "Type: Z=F(X,Y)" : "Type: Y=F(X)" );
		item->setToolTip( tip.c_str() );
	}
	mViewsListWidget->setCurrentRow( 0 );
}


COperationViewsDialog::COperationViewsDialog( QWidget *parent, COperation &operation, CModel &model )
	: base_t( parent )
	, mOperation( operation )
	, mModel( model )
{
	CreateWidgets();
}


COperationViewsDialog::~COperationViewsDialog()
{}



void COperationViewsDialog::HandleDeleteView()
{
	auto *item = mViewsListWidget->currentItem();
	if ( !item )
	{
		SimpleErrorBox( "There is no selected view to delete" );
		return;
	}
	std::string dname = q2a( item->text() );
	mSelectedDisplay = mModel.Workspace< CWorkspaceDisplay >()->GetDisplay( dname );
	if ( SimpleQuestion( "Are you sure you want to delete the view '" + dname + "'?" ) )
	{
		mModel.Workspace< CWorkspaceDisplay >()->DeleteDisplay( mSelectedDisplay );
		mSelectedDisplay = nullptr;

		reject();
	}
}


//virtual 
QSize COperationViewsDialog::sizeHint() const
{
	int delta_height = base_t::sizeHint().height() - mViewsListWidget->sizeHint().height() + 4;		//+ 4: guessing borders
    QSize s( base_t::sizeHint().width(), 
		delta_height + (int)mModel.OperationDisplays( mOperation.GetName() ).size() * ( mViewsListWidget->fontMetrics().height() + 2 ) );	//+2: guessing items spacing
	return s;
}


//virtual 
void COperationViewsDialog::accept()
{
	auto *item = mViewsListWidget->currentItem();
	if ( !item )
	{
		SimpleErrorBox( "There is no selected view to open" );
		return;
	}
	std::string dname = q2a( item->text() );
	mSelectedDisplay = mModel.Workspace< CWorkspaceDisplay >()->GetDisplay( dname );

	base_t::accept();
}






///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_OperationViewsDialog.cpp"
