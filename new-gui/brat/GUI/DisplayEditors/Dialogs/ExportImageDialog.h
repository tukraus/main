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
#if !defined GUI_DISPLAY_EDITORS_DIALOGS_EXPORT_IMAGE_DIALOG_H
#define GUI_DISPLAY_EDITORS_DIALOGS_EXPORT_IMAGE_DIALOG_H


#include <QDialog>

#include "new-gui/brat/DataModels/Workspaces/Display.h"



class CExportImageDialog : public QDialog
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

    Q_OBJECT;

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

    /////////////////////////////
    //	types
    /////////////////////////////

    using base_t = QDialog;

	using EImageExportType = CDisplayData::EImageExportType;

protected:

    /////////////////////////////
    // static data
    /////////////////////////////


    /////////////////////////////
    // instance data
    /////////////////////////////

    QLineEdit *mOutputPathLineEdit2D = nullptr;
	QPushButton *mBrowseButton2D = nullptr;
    QLineEdit *mOutputPathLineEdit3D = nullptr;
	QPushButton *mBrowseButton3D = nullptr;
    QComboBox *mExtensionCombo = nullptr;
    QCheckBox *m2DCheck = nullptr;
    QCheckBox *m3DCheck = nullptr;
    QGroupBox *mOutputFileGroup2D = nullptr;
    QGroupBox *mOutputFileGroup3D = nullptr;
	QPushButton *mSaveButton = nullptr;

    QDialogButtonBox *mButtonBox = nullptr;

	std::string mOutputFileName2D;
	std::string mOutputFileName3D;
	EImageExportType mOutputFileType = CDisplayData::eTif;
	bool m2D = false;
	bool m3D = false;


    /////////////////////////////
    //construction / destruction
    /////////////////////////////
private:

    void CreateWidgets( bool enable2D, bool enable3D );
    void Wire( bool enable2D, bool enable3D );

public:
    CExportImageDialog( bool enable2D, bool enable3D, std::string &ouput_path2d, std::string &ouput_path3d, QWidget *parent );

    virtual ~CExportImageDialog();


    /////////////////////////////
    // getters / setters / testers
    /////////////////////////////

	const std::string& OutputFileName2D() const { return mOutputFileName2D; }

	const std::string& OutputFileName3D() const { return mOutputFileName3D; }

	EImageExportType OutputFileType() const { return mOutputFileType; }

	bool Save2D() const { return m2DCheck->isChecked(); }
	bool Save3D() const { return m3DCheck->isChecked(); }


    /////////////////////////////
    // Operations
    /////////////////////////////

protected:
    virtual void accept() override;

	bool Check();


protected slots:

    void Handle2DChecked( bool checked );
    void Handle3DChecked( bool checked );
    void HandleExportExtension(int);
    void HandleChangeExportPath();
};



#endif      // GUI_DISPLAY_EDITORS_DIALOGS_EXPORT_IMAGE_DIALOG_H
