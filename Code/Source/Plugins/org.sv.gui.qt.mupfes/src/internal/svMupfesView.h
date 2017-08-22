//     Copyright, 2013
//     Mahdi Esmaily Moghadam

//     This program is distributed in the hope that it will be useful,
//     but WITHOUT ANY WARRANTY; without even the implied warranty of
//     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//--------------------------------------------------------------------

#ifndef GUI_H
#define GUI_H

#include <QMainWindow>
#include "QListWidget"
#include "eqclass.h"
#include "ui_gui.h"

namespace Ui {
class gui;
}

class gui : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit gui(QWidget *parent = 0);
    ~gui();

private slots:
    int getEq();

    bool checkFace(const faceClass& faI, QListWidgetItem* item=0);

    void on_connectivityBrowse_clicked();

    void on_coordinatesBrowse_clicked();

    void on_faceAdd_clicked();

    void on_faceSave_clicked();

    void on_faceList_itemSelectionChanged();

    void on_face_ebc_vtk_textChanged(const QString &arg1);

    void on_faceName_textChanged(const QString &arg1) { on_face_ebc_vtk_textChanged(arg1); }

    void on_faceClear_clicked();

    void on_faceClearAll_clicked();

    void on_addEq_clicked();

    void on_clearEq_clicked();

    void on_addBc_clicked();

    void on_faceBrowse_clicked();

    void on_addOutput_clicked();

    void on_clearOutput_clicked();

    void updateEq( const int iEq );

    void saveEq( const int iEq );

    void addEq( const int  row, const QString eqName);

    void on_eqList_itemSelectionChanged();

    void on_resetEq_clicked();

    void on_tol_textEdited(const QString &arg1);

    void on_minItr_editingFinished();

    void on_dBr_editingFinished();

    void on_maxItr_editingFinished();

    void on_coupled_clicked();

    void on_prop_1_box_textEdited(const QString &arg1);

    void on_prop_2_box_textEdited(const QString &arg1);

    void on_prop_3_box_textEdited(const QString &arg1);

    void on_prop_4_box_textEdited(const QString &arg1);

    void on_prop_5_box_textEdited(const QString &arg1);

    void on_nsd_currentIndexChanged(const QString &arg1);

    void on_modifyBc_clicked();

    void on_clearBc_clicked();

    void on_runSim_clicked();

    void on_stopSim_clicked();

    void change_running_stat(const bool stat=true);

    void printOutput();

    void printError();

    void on_killSim_clicked();

    void on_clearSimHistory_clicked();

    void on_new_icon_clicked();

    void on_save_icon_clicked();

    void on_open_icon_clicked();

    bool save_mfs(bool checkPram);

    void on_actionAbout_triggered();

    void on_actionNew_triggered();

    void on_actionSave_triggered();

    void on_actionSave_as_triggered();

    void on_actionExit_triggered();

private:
    Ui::gui *ui;

    QList<QLabel *> propL;
    QList<QLineEdit *> propB;    
};

extern int nsd;
//extern int prevEq;
//extern int prevBC;

#endif // GUI_H
