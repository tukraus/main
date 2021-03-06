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
#ifndef BRAT_QT_UTILS_H
#define BRAT_QT_UTILS_H

#if defined (QT_UTILS_H)
#error Wrong QtUtils.h included
#endif

#include <limits>

#include <qglobal.h>


#if QT_VERSION >= 0x050000
	#include <QtWidgets/QApplication>
	#include <QtWidgets/QFileDialog>
	#include <QtWidgets/QMessageBox>
	#include <QtWidgets/QTextEdit>
	#include <QtWidgets/QLayout>
	#include <QtWidgets/QSplitter>
	#include <QtWidgets/QToolBar>
	#include <QtWidgets/QMainWindow>
	#include <QtWidgets/QListWidget>
    #include <QtWidgets/QGroupBox>
	#include <QtWidgets/QToolButton>
	#include <QtWidgets/QPushButton>
	#include <QtWidgets/QActionGroup>
	#include <QtWidgets/QMenu>
	#include <QtWidgets/QInputDialog>
	#include <QtWidgets/QDesktopWidget>
    #include <QtGui/QDesktopServices>
    #include <QElapsedTimer>
#else
	#include <QtGui/QApplication>
	#include <QtGui/QFileDialog>
    #include <QtGui/QTextEdit>
    #include <QtGui/QMessageBox>
    #include <QtGui/QLayout>
    #include <QtGui/QSplitter>
    #include <QtGui/QToolBar>
    #include <QtGui/QMainWindow>
    #include <QtGui/QListWidget>
    #include <QtGui/QGroupBox>
    #include <QtGui/QToolButton>
    #include <QtGui/QPushButton>
    #include <QtGui/QActionGroup>
    #include <QtGui/QMenu>
    #include <QtGui/QInputDialog>
    #include <QtGui/QDesktopServices>
    #include <QtGui/QDesktopWidget>
#if QT_VERSION >= 0x040700
    #include <QElapsedTimer>
#endif
#endif
#include <QSettings>
#include <QResource>
#include <QThread>
#include <QUrl>


#include "QtUtilsIO.h"	// QtUtilsIO.h => QtStringUtils.h => +Utils.h


//Table of Contents
//                      QSettings Reader/Writer Helpers
//
//                      Wait Cursor MFC Style
//
//                      QString Conversions (in "QtStringUtils.h")
//							q2X conversions
//							X2q conversions
//
//						Simple Message Boxes
//
//						File System Utilities  (in "QtFileUtils.h")
//
//						GUI File System Utilities
//
//						Resources File System Utilities
//
//						Timer
//
//						Widget Creation Utilities
//




///////////////////////////////////////////////////////////////////////////
//                      QSettings Reader/Writer Helpers
///////////////////////////////////////////////////////////////////////////


template< typename T >
QSettings& QSettingsWriteArray( QSettings &s, const T &o, const std::string name )
{
    s.beginWriteArray( name.c_str() );
    size_t size = o.size();
    for ( size_t i = 0; i < size; ++i )
    {
        s.setArrayIndex( (int)i );
        s << o[i];
    }
    s.endArray();
    return s;
}

template< typename T >
inline void DefaultItemReader( QSettings &s, T &o, size_t i )  //QSettings cannot be const
{
    o.push_back( typename T::value_type() );
    s >> o[i];
}

template< typename T, typename READER >
const QSettings& QSettingsReadArray( QSettings &s, T &o, const std::string name, READER r = DefaultItemReader )  //QSettings cannot be const
{
    size_t size = s.beginReadArray( name.c_str() );
    for ( size_t i = 0; i < size; ++i )
    {
        s.setArrayIndex( (int)i );
        r( s, o, i );
    }
    s.endArray();
    return s;
}

///////////////////////////////////////////////////////////////////////////
//                      Wait Cursor MFC Style
///////////////////////////////////////////////////////////////////////////

class WaitCursor
{
public:
    WaitCursor()
    {
        QApplication::setOverrideCursor( Qt::WaitCursor );
    }

	void Restore()
	{
        QApplication::restoreOverrideCursor();
	}

    ~WaitCursor()
    {
		Restore();
    }
};



///////////////////////////////////////////////////////////////////
//                  Simple Message Boxes
///////////////////////////////////////////////////////////////////

// DO NOT DELETE: serve as reference for comparison with the implementation 
//					style below	(better, see comment there)
//
inline void SimpleMsgBox2( const QString &msg )
{
    QMessageBox msgBox;
	msgBox.setIcon( QMessageBox::Information );
	msgBox.setText( msg );
    msgBox.exec();
}
template< class STRING >
inline void SimpleMsgBox2( const STRING &msg )
{
    SimpleMsgBox2( t2q( msg ) );
}



inline QWidget* SetApplicationWindow( QWidget *w = nullptr )
{
    static QWidget *m = w;
    return m;
}

inline QWidget* ApplicationWindow()
{
    static QWidget *m = SetApplicationWindow();
    return m ? m : qApp->activeWindow();
}

