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
#if !defined GUI_WORKSPACE_DIALOG_H
#define GUI_WORKSPACE_DIALOG_H


#include <QDialog>

#include "DataModels/Workspaces/TreeWorkspace.h"

class CModel;



class CWorkspaceDialog : public QDialog
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
	//	Types
	/////////////////////////////

	using base_t = QDialog;

	using EValidationError = CTreeWorkspace::EValidationError;
	using EValidationTask = CTreeWorkspace::EValidationTask;


	/////////////////////////////
	//	Instance Data
	/////////////////////////////

	QLineEdit *mNameText = nullptr;
	QLineEdit *mPathText = nullptr;
	QPushButton *mBrowseDirButton = nullptr;
    QListWidget *mFormulasList = nullptr;
	QGroupBox *mFormulasGroup = nullptr;
	QCheckBox *mDatasetsCheck = nullptr;
	QCheckBox *mOperationsCheck = nullptr;
	QCheckBox *mViewsCheck = nullptr;
	QDialogButtonBox *mButtonBox = nullptr;


	EValidationTask mTask = CTreeWorkspace::eNew;
	QString mInitialDir;

	const CWorkspace *mWks = nullptr;
	const CModel &mModel;

public:
	std::string mPath;
	std::string mName;

	bool mFormulas = false;
	bool mDatasets = false;
	bool mOperations = false;
	bool mViews = false;

	std::vector< std::string > mImportFormulas;


	/////////////////////////////
	//	Construction /Destruction
	/////////////////////////////

private:
	
	QString CreateHelpText();
	void CreateGUI();

public:
	explicit CWorkspaceDialog( QWidget *parent, EValidationTask task, const CWorkspace *wks, const QString &initial_dir, const CModel &model );

    virtual ~CWorkspaceDialog();

    
	/////////////////////////////
	//		Operations
	/////////////////////////////

protected:

	void FillImportFormulas();

	bool Validate( std::string &name, std::string &path );

	virtual void accept();

private slots:

	void collapsedStateChanged( bool );

    void on_button_box_clicked( QAbstractButton *button );

	void on_button_browse_clicked();
	void on_button_select_all_clicked();
	void on_button_deselect_all_clicked();

	void on_button_datasets_toggled( bool toggled );
	void on_button_formulas_toggled( bool toggled );
	void on_button_operations_toggled( bool toggled );
	void on_button_views_toggled( bool toggled );


private:
    void reset();
};


#endif	//GUI_WORKSPACE_DIALOG_H
