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

#include "new-gui/Common/GUI/TextWidget.h"

#include "EditExportAsciiDialog.h"



void  CEditExportAsciiDialog::CreateWidgets()
{
    //	Text

	mTextEdit = new CTextWidget;
	mTextEdit->SetSizeHint( mTextEdit->sizeHint().width(), 2 * mTextEdit->sizeHint().height() );
	auto *content_l = LayoutWidgets( Qt::Vertical, { mTextEdit }, nullptr, 2, 2, 2, 2, 2 );


    //	... Help

    auto help = new CTextWidget;
    help->SetHelpProperties( "Exported ASCII file for operation " + t2q( mOperation->GetName() ) + ".", 0 , 6 );
    auto help_group = CreateGroupBox( ELayoutType::Grid, { help }, "", nullptr, 6, 6, 6, 6, 6 );
    help_group->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Maximum );


    //	... Buttons

    mButtonBox = new QDialogButtonBox( this );
    mButtonBox->setObjectName( QString::fromUtf8( "mButtonBox" ) );
    mButtonBox->setOrientation( Qt::Horizontal );
    mButtonBox->setStandardButtons( QDialogButtonBox::Close );
    mButtonBox->button( QDialogButtonBox::Close )->setDefault( true );

    QBoxLayout *main_l =
            LayoutWidgets( Qt::Vertical,
                            {
								content_l,
								WidgetLine( nullptr, Qt::Horizontal ),
                                help_group,
                                mButtonBox

                            }, this, 6, 6, 6, 6, 6 );                       Q_UNUSED( main_l );


    //	Wrap up dimensions

    adjustSize();
    setMinimumWidth( width() );

   Wire();
}


void  CEditExportAsciiDialog::Wire()
{
	mTextEdit->readFromFile( t2q( mOperation->GetExportAsciiOutputPath() ) );
	mTextEdit->MoveTo( 0, false );

    //	connect

    connect( mButtonBox, SIGNAL( accepted() ), this, SLOT( accept() ) );
    connect( mButtonBox, SIGNAL( rejected() ), this, SLOT( reject() ) );
}


CEditExportAsciiDialog::CEditExportAsciiDialog( COperation *operation, QWidget *parent )
    : base_t( parent )
	, mOperation( operation )
{
    CreateWidgets();

	setWindowTitle( "Exported ASCII file: " + t2q( mOperation->GetExportAsciiOutputPath() ) );
}


CEditExportAsciiDialog::~CEditExportAsciiDialog()
{}


//virtual
void  CEditExportAsciiDialog::accept()
{
    base_t::accept();
}


///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_EditExportAsciiDialog.cpp"