inline void SimpleAbout( const std::string &version, const std::string &proc_arch, const std::string &copy_right, std::string app_name = std::string() )
{
	if ( app_name.empty() )
		app_name = q2a( QCoreApplication::applicationName() );

	std::string title = "About " + app_name;

	QString msg = QString( "Welcome to " ) + app_name.c_str() + " " + version.c_str() + " - " + proc_arch.c_str() + "\n(C)opyright " + copy_right.c_str();

	QMessageBox::about( ApplicationWindow(), QObject::tr( title.c_str() ), msg );
}

inline bool SimpleSystemOpenFile( const std::string &path )
{
	return QDesktopServices::openUrl( QUrl( ( "file:///" + path ).c_str(), QUrl::TolerantMode ) );
}

//	The implementations style of these functions is preferable to the one above (SimpleMsgBox2),
//	not only because they are simpler but also because the appropriate icon is automatically set 
//	not only in the dialog body but also the application icon is set in the frame: the one above
//	has the default NULL icon in the frame.
//	If you get bored of having the redundant titles below in the dialogs frame, just pass nullptr 
//	instead of the title string and you'll get the application name as title.
//
inline QMessageBox::StandardButton SimpleMsgBox( const QString &msg )
{
    return QMessageBox::information( ApplicationWindow(), "Information", msg );
}
template< class STRING >
inline QMessageBox::StandardButton SimpleMsgBox( const STRING &msg )
{
	return SimpleMsgBox( t2q( msg ) );
}
inline void SimpleMsgBox( const char *msg )
{
	SimpleMsgBox( QString( msg ) );
}
											 

inline QMessageBox::StandardButton SimpleErrorBox( const QString &msg )
{
    return QMessageBox::critical( ApplicationWindow(), "Error", msg );
}
template< class STRING >
inline QMessageBox::StandardButton SimpleErrorBox( const STRING &msg )
{
	return SimpleErrorBox( t2q( msg ) );
}
inline bool SimpleErrorBox( const char *msg )
{
	return SimpleErrorBox( QString( msg ) );
}


inline QMessageBox::StandardButton SimpleWarnBox( const QString &msg )
{
    return QMessageBox::warning( ApplicationWindow(), "Warning", msg );
}
template< class STRING >
inline QMessageBox::StandardButton SimpleWarnBox( const STRING &msg )
{
	return SimpleWarnBox( t2q( msg ) );
}
inline bool SimpleWarnBox( const char *msg )
{
	return SimpleWarnBox( QString( msg ) );
}


inline bool SimpleQuestion( const QString &msg )
{
    return QMessageBox::warning( ApplicationWindow(), "Question", msg, QMessageBox::Yes | QMessageBox::No ) == QMessageBox::Yes;
}
template< class STRING >
inline bool SimpleQuestion( const STRING &msg )
{
	return SimpleQuestion( t2q( msg ) );
}
inline bool SimpleQuestion( const char *msg )
{
	return SimpleQuestion( QString( msg ) );
}




///////////////////////////////////////////////////////////////////
//                  Simple Input Boxes
///////////////////////////////////////////////////////////////////


inline std::pair< bool, QString > SimpleInputString( const QString &input_name, const QString &init_value, const QString &dialog_title = "" )
{
	std::pair< bool, QString > result( { false, init_value } );

    QString text = QInputDialog::getText( ApplicationWindow(),
		dialog_title, 
		input_name, 
		QLineEdit::Normal, 
		result.second,
        &result.first );

    if ( result.first )
        result.second = text;

	return result;
}

inline std::pair< bool, std::string > SimpleInputString( const std::string &input_name, const std::string &init_value, const std::string &dialog_title = "" )
{
	std::pair< bool, QString > qresult = SimpleInputString( t2q( input_name ), t2q( init_value ), t2q( dialog_title ) );

	return std::pair< bool, std::string >( { qresult.first, q2a( qresult.second ) } );
}


// Validate Input String (to be used on rename objects such as filters, operations...)
//
inline std::pair< bool, std::string > ValidatedInputString( const std::string &input_name, const std::string &init_value, const std::string &dialog_title = "" )
{
    std::pair< bool, QString > qresult = SimpleInputString( t2q( input_name ), t2q( init_value ), t2q( dialog_title ) );

    /////////////////////////////////////////////////
    // Check if text has only alphanumeric letters //
    /////////////////////////////////////////////////
    static QRegExp re("[_a-zA-Z0-9]+"); // alphanumeric letters
    if ( !re.exactMatch( qresult.second ) && qresult.first ) // Has an Invalid Name
    {
        SimpleWarnBox( QString( "Unable to rename '%1' by '%2'.\nPlease enter only alphanumeric letters, 'A-Z' or '_a-z' or '0-9'.").arg(
                                 t2q(init_value), qresult.second ) );
        qresult.first = false;
        qresult.second = t2q(init_value);
    }

    return std::pair< bool, std::string >( { qresult.first, q2a( qresult.second ) } );
}



template< typename VALUE > 
struct input_number_traits;

template<> 
struct input_number_traits< int >
{
	static decltype( &QInputDialog::getInt ) function()
	{
		return QInputDialog::getInt;
	}
};

template<> 
struct input_number_traits< double >
{
	static decltype( &QInputDialog::getDouble ) function()
	{
		return QInputDialog::getDouble;
	}
};


