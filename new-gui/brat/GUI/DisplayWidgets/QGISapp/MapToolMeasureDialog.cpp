/***************************************************************************
                                 qgsmeasure.h
                               ------------------
        begin                : March 2005
        copyright            : (C) 2005 by Radim Blazek
        email                : blazek@itc.it
 ***************************************************************************/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "new-gui/brat/stdafx.h"

#include <QCloseEvent>
#include <QLocale>
#include <QPushButton>

#include <qgslogger.h>
#include <qgsdistancearea.h>
#include <qgsmapcanvas.h>
#include <qgsmaprenderer.h>
#include <qgsproject.h>
#include <qgscoordinatereferencesystem.h>

#include "MapToolMeasureDialog.h"
#include "MapToolMeasure.h"


CMeasureDialog::CMeasureDialog( CMeasureTool* tool, Qt::WindowFlags f )
	: QDialog( tool->canvas()->topLevelWidget(), f ), mTool( tool )
{
	setupUi( this );

	QPushButton *nb = new QPushButton( tr( "&New" ) );
	buttonBox->addButton( nb, QDialogButtonBox::ActionRole );
	connect( nb, SIGNAL( clicked() ), this, SLOT( restart() ) );

	mMeasureArea = tool->measureArea();
	mTotal = 0.;

	mUnitsCombo->addItem( MapUnit2String( QGis::Meters ) );
	mUnitsCombo->addItem( MapUnit2String( QGis::Feet ) );
	mUnitsCombo->addItem( MapUnit2String( QGis::Degrees ) );
	mUnitsCombo->addItem( MapUnit2String( QGis::NauticalMiles ) );

	QSettings settings;
	QString units = settings.value( "/qgis/measure/displayunits", EncodeUnit( QGis::Meters ) ).toString();
	mUnitsCombo->setCurrentIndex( mUnitsCombo->findText( MapUnit2String( DecodeDistanceUnit( units ) ), Qt::MatchFixedString ) );

	updateSettings();

	connect( mUnitsCombo, SIGNAL( currentIndexChanged( const QString & ) ), this, SLOT( unitsChanged( const QString & ) ) );

	groupBox->setCollapsed( true );
}

void CMeasureDialog::updateSettings()
{
	QSettings settings;

	mDecimalPlaces = settings.value( "/qgis/measure/decimalplaces", "3" ).toInt();
	mCanvasUnits = mTool->canvas()->mapUnits();
	// Configure QgsDistanceArea
	mDisplayUnits = StringToDistanceUnit( mUnitsCombo->currentText() );
	mDa.setSourceCrs( mTool->canvas()->mapSettings().destinationCrs().srsid() );
	mDa.setEllipsoid( QgsProject::instance()->readEntry( "Measure", "/Ellipsoid", GEO_NONE ) );
	// Only use ellipsoidal calculation when project wide transformation is enabled.
	if ( mTool->canvas()->mapSettings().hasCrsTransformEnabled() )
	{
		mDa.setEllipsoidalMode( true );
	}
	else
	{
		mDa.setEllipsoidalMode( false );
	}

	QgsDebugMsg( "****************" );
	QgsDebugMsg( QString( "Ellipsoid ID : %1" ).arg( mDa.ellipsoid() ) );
	QgsDebugMsg( QString( "Ellipsoidal  : %1" ).arg( mDa.ellipsoidalEnabled() ? "true" : "false" ) );
	QgsDebugMsg( QString( "Decimalplaces: %1" ).arg( mDecimalPlaces ) );
	QgsDebugMsg( QString( "Display units: %1" ).arg( EncodeUnit( mDisplayUnits ) ) );
	QgsDebugMsg( QString( "Canvas units : %1" ).arg( EncodeUnit( mCanvasUnits ) ) );

	mTotal = 0;
	updateUi();
}

void CMeasureDialog::unitsChanged( const QString &units )
{
	mDisplayUnits = StringToDistanceUnit( units );
	mTable->clear();
	mTotal = 0.;
	updateUi();
}

void CMeasureDialog::restart()
{
	mTool->restart();

	mTable->clear();
	mTotal = 0.;
	updateUi();
}