template< typename VALUE >
inline std::pair< bool, VALUE > SimpleInputNumber( const QString &input_name, const VALUE &init_value, const QString &dialog_title )
{
	std::pair< bool, VALUE > result( { false, init_value } );

    VALUE value = (*input_number_traits<VALUE>::function())( ApplicationWindow(),
		dialog_title, 
		input_name, 
		result.second,
		std::numeric_limits< VALUE >::min(),
		std::numeric_limits< VALUE >::max(),
		(VALUE)1,
		&result.first,
		(Qt::WindowFlags)0 );

    if ( result.first )
        result.second = value;

	return result;
}


///////////////////////////////////////////////////////////////////////////
//						Widget Creation Utilities
///////////////////////////////////////////////////////////////////////////


const int icon_size = 24;
const int tool_icon_size = 20;


//////////
// generic
//////////

inline void SetObjectName( QObject *w, const std::string &name )
{
	static int index = 0;

    w->setObjectName( QString::fromUtf8( ( name + n2s< std::string >( ++index ) ).c_str() ) );
}


//////////
// space
//////////

inline QSpacerItem* CreateSpace( int w = 0, int h = 0, QSizePolicy::Policy hData = QSizePolicy::Expanding, QSizePolicy::Policy vData = QSizePolicy::Expanding )
{
	return new QSpacerItem( w, h, hData, vData );
}


//////////
//layouts
//////////

const int default_spacing = 2;
const int default_left = 2;
const int default_top = 2;
const int default_right = 2;
const int default_bottom = 2;



template< class LAYOUT >
inline LAYOUT* FinishLayout( LAYOUT *l, int spacing, int left, int top, int right, int bottom )
{
	l->setSpacing( spacing );
    SetObjectName( l, "boxLayout" );
	l->setContentsMargins( left, top, right, bottom );
	return l;
}


inline QBoxLayout* CreateLayout( QWidget *parent, Qt::Orientation orientation, int spacing = 0, int left = 0, int top = 0, int right = 0, int bottom = 0 )
{
	QVBoxLayout *vboxLayout;
	QHBoxLayout *hboxLayout;
	QBoxLayout *l;
	if ( orientation == Qt::Vertical ){
		l = vboxLayout = new QVBoxLayout( parent );
	} else {
		l = hboxLayout = new QHBoxLayout( parent );
	} 
	return FinishLayout< QBoxLayout >( l, spacing, left, top, right, bottom );
}


inline QGridLayout* CreateGridLayout( QWidget *parent, int spacing = 0, int left = 0, int top = 0, int right = 0, int bottom = 0 )
{
	return FinishLayout< QGridLayout >( new QGridLayout( parent ), spacing, left, top, right, bottom );
}



////////////////////////////////////////////////////////////
// C++11 only //////////////////////////////////////////////

#if !defined(PRE_CPP11)


inline QBoxLayout* LayoutWidgets( Qt::Orientation o, const std::vector< QObject* > &v, 
	QWidget *parent, int spacing, int left, int top, int right, int bottom )			
{
	QBoxLayout *main_l = CreateLayout( parent, o, spacing, left, top, right, bottom );
	for ( auto ob : v )
	{
		if ( !ob )
		{
			main_l->addItem( CreateSpace() );
			continue;
		}

		auto w = qobject_cast<QWidget*>( ob );
		auto l = qobject_cast<QLayout*>( ob );		assert__( w || l );
		if ( w )
			main_l->addWidget( w );
		else
			main_l->addLayout( l );
	}
	return main_l;
}


inline QGridLayout* LayoutWidgets( const std::vector< QObject* > &v, 
	QWidget *parent, int spacing, int left, int top, int right, int bottom,
	int row_span = 1, int col_span = 1 )
{
	QGridLayout *main_l = CreateGridLayout( parent, spacing, left, top, right, bottom );
	int line = 0, col = 0;
	for ( auto ob : v )
	{
		if ( !ob )
		{
			++line;
			col = 0;
		}
		else
		{
			auto w = qobject_cast<QWidget*>( ob );
			auto l = qobject_cast<QLayout*>( ob );		assert__( w || l );
			if ( w )
				main_l->addWidget( w, line, col, row_span, col_span );
			else
				main_l->addLayout( l, line, col, row_span, col_span );

			++col;
		}
	}
	return main_l;
}


////////////////////
//layout group-box
////////////////////


// For use with CreateGroupBox
//
enum class ELayoutType
{
	Horizontal = Qt::Horizontal,
	Vertical = Qt::Vertical,
	Grid,

	ELayoutType_size
};


//	spacing and coordinates refer to enclosed layout
//
template< class GROUP_BOX = QGroupBox >
GROUP_BOX* CreateGroupBox( ELayoutType o, const std::vector< QObject* > &v, const QString &title = "", QWidget *parent = nullptr,
	int spacing = default_spacing, int left = default_left, int top = default_top, int right = default_right, int bottom = default_bottom )
{
	auto group = new GROUP_BOX( title, parent );
	SetObjectName( group, parent ? q2a( parent->objectName() ) : "group_box" );

	for ( auto ob : v )
	{
		if ( !ob )
			continue;

		// QObject and QWidget setParent are not polymorphic
		//
		auto w = qobject_cast<QWidget*>( ob );
		auto l = qobject_cast<QLayout*>( ob );		assert__( w || l );
		if ( w )
			w->setParent( group );
		else
			l->setParent( nullptr );
	}

	switch ( o )
	{
		case ELayoutType::Horizontal:
		case ELayoutType::Vertical:
			LayoutWidgets( static_cast< Qt::Orientation >( o ), v, group, spacing, left, top, right, bottom  );
			break;

		case ELayoutType::Grid:
			LayoutWidgets( v, group, spacing, left, top, right, bottom  );
			break;

		default:
			assert__( false );
	}

	return group;
}


///////////
// position
///////////

inline QWidget* CenterOnParentCenter( QWidget *const w, QPoint pcenter )
{
	w->move( pcenter - w->rect().center() );

    return w;
}

inline QWidget* CenterOnScreen( QWidget *const w )
{
	assert__( qApp );

	return CenterOnParentCenter( w, qApp->desktop()->availableGeometry().center() );
}

inline QWidget* CenterOnScreen2( QWidget *const w )
{
	assert__( qApp );

    w->setGeometry( QStyle::alignedRect( Qt::LeftToRight, Qt::AlignCenter, w->size(), qApp->desktop()->availableGeometry() ) );

    return w;
}

inline QWidget* CenterOnScreen3( QWidget *const w )
{
    QRect screenGeometry = QApplication::desktop()->screenGeometry();
    int x = ( screenGeometry.width() - w->width() ) / 2;
    int y = ( screenGeometry.height() - w->height() ) / 2;
    w->move(x, y);

    return w;
}


///////////
// widgets
///////////

// widget is added to parent's layout, which is created if necessary
//
inline QWidget* AddWidget( QWidget *parent, QWidget *component )
{ 
	QLayout *l = parent->layout();
	if ( !l )
		l = CreateLayout( parent, Qt::Vertical );

	l->addWidget( component );
	return parent;
}


inline QFrame* WidgetLine( QWidget *parent, Qt::Orientation o )
{
    QFrame *line = new QFrame( parent );
    SetObjectName( line, "line" );
    line->setSizePolicy(
        o == Qt::Horizontal ? QSizePolicy::Expanding : QSizePolicy::Minimum,
        o == Qt::Horizontal ? QSizePolicy::Minimum : QSizePolicy::Expanding );
    line->setFrameStyle( ( o == Qt::Horizontal ? QFrame::HLine : QFrame::VLine ) | QFrame::Sunken );

    return line;
}




///////////
//text
///////////

inline const QString& TextType2String( const QTextEdit * )
{
	static const QString s = "QTextEdit";
	return s;
}
inline const QString& TextType2String( const QLineEdit * )
{
	static const QString s = "QLineEdit";
	return s;
}


template< typename TEXT_EDITOR >
inline void SetReadOnlyEditor( TEXT_EDITOR *ed, bool ro )
{
	if ( ro )
		ed->setStyleSheet( TextType2String( ed ) + " {background: #DFDFDF; color: #000000;}" );
	else
		ed->setStyleSheet("");
	ed->setReadOnly( ro );
}



///////////
//splitter
///////////

inline QSplitter* CreateSplitter( QWidget *parent, Qt::Orientation orientation )
{
    QSplitter *s = new QSplitter( orientation, parent );
    SetObjectName( s, "splitter" );
	return s;
}
inline QSplitter* CreateSplitterIn( QWidget *parent, Qt::Orientation orientation )
{
    QSplitter *s = CreateSplitter( parent, orientation );
	if ( !parent->layout() )
			CreateLayout( parent, orientation );
    AddWidget( parent, s );
	return s;
}
inline QSplitter* CreateSplitterIn( QSplitter *parent, Qt::Orientation orientation )
{
    QSplitter *s = CreateSplitter( parent, orientation );
    parent->addWidget( s );
	return s;
}
inline QSplitter* CreateSplitterIn( QMainWindow *parent, Qt::Orientation orientation )
{
	QSplitter *s = CreateSplitter( parent, orientation );
	parent->setCentralWidget( s );
	return s;
}
inline QSplitter* CreateSplitter( QWidget *parent, Qt::Orientation o, const std::vector< QWidget* > &v, 
	bool collapsible = false, const QList< int > sizes = QList< int >() )
{
	QSplitter *splitter = CreateSplitter( parent, o );
	int index = 0;
	for ( auto *w : v )
	{
		splitter->addWidget( w );
		splitter->setCollapsible( index++, collapsible );
	}
	if ( !sizes.isEmpty() )
		splitter->setSizes( sizes );

	return splitter;
}






///////////
// tool-bar
///////////