void CMeasureDialog::mouseMove( QgsPoint &point )
{
	mLastMousePoint = point;
	// show current distance/area while moving the point
	// by creating a temporary copy of point array
	// and adding moving point at the end
	if ( mMeasureArea && mTool->points().size() >= 2 )
	{
		QList<QgsPoint> tmpPoints = mTool->points();
		tmpPoints.append( point );
		double area = mDa.measurePolygon( tmpPoints );
		editTotal->setText( formatArea( area ) );
	}
	else if ( !mMeasureArea && mTool->points().size() >= 1 )
	{
		QgsPoint p1( mTool->points().last() ), p2( point );
		double d = mDa.measureLine( p1, p2 );

		editTotal->setText( formatDistance( mTotal + d ) );

		QGis::UnitType displayUnits;
		// Meters or feet?
		convertMeasurement( d, displayUnits, false );

		// Set moving
		QTreeWidgetItem *item = mTable->topLevelItem( mTable->topLevelItemCount() - 1 );
		item->setText( 0, QLocale::system().toString( d, 'f', mDecimalPlaces ) );
		QgsDebugMsg( QString( "Final result is %1" ).arg( item->text( 0 ) ) );
	}
}

void CMeasureDialog::addPoint( QgsPoint &p )
{
	Q_UNUSED( p );

	int numPoints = mTool->points().size();
	if ( mMeasureArea && numPoints > 2 )
	{
		double area = mDa.measurePolygon( mTool->points() );
		editTotal->setText( formatArea( area ) );
	}
	else if ( !mMeasureArea && numPoints >= 1 )
	{
		if ( !mTool->done() )
		{
			QTreeWidgetItem * item = new QTreeWidgetItem( QStringList( QLocale::system().toString( 0.0, 'f', mDecimalPlaces ) ) );
			item->setTextAlignment( 0, Qt::AlignRight );
			mTable->addTopLevelItem( item );
			mTable->scrollToItem( item );
		}
		if ( numPoints > 1 )
		{
			mTotal = mDa.measureLine( mTool->points() );
			editTotal->setText( formatDistance( mTotal ) );
		}
	}
	QgsDebugMsg( "Exiting" );
}

void CMeasureDialog::removeLastPoint()
{
	int numPoints = mTool->points().size();
	if ( mMeasureArea )
	{
		if ( numPoints > 1 )
		{
			QList<QgsPoint> tmpPoints = mTool->points();
			tmpPoints.append( mLastMousePoint );
			double area = mDa.measurePolygon( tmpPoints );
			editTotal->setText( formatArea( area ) );
		}
		else
		{
			editTotal->setText( formatArea( 0 ) );
		}
	}
	else if ( !mMeasureArea && numPoints >= 1 )
	{
		//remove final row
		delete mTable->takeTopLevelItem( mTable->topLevelItemCount() - 1 );

		QgsPoint p1( mTool->points().last() );
		double d = mDa.measureLine( p1, mLastMousePoint );

		mTotal = mDa.measureLine( mTool->points() );
		editTotal->setText( formatDistance( mTotal + d ) );

		QGis::UnitType displayUnits;
		// Meters or feet?
		convertMeasurement( d, displayUnits, false );

		QTreeWidgetItem *item = mTable->topLevelItem( mTable->topLevelItemCount() - 1 );
		item->setText( 0, QLocale::system().toString( d, 'f', mDecimalPlaces ) );
	}
}

void CMeasureDialog::on_buttonBox_rejected( void )
{
	restart();
	QDialog::close();
}

void CMeasureDialog::closeEvent( QCloseEvent *e )
{
	saveWindowLocation();
	e->accept();
}

void CMeasureDialog::restorePosition()
{
	QSettings settings;
	restoreGeometry( settings.value( "/Windows/Measure/geometry" ).toByteArray() );
	int wh;
	if ( mMeasureArea )
		wh = settings.value( "/Windows/Measure/hNoTable", 70 ).toInt();
	else
		wh = settings.value( "/Windows/Measure/h", 200 ).toInt();
	resize( width(), wh );
	updateUi();
}

void CMeasureDialog::saveWindowLocation()
{
	QSettings settings;
	settings.setValue( "/Windows/Measure/geometry", saveGeometry() );
	const QString &key = mMeasureArea ? "/Windows/Measure/hNoTable" : "/Windows/Measure/h";
	settings.setValue( key, height() );
}

QString CMeasureDialog::formatDistance( double distance )
{
	QSettings settings;
	bool baseUnit = settings.value( "/qgis/measure/keepbaseunit", false ).toBool();

	QGis::UnitType newDisplayUnits;
	convertMeasurement( distance, newDisplayUnits, false );
	return FormatDistance( distance, mDecimalPlaces, newDisplayUnits, baseUnit );
}