//	Destroys any existing layout, so it must be called before any other
//	widgets/layouts are inserted in w
//
//	(*) See IMPORTANT note in	OSGGraphicEditor constructor or build
//
inline void insertToolBar( QWidget *w, QToolBar *toolbar, Qt::ToolBarArea area )
{
    Qt::Orientation	orientation = Qt::Horizontal;   //initializarion to silent compiler warnings in release mode
    switch ( area )
    {
        case Qt::LeftToolBarArea:
        case Qt::RightToolBarArea:
        {
            orientation = Qt::Horizontal;
            toolbar->setOrientation( Qt::Vertical );
            break;
        }
        case Qt::TopToolBarArea:
        case Qt::BottomToolBarArea:
            orientation = Qt::Vertical;
            toolbar->setOrientation( Qt::Horizontal );
            break;
        default:
            assert__( false );
    }

    QLayout *l = w->layout();
    {//(*)
        if ( l )
            delete l;
        l = CreateLayout( w, orientation, 1, 1, 1, 1, 1 );	//(*)
    }
    l->addWidget( toolbar );
}
inline void insertToolBar( QMainWindow *w, QToolBar *toolbar, Qt::ToolBarArea area )
{
    w->addToolBar( area, toolbar );
}


////////////////////
// actions / buttons
////////////////////

// see also ActionsTable.*


inline QToolButton* CreateToolButton( const std::string &name, const std::string &pix_path, const std::string &tip  = "", bool auto_raise = false )
{
	QToolButton *button = new QToolButton;
	button->setAutoRaise( auto_raise );
	if ( !name.empty() )
	{
		button->setText( name.c_str() );
		button->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
	}
	if (!tip.empty() )
		button->setToolTip( tip.c_str() );
	QPixmap pix( pix_path.c_str() );
	QIcon icon(pix);	
	button->setIcon(icon);
	button->setIconSize(QSize(tool_icon_size, tool_icon_size));

	return button;
}


inline QPushButton* CreatePushButton( const std::string &name, const std::string &pix_path, const std::string &tip  = "" )
{
	QPushButton *button = new QPushButton;
	if ( !name.empty() )
	{
		button->setText( name.c_str() );
	}
	if (!tip.empty() )
		button->setToolTip( tip.c_str() );
	if ( !pix_path.empty() )
	{
		QPixmap pix( pix_path.c_str() );
		QIcon icon( pix );
		button->setIcon( icon );
		button->setIconSize( QSize( tool_icon_size, tool_icon_size ) );
	}
	return button;
}


inline QActionGroup* CreateActionGroup( QObject *parent, bool exclusive )
{
	QActionGroup *group = new QActionGroup( parent );
	group->setExclusive( exclusive );
	return group;
}


inline QActionGroup* CreateActionGroup( QObject *parent, const QList<QAction*> &actions, bool exclusive )
{
	QActionGroup *group = CreateActionGroup( parent, exclusive );
	for ( auto *action : actions )
	{
		group->addAction( action );
		action->setParent( group );					//to find in group
		action->setObjectName( action->text() );	//to find in group
		action->setCheckable( exclusive );
	}
	return group;
}


inline QActionGroup* CreateActionGroup( QObject *parent, std::initializer_list<QAction*> actions, bool exclusive )
{
	QList<QAction*> list;
	for ( auto *a : actions )
		list << a;
	return CreateActionGroup( parent, list, exclusive );
}


inline QToolButton* CreateMenuButton( const std::string &name, const std::string &pix_path, const std::string &tip, const QList<QAction*> &actions )
{
	QMenu *menu = new QMenu();
	for ( auto a : actions )
		menu->addAction( a );

	QToolButton *toolButton = CreateToolButton( name, pix_path, tip ); 		assert__( toolButton );
	toolButton->setMenu( menu );
	toolButton->setPopupMode( QToolButton::InstantPopup );

	return toolButton;
}


inline QToolButton* CreateMenuButton( const std::string &name, const std::string &pix_path, const std::string &tip, 
	std::initializer_list<QAction*> actions )
{
	QList<QAction*> list;
	for ( auto *a : actions )
		list << a;
	return CreateMenuButton( name, pix_path, tip, list );
}


inline QToolButton* CreatePopupButton( const std::string &name, const std::string &pix_path, const std::string &tip, const QList<QAction*> &actions, QAction *default_action = nullptr )
{

	QToolButton *tool_button = CreateToolButton( name, pix_path, tip, false ); 		assert__( tool_button );
	for ( auto a : actions )
		tool_button->addAction( a );
	tool_button->setPopupMode( QToolButton::MenuButtonPopup );
	if ( default_action )
		tool_button->setDefaultAction( default_action );

	return tool_button;
}


inline QWidget* CreateButtonRow( bool exclusive, Qt::Orientation o, const std::vector< QObject* > &v, int spacing = 2, int margins = 2 )
{
	QWidget *buttons_row = new QWidget;

	for ( auto *obj : v )
	{
		auto *button = qobject_cast<QAbstractButton*>( obj );
		if ( !button )
			continue;

		if ( exclusive )
		{
			button->setCheckable( true );
			button->setAutoExclusive( true );
		}

		//compensate any button width/height differences

		button->setSizePolicy(
			o == Qt::Vertical ? QSizePolicy::Expanding : QSizePolicy::Maximum,
			o == Qt::Vertical ? QSizePolicy::Maximum : QSizePolicy::Expanding );
	}

	LayoutWidgets( o, v, buttons_row, spacing, margins, margins, margins, margins );
	buttons_row->setSizePolicy(
		o == Qt::Horizontal ? QSizePolicy::Expanding : QSizePolicy::Maximum,
		o == Qt::Horizontal ? QSizePolicy::Maximum : QSizePolicy::Expanding );

	return buttons_row;
}





///////////
//combo box
///////////

inline const QString& qidentity( const QString &s ){  return s; }


template <
    typename COMBO, typename CONTAINER, typename FUNC = decltype( qidentity )
>
inline void FillCombo( COMBO *c, const CONTAINER &items, int selected = 0, bool enabled = true, const FUNC &f = qidentity )
{
	c->clear();				assert__( c->currentIndex() == -1 );
	if ( selected > 0 )
		c->blockSignals( true );

    for ( auto i : items )
    {
        c->addItem( f( i ) );
    }

	if ( selected > 0 )
	{
		c->blockSignals( false );
		c->setCurrentIndex( selected );
	}
    c->setEnabled( enabled );
}


template< typename COMBO >
inline void FillCombo( COMBO *c, const std::vector<std::string> &names, int selected, bool enabled )
{
	FillCombo( c, names, selected, enabled,

		[]( const std::string &s ) -> const char*
        {
			return s.c_str();
		}

		);
}

//NOTE: this is a template only for the compiler not to error "use of undefined type" in the poor moc files
//
template< typename COMBO >
inline void FillCombo( COMBO *c, const std::string *names, size_t size, int selected, bool enabled )
{
	FillCombo( c, std::vector< std::string >( &names[0], &names[size] ),  selected, enabled );
}



//////////////////////
//list widget types
//////////////////////

template <
    typename LIST_TYPE, typename CONTAINER, typename ENUM_VALUE, typename FUNC = decltype( qidentity )
>
inline LIST_TYPE* FillAnyList( LIST_TYPE *c, const CONTAINER &items, std::initializer_list< ENUM_VALUE > selection, bool enable, const FUNC &f = qidentity )
{
	for ( auto const &item : items ) 
	{
		c->addItem( f( item ) );
	}

	for ( auto selected : selection )
		if ( selected >= 0 )
		{
			c->item( selected )->setSelected( true );
			c->setCurrentRow( selected );			 //setCurrentRow must be called for all selected items; if called once, deselects all others
		}
		else
			c->setCurrentRow( -1 );

	c->setEnabled( enable );

	return c;
}


template< typename LIST_TYPE, typename CONTAINER, typename FUNC = decltype( qidentity ) >
inline LIST_TYPE* FillList( LIST_TYPE *c, const CONTAINER &items, int selection, bool enable, const FUNC &f = qidentity )
{
	return FillAnyList< LIST_TYPE, CONTAINER, int >( c, items, { selection }, enable, f );
}


template < typename LIST_TYPE >
inline QListWidget* FillList( LIST_TYPE *c, const std::vector<std::string> &items, int selection, bool enable )
{
	return FillAnyList< LIST_TYPE, std::vector<std::string>, int >( c, items, { selection }, enable, 

		[]( const std::string &s ) -> const char*
        {
			return s.c_str();
		}

		);
}




//////////////////////
//table widget
//////////////////////


// This is not always accurate.	resizeColumnsToContents should be
//	called if the returned width is used to resize the table and 
//	all width must be visible; but this is not desirable for big 
//	data.
//	To be improved with empiric experience.
//
template< class TABLE >
inline int ComputeTableWidth( TABLE *t )
{
	//t->resizeColumnsToContents();
	t->resize( 1, 1 );
	auto const vmode = t->verticalScrollMode();
	auto const hmode = t->horizontalScrollMode();
	t->setVerticalScrollMode( QAbstractItemView::ScrollPerPixel );		//vertical must be done first...
	t->setHorizontalScrollMode( QAbstractItemView::ScrollPerPixel );	//...and horizontal must also be done, even if only vertical scrollBar size matters

	int width = t->contentsMargins().left() + t->contentsMargins().right() + t->verticalScrollBar()->width();
	if ( t->verticalHeader()->isVisible() )
		width += t->verticalHeader()->width();
    const int columns = t->columnCount();
	for ( auto i = 0; i < columns; ++i )
		width += t->columnWidth( i );

	t->setVerticalScrollMode( vmode );
	t->setHorizontalScrollMode( hmode );
	return width;
}


// This was based on ComputeTableWidth but is (even more) less tested.
//	To be improved also with empiric experience.
//
template< class TABLE >
inline int ComputeTableHeight( TABLE *t )
{
	//t->resizeRowsToContents();
	t->resize( 1, 1 );
	auto const vmode = t->verticalScrollMode();
	auto const hmode = t->horizontalScrollMode();
	t->setVerticalScrollMode( QAbstractItemView::ScrollPerPixel );
	t->setHorizontalScrollMode( QAbstractItemView::ScrollPerPixel );

	int height = t->contentsMargins().top() + t->contentsMargins().bottom() + t->horizontalScrollBar()->height();
	if ( t->horizontalHeader()->isVisible() )
		height += t->horizontalHeader()->height();
    const int rows = t->rowCount();
	for ( auto i = 0; i < rows; ++i )
		height += t->rowHeight( i );

	t->setVerticalScrollMode( vmode );
	t->setHorizontalScrollMode( hmode );
	return height;
}




//////////////////////
//	dialog box
//////////////////////