QString CMeasureDialog::formatArea( double area )
{
	QSettings settings;
	bool baseUnit = settings.value( "/qgis/measure/keepbaseunit", false ).toBool();

#if (VERSION_INT < 21601)

	QGis::UnitType newDisplayUnits;
	convertMeasurement( area, newDisplayUnits, true );

#else
	//TODO: review

	QgsUnitTypes::AreaUnit newDisplayUnits = QgsProject::instance()->areaUnits();
	area = mDa.convertAreaMeasurement( area, newDisplayUnits );		//QGIS 2.16.1 double QgsMeasureDialog::convertArea( double area, QgsUnitTypes::AreaUnit toUnit ) const

#endif

	return FormatArea( area, mDecimalPlaces, newDisplayUnits, baseUnit );
}


void CMeasureDialog::updateUi()
{
	// Set tooltip to indicate how we calculate measurments
	QString toolTip = tr( "The calculations are based on:" );
	if ( ! mTool->canvas()->hasCrsTransformEnabled() )
	{
		toolTip += "<br> * " + tr( "Project CRS transformation is turned off." ) + " ";
		toolTip += tr( "Canvas units setting is taken from project properties setting (%1)." ).arg( MapUnit2String( mCanvasUnits ) );
		toolTip += "<br> * " + tr( "Ellipsoidal calculation is not possible, as project CRS is undefined." );
		setWindowTitle( tr( "Measure (OTF off)" ) );
	}
	else
	{
		if ( mDa.ellipsoidalEnabled() )
		{
			toolTip += "<br> * " + tr( "Project CRS transformation is turned on and ellipsoidal calculation is selected." ) + " ";
			toolTip += "<br> * " + tr( "The coordinates are transformed to the chosen ellipsoid (%1), and the result is in meters" ).arg( mDa.ellipsoid() );
		}
		else
		{
			toolTip += "<br> * " + tr( "Project CRS transformation is turned on but ellipsoidal calculation is not selected." );
			toolTip += "<br> * " + tr( "The canvas units setting is taken from the project CRS (%1)." ).arg( MapUnit2String( mCanvasUnits ) );
		}
		setWindowTitle( tr( "Measure (OTF on)" ) );
	}

	if ( ( mCanvasUnits == QGis::Meters && mDisplayUnits == QGis::Feet ) || ( mCanvasUnits == QGis::Feet && mDisplayUnits == QGis::Meters ) )
	{
		toolTip += "<br> * " + tr( "Finally, the value is converted from %1 to %2." ).arg( MapUnit2String( mCanvasUnits ) ).arg( MapUnit2String( mDisplayUnits ) );
	}

	editTotal->setToolTip( toolTip );
	mTable->setToolTip( toolTip );
	mNotesLabel->setText( toolTip );

	QGis::UnitType newDisplayUnits;
	double dummy = 1.0;
	convertMeasurement( dummy, newDisplayUnits, true );
	mTable->setHeaderLabels( QStringList( tr( "Segments [%1]" ).arg( MapUnit2String( newDisplayUnits ) ) ) );

	if ( mMeasureArea )
	{
		double area = 0.0;
		if ( mTool->points().size() > 1 )
		{
			area = mDa.measurePolygon( mTool->points() );
		}
		mTable->hide(); // Hide the table, only show summary.
		editTotal->setText( formatArea( area ) );
	}
	else
	{
		QList<QgsPoint>::const_iterator it;
		bool b = true; // first point

		QgsPoint p1, p2;

		for ( it = mTool->points().constBegin(); it != mTool->points().constEnd(); ++it )
		{
			p2 = *it;
			if ( !b )
			{
				double d  = mDa.measureLine( p1, p2 );
				QGis::UnitType dummyUnits;
				convertMeasurement( d, dummyUnits, false );

				QTreeWidgetItem *item = new QTreeWidgetItem( QStringList( QLocale::system().toString( d, 'f', mDecimalPlaces ) ) );
				item->setTextAlignment( 0, Qt::AlignRight );
				mTable->addTopLevelItem( item );
				mTable->scrollToItem( item );
			}
			p1 = p2;
			b = false;
		}
		mTotal = mDa.measureLine( mTool->points() );
		mTable->show(); // Show the table with items
		editTotal->setText( formatDistance( mTotal ) );
	}
}

void CMeasureDialog::convertMeasurement( double &measure, QGis::UnitType &u, bool isArea )
{
	// Helper for converting between meters and feet
	// The parameter &u is out only...

	// Get the canvas units
	QGis::UnitType myUnits = mCanvasUnits;

	QgsDebugMsg( QString( "Preferred display units are %1" ).arg( EncodeUnit( mDisplayUnits ) ) );

	mDa.convertMeasurement( measure, myUnits, mDisplayUnits, isArea );
	u = myUnits;
}



///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_MapToolMeasureDialog.cpp"