inline void SetMaximizableDialog( QDialog *d )
{
#if defined (_WIN32) || defined (WIN32)
	// Show maximize button in windows
	// If this is set in linux, it will not center the dialog over parent
	d->setWindowFlags( ( d->windowFlags() & ~Qt::Dialog ) | Qt::Window | Qt::WindowMaximizeButtonHint );
#elif defined (Q_OS_MAC)
	// Qt::WindowStaysOnTopHint also works (too weel: stays on top of other apps also). Without this, we have the mac MDI mess...
	d->setWindowFlags( ( d->windowFlags() & ~Qt::Dialog ) | Qt::Tool );
#endif
}



///////////////////////////////////////////////////////////////////////////
//                      File System GUI Utilities
///////////////////////////////////////////////////////////////////////////


inline QString BrowseDirectory( QWidget *parent, const char *title, QString InitialDir )
{
    static QString lastDir = InitialDir;

    if ( InitialDir.isEmpty() )
        InitialDir = lastDir;
    QFileDialog::Options options = QFileDialog::DontResolveSymlinks | QFileDialog::ShowDirsOnly;
    //if (!native->isChecked())
    options |= QFileDialog::DontUseNativeDialog;
    QString dir = QFileDialog::getExistingDirectory( parent, QObject::tr( title ), InitialDir, options );
    if ( !dir.isEmpty() )
        lastDir = dir;
    return dir;
}

inline QStringList getOpenFileNames( QWidget * parent = 0, const QString & caption = QString(),
                              const QString & dir = QString(), const QString & filter = QString(),
                              QString * selectedFilter = 0, QFileDialog::Options options = 0 )
{
    return QFileDialog::getOpenFileNames( parent, caption, dir, filter, selectedFilter, options );
}

inline QStringList getOpenFileName( QWidget * parent = 0, const QString & caption = QString(),
                             const QString & dir = QString(), const QString & filter = QString(),
                             QString * selectedFilter = 0, QFileDialog::Options options = 0 )
{
    QStringList result;
    result.append( QFileDialog::getOpenFileName( parent, caption, dir, filter, selectedFilter, options ) );
    return result;
}

inline QStringList getSaveFileName( QWidget * parent = 0, const QString & caption = QString(),
                             const QString & dir = QString(), const QString & filter = QString(),
                             QString * selectedFilter = 0, QFileDialog::Options options = 0 )
{
    QStringList result;
    result.append( QFileDialog::getSaveFileName( parent, caption, dir, filter, selectedFilter, options ) );
    return result;
}

template< typename F >
QStringList tBrowseFileWithExtension( F f, QWidget *parent, const char *title, QString Initial, const QString &extension, const QString &description = "" )
{
    QFileDialog::Options options;
    QString selectedFilter;
	QString desc = description.isEmpty() ? extension : description;
    QString filter = desc + " (*." + extension + ");;" + "All Files (*)";
    return f( parent, QObject::tr( title ), Initial, filter, &selectedFilter, options);
}

template< typename F >
QStringList tBrowseFile( F f, QWidget *parent, const char *title, QString Initial )
{
    QFileDialog::Options options;
    //if (!native->isChecked())
    //    options |= QFileDialog::DontUseNativeDialog;
    QString selectedFilter;

    return f( parent, QObject::tr( title ), Initial, QObject::tr("All Files (*);;Text Files (*.txt)"), &selectedFilter, options);
}

inline QString BrowseFile( QWidget *parent, const char *title, QString Initial )
{
    return tBrowseFile( getOpenFileName, parent, title, Initial )[0];
}

inline QString BrowseFileWithExtension( QWidget *parent, const char *title, QString Initial, const QString &extension, const QString &description = "" )
{
    return tBrowseFileWithExtension( getOpenFileName, parent, title, Initial, extension, description )[0];
}

inline QString BrowseNewFile( QWidget *parent, const char *title, QString Initial )
{
    return tBrowseFile( getSaveFileName, parent, title, Initial )[0];
}

inline QStringList BrowseFiles( QWidget *parent, const char *title, QString Initial )
{
    return tBrowseFile( getOpenFileNames, parent, title, Initial );
}



///////////////////////////////////////////////////////////////////////////
// Resources "file system" utilities
///////////////////////////////////////////////////////////////////////////


inline bool readFileFromResource( const QString &rpath, QString &dest, bool unicode = false )
{
    QResource r( rpath );
    if ( !r.isValid() )
        return false;
    QByteArray ba( reinterpret_cast< const char* >( r.data() ), (int)r.size() );
    QByteArray data;
    if ( r.isCompressed() )
        data = qUncompress( ba );
    else
        data = ba;

    if ( unicode )
        dest.setUtf16( (const ushort*)(const char*)data, data.size() / sizeof(ushort) );
    else
        dest = data;

    return true;
}

template< typename STRING >
inline bool readFileFromResource( const QString &rpath, STRING &dest, bool unicode = false )
{
    QString str;
    if ( !readFileFromResource( rpath, str, unicode ) )
        return false;
    dest = q2t< STRING >( str );
    return true;
}

inline bool readUnicodeFileFromResource( const QString &rpath, std::wstring &dest )
{
    return readFileFromResource( rpath, dest, true );
}



//////////////////////////////////////////////////////////////////
//					Dimensions - BRAT Specific
//////////////////////////////////////////////////////////////////

inline QSize DefaultSizeHint( const QWidget *w )
{
	return QSize( 72 * w->fontMetrics().width( 'x' ), 25 * w->fontMetrics().lineSpacing() );
}



const int min_main_window_width = 1024;
const int min_main_window_height = 728;

const auto hchild_ratio = 2. / 3.;
const auto aspect_ratio = 1. / 2.;	// 3. / 4.;

const int min_globe_widget_width =  min_main_window_width * hchild_ratio;
const int min_globe_widget_height = min_globe_widget_width * aspect_ratio;

const int min_plot_widget_width = min_globe_widget_width;
const int min_plot_widget_height = min_globe_widget_height;


const int min_editor_dock_width = 200;
const int min_editor_dock_height = 10;

const int min_readable_combo_width = 90;






#endif		//if !defined(PRE_CPP11)
// C++11 only - block end //////////////////////////////////
////////////////////////////////////////////////////////////







//////////////////////////////////////////////////////////////////
//	Development only
//////////////////////////////////////////////////////////////////

inline void NotImplemented( const char *msg = nullptr )
{
	SimpleMsgBox( std::string( "Not implemented. " ) + ( msg ? msg : "" ) );
}

#define BRAT_NOT_IMPLEMENTED  NotImplemented( __func__ );
#define BRAT_MSG_NOT_IMPLEMENTED( x )  NotImplemented( std::string( x ).c_str() );






//////////////////////////////////////////////////////////////////
//						OS Standard Paths
//////////////////////////////////////////////////////////////////
//
//in this file and not IO because include GUI


inline const std::string& SystemUserDocumentsPath()
{
#if QT_VERSION >= 0x050000
	static const std::string docs = q2a( QStandardPaths::writableLocation( QStandardPaths::DocumentsLocation ) );
#else
	static const std::string docs = q2a( QDesktopServices::storageLocation( QDesktopServices::DocumentsLocation ) );
#endif

    assert__( IsDir( docs ) );

	return docs;
}

// If applicationName and/or organizationName are set, are used by the framework as sub-directories.
// If parameter root is false (the default), this function always removes the application 
//  sub-directory, if it exists; so, for different applications of the same "organization" 
//	(or of "no organization") the returned path is the same in the same machine.
//
inline std::string ComputeSystemUserDataPath( bool root = false )
{
#if QT_VERSION >= 0x050000

	std::string data = q2a( QStandardPaths::writableLocation( QStandardPaths::GenericDataLocation ) );
	if ( !root && !QCoreApplication::organizationName().isEmpty() )
		data += ( "/" + q2a( QCoreApplication::organizationName() ) );

#else
	std::string data = q2a( QDesktopServices::storageLocation( QDesktopServices::DataLocation ) );

	if ( !QCoreApplication::applicationName().isEmpty() )
		data = GetDirectoryFromPath( data );
	if ( root && !QCoreApplication::organizationName().isEmpty() && !QCoreApplication::applicationName().isEmpty() )
		data = GetDirectoryFromPath( data );

#endif

	return data;
}

inline const std::string& SystemUserDataPath()
{
	static const std::string data = ComputeSystemUserDataPath();		assert__( IsDir( GetDirectoryFromPath( data ) ) );
	return data;
}


//////////////////////////////////////////////////////////////////
//	Candidates
//////////////////////////////////////////////////////////////////

//inline int ItemRow( QListWidgetItem *item )
//{
//	return item->listWidget()->row( item );
//}
//inline int FindText( QListWidget *list, const char *text  )
//{
//    auto items = list->findItems( text, Qt::MatchExactly );			assert__( items.size() == 1 );
//	return ItemRow( items[ 0 ] );
//}
//inline QString ItemText( QListWidget *list, int index  )
//{
//	auto *item = list->item( index );
//	return item->text();
//}


inline QString UserName()
{
	QString name = qgetenv( "USER" );
	if ( name.isEmpty() )
		name = qgetenv( "USERNAME" );

	return name;
}



class QBratThread : public QThread
{
public:
	static void sleep( unsigned long secs ) 
	{
		QThread::sleep( secs );
	}

	static void msleep( unsigned long msecs ) 
	{
		QThread::msleep( msecs );
	}

	static void usleep( unsigned long usecs ) 
	{
		QThread::usleep( usecs );
	}
};




inline bool IsLinuxDesktop( const std::string &desktop )
{
    //Tested only in debian 7
    //DESKTOP_SESSION=kde-plasma
    //DESKTOP_SESSION=gnome

#if defined (Q_OS_LINUX)
    const char *DESKTOP_SESSION = getenv( "DESKTOP_SESSION" );
    return DESKTOP_SESSION && ( std::string( DESKTOP_SESSION ).find( desktop ) != std::string::npos );
#else

    Q_UNUSED( desktop )

    return false;
#endif
}
inline bool IsGnomeDesktop()
{
    return IsLinuxDesktop( "gnome" );
}
inline bool IsKDEDesktop()
{
    return IsLinuxDesktop( "kde" );
}



#endif		// BRAT_QT_UTILS_H
