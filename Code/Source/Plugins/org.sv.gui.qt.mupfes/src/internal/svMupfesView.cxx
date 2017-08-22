//     Copyright, 2013
//     Mahdi Esmaily Moghadam

//     This program is distributed in the hope that it will be useful,
//     but WITHOUT ANY WARRANTY; without even the implied warranty of
//     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//--------------------------------------------------------------------

#include "gui.h"
#include "ui_gui.h"
#include "QIntValidator"
#include "QFileDialog"
#include "QErrorMessage"
#include "QMessageBox"
#include "eqclass.h"
#include "bc.h"
#include "QProcess"
#include "QTextStream"

/*
have a single config file for installation of the whole thing

add coupled BC config to output
initialization file path to output
fourier coefficient to output
*/

int nsd = 3;
int prevEq=-1;
bool isEqSaved = true;
QList <bcClass> tmpBCs;
QList<faceClass> aviFace;
QList<faceClass> face;
QList<eqClass> eq;
QString saveName = "";
QString checkVersion = "#MUPFES GUI VERSION 1.0";
char endL[2] = "\n";
char tabS[4] = "   ";
QProcess* proc = new(QProcess);

bool gzwarn=true;
bool gunzip(QString & fileName) {
    int ans;
    if ( QFileInfo(fileName).suffix() == "gz" ) {
        if ( gzwarn ) {
            ans = QMessageBox::question(0,"gz file detected","Would you like to unzip the file?",QMessageBox::Yes,QMessageBox::No);
        } else {
            ans = QMessageBox::Yes;
        }
        if (ans == QMessageBox::Yes) {
            // next time it wouldn't show message box
            gzwarn = false;
            // unzip file
            QStringList arguments;
            arguments << fileName;
            proc->start("gunzip", arguments);
            fileName = fileName.left(fileName.lastIndexOf("."));
            proc->waitForFinished();
            return false;
        } else {
            return true;
        }
    } else {
        return false;
    }
}

gui::gui(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::gui)
{
    ui->setupUi(this);
    setWindowTitle("MUPFES Graphical user interface");

    QDoubleValidator *realVal=new QDoubleValidator;
    realVal->setBottom(0.0);
    ui->dt->setValidator(realVal);

    propL.append(ui->prop_1_txt); propB.append(ui->prop_1_box);
    propL.append(ui->prop_2_txt); propB.append(ui->prop_2_box);
    propL.append(ui->prop_3_txt); propB.append(ui->prop_3_box);
    propL.append(ui->prop_4_txt); propB.append(ui->prop_4_box);
    propL.append(ui->prop_5_txt); propB.append(ui->prop_5_box);

    for ( int i=0 ; i < propL.length() ; i++ ) {
        propB.at(i)->setValidator(realVal);
    }
    ui->phys_prop_group_box->setVisible(false);
    ui->bcList->horizontalHeader()->setVisible(true);

    ui->statusbar->showMessage("Ready");
}

gui::~gui()
{
    foreach ( eqClass iEq, eq ) {
        iEq.bcs.clear();
    }
    eq.clear();
    face.clear();
    delete ui;
}

//********************************************************************************

// Reading connectivity path
void gui::on_connectivityBrowse_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("Open File"),QDir::currentPath(),tr("Connectivity file (*.connectivity *.connectivity.gz)"));
    if ( fileName.isEmpty()) return;
    QFileInfo info = QFileInfo(fileName);
    if (gunzip(fileName)) return;
    ui->connectivity->setText(fileName);
    QDir::setCurrent(info.path());
}

// Reading coordinates path
void gui::on_coordinatesBrowse_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("Open File"),QDir::currentPath(),tr("Coordinates file (*.coordinates *.coordinates.gz)"));
    if ( fileName.isEmpty()) return;
    QFileInfo info = QFileInfo(fileName);
    if (gunzip(fileName)) return;
    ui->coordinates->setText(fileName);
    QDir::setCurrent(info.path());
}

// This is to check if the face name has been used before and all fields are complete
bool gui::checkFace(const faceClass &faI, QListWidgetItem* item)
{
    QMessageBox messageBox;
    if ( faI.add.isEmpty() ) {
        messageBox.critical(0,"Error","ebc/vtk file path is missing!");
        return false;
    }
    if ( faI.name.isEmpty() ) {
        messageBox.critical(0,"Error","Face name is missing!");
        return false;
    }

    QList<QListWidgetItem *> items;
    items = ui->faceList->findItems( faI.name, Qt::MatchExactly );
    for ( int i=0; i < items.length() ; i++ ) {
        if ( items.at(i) != item ) {
            QString mssg("Face name <");
            mssg.append(faI.name);
            mssg.append("> is used before!");
            messageBox.critical(0,"Error",mssg);
            return false;
        }
    }
    return true;
}

// Adding a face to the list
void gui::on_faceAdd_clicked()
{
    QString faceAdd = ui->face_ebc_vtk->text();
    QString faceName = ui->faceName->text();
    faceClass faI(faceName,faceAdd);
    if ( !checkFace(faI) ) { return; }

    QListWidgetItem *item = new QListWidgetItem(faceName, ui->faceList);
    ui->faceList->addItem(item);

    face.append(faI);

    int row = ui->faceList->row(item);
    ui->faceList->item(row)->setSelected(true);
    ui->faceSave->setEnabled(false);
}

// Changing modifications made to the face
void gui::on_faceSave_clicked()
{
    QString faceAdd = ui->face_ebc_vtk->text();
    QString faceName = ui->faceName->text();
    faceClass faI(faceName,faceAdd);

    QList<QListWidgetItem*> items = ui->faceList->selectedItems();
    if ( items.isEmpty() ) { return; }
    QListWidgetItem* item =items.first();
    if ( !checkFace(faI,item) ) { return; }
    int row = ui->faceList->row(item);
    face.replace(row,faI);
    item->setText(faceName);
    ui->faceSave->setEnabled(false);
}

// Updating the ebc and nbc address and faceName fields
void gui::on_faceList_itemSelectionChanged()
{
    ui->face_ebc_vtk->setText("");
    ui->faceName->setText("");

    QList<QListWidgetItem*> items = ui->faceList->selectedItems();
    if ( items.isEmpty() ) { return; }
    QListWidgetItem* item =items.first();
    int row = ui->faceList->row(item);

    ui->face_ebc_vtk->setText(face.at(row).add);
    ui->faceName->setText(face.at(row).name);
}

// Activating save button if there is any difference between ebc/nbc/faceName fields
void gui::on_face_ebc_vtk_textChanged(const QString &arg1)
{
    QList<QListWidgetItem*> items = ui->faceList->selectedItems();
    if ( items.isEmpty() ) { return; }
    QListWidgetItem* item =items.first();
    int row = ui->faceList->row(item);
    bool flag = false;

    if ( face.at(row).add.compare(ui->face_ebc_vtk->text(),Qt::CaseSensitive) != 0 ) flag = true;
    if ( face.at(row).name.compare(ui->faceName->text(),Qt::CaseSensitive) != 0 ) flag = true;
    ui->faceSave->setEnabled(flag);
}

// Removing a face from the list
void gui::on_faceClear_clicked()
{
    QList<QListWidgetItem*> items = ui->faceList->selectedItems();
    if ( items.isEmpty() ) { return; }
    int row = ui->faceList->row(items.first());
    ui->faceList->takeItem(row);
    face.removeAt(row);
}

// Removing all the faces
void gui::on_faceClearAll_clicked()
{
    ui->faceList->clear();
    face.clear();
}

// Adding a bunch of faces to the list
void gui::on_faceBrowse_clicked()
{
    ui->statusbar->showMessage("Adding faces");
    QStringList fileNames = QFileDialog::getOpenFileNames(this,tr("Open File"),QDir::currentPath(),"ebc files (*.ebc *.ebc.gz);;vtk files (*.vtk)");
    if ( fileNames.isEmpty()) {
        ui->statusbar->showMessage("Ready");
        return;
    }
    fileNames.sort();
    QString faceName, faceAdd;
    QListWidgetItem *item;

    for ( int i=0;  i < fileNames.length(); i++) {
        faceAdd = fileNames.at(i);

        if ( gunzip(faceAdd) ) return;

        QFileInfo info = QFileInfo(faceAdd);
        QDir::setCurrent(info.path());
        faceName = info.baseName();

        faceClass faI(faceName,faceAdd);
        if ( !checkFace(faI) ) {
            ui->statusbar->showMessage("Ready");
            return;
        }

        item = new QListWidgetItem(faceName, ui->faceList);
        ui->faceList->addItem(item);
        face.append(faI);

        int row = ui->faceList->row(item);
        ui->faceList->item(row)->setSelected(true);
        ui->faceSave->setEnabled(false);
    }
    ui->statusbar->showMessage("Ready");
}

//********************************************************************************

// Adding an equation to the equation list
void gui::on_addEq_clicked()
{
    QList<QListWidgetItem*> items = ui->equations->selectedItems();
    if ( items.isEmpty() ) { return; }
    QString eqName = items.first()->text();

    if ( eqName == "Incomp. fluid") {
        addEq(0,eqName);
    } else if ( eqName == "FSI" ) {
        addEq(0,eqName);
        addEq(1,"Mesh motion");
    } else {
        addEq(eq.length(),eqName);
    }
    ui->eqAdvOptions->setEnabled(true);
}

// Removing an equation from the list
void gui::on_clearEq_clicked()
{
    int row = getEq();
    if ( row < 0 ) return;
    if (QMessageBox::No == QMessageBox::question(0, "Removing equation", "All modifications will be lost.\nAre you sure?", QMessageBox::Yes|QMessageBox::No)) return;
    isEqSaved = true;
    ui->eqList->takeItem(row);
    eqClass iEq = eq.takeAt(row);
    iEq.bcs.clear();
    // Special case of FSI and Incomp fluid
    if ( (iEq.getPhysName() == "FSI") || (iEq.getPhysName() == "fluid") ) {
        ui->equations->item(0)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        ui->equations->item(2)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    }
    if ( iEq.getPhysName() == "FSI" ) {
        ui->eqList->takeItem(row);
        eqClass iEq = eq.takeAt(row);
        iEq.bcs.clear();
    }
    ui->bcList->clearContents();
    ui->bcList->setRowCount(0);
    if ( eq.length() == 0 ) ui->eqAdvOptions->setEnabled(false);
}

// Adding a new equation
void gui::addEq(const int row, const QString eqName)
{
    eq.insert(row,eqName);
    QListWidgetItem* item = new QListWidgetItem(eqName);
    ui->eqList->insertItem(row,item);
    //    int row = ui->eqList->row(item);
    ui->eqList->item(row)->setSelected(true);
}

// Returning the index of currently selected equation
int gui::getEq()
{
    QList<QListWidgetItem*> items = ui->eqList->selectedItems();
    if ( items.isEmpty() ) return -1;
    return ui->eqList->row(items.first());
}

// Refresh the equation based on the eq object
void gui::updateEq( const int iEq )
{
    ui->statusbar->showMessage("Updating the current equation");
    ui->bc_box->setEnabled(false);
    ui->output->clear();
    ui->outputList->clear();
    for ( int i=0 ; i < propL.length() ; i++ ) {
        propL.at(i)->setVisible(false);
        propB.at(i)->setVisible(false);
    }
    if ( iEq < 0 ) {
        ui->phys_prop_group_box->setVisible(false);
        ui->statusbar->showMessage("Ready");
        return;
    }
    ui->bc_box->setEnabled(true);
    ui->coupled->setChecked(eq.at(iEq).getCoupled());
    ui->minItr->setValue(eq.at(iEq).getMinItr());
    ui->maxItr->setValue(eq.at(iEq).getMaxItr());
    ui->tol->setText(QString::number(eq.at(iEq).getTol()));
    ui->dBr->setValue(eq.at(iEq).getdBr());
    ui->output->addItems(eq.at(iEq).getOutputCandidates());
    ui->outputList->addItems(eq.at(iEq).getOutputNames());
    //  Now setting prop
    ui->phys_prop_group_box->setVisible(true);
    for ( int i=0 ; i < eq.at(iEq).getPropCount() ; i++ ) {
        propL.at(i)->setVisible(true);
        propB.at(i)->setVisible(true);
        propL.at(i)->setText(eq.at(iEq).getPropName(i));
        propB.at(i)->setText(QString::number(eq.at(iEq).getPropValue(i)));
    }
    // And BCs
    ui->bcList->clearContents();
    ui->bcList->setRowCount(eq.at(iEq).bcs.length());
    for ( int i=0 ; i < eq.at(iEq).bcs.length() ; i++ ) {
        QTableWidgetItem* name = new QTableWidgetItem(face.at(eq.at(iEq).bcs.at(i).faIn).name);
        QTableWidgetItem* bcT = new QTableWidgetItem(eq.at(iEq).bcs.at(i).bcGrp);
        ui->bcList->setItem(i,0,name);
        ui->bcList->setItem(i,1,bcT);
    }
    // Special case of FSI and Incomp fluid
    if ( (eq.at(iEq).getPhysName() == "FSI") || (eq.at(iEq).getPhysName() == "fluid") ) {
        ui->equations->item(0)->setFlags(Qt::NoItemFlags);
        ui->equations->item(2)->setFlags(Qt::NoItemFlags);
    }
    if ( eq.at(iEq).getPhysName() == "mesh" ) {
        ui->clearEq->setEnabled(false);
    } else {
        ui->clearEq->setEnabled(true);
    }
    ui->statusbar->showMessage("Ready");
}

// Save the data from GUI to eqClass object
void gui::saveEq( const int iEq )
{
    if ( (iEq < 0) || (iEq >= eq.length()) || isEqSaved ) return;
    ui->statusbar->showMessage("Saving the current equation");
    eqClass myEq(eq.at(iEq));

    myEq.setCoupled(ui->coupled->isChecked());
    myEq.setMinItr(ui->minItr->value());
    myEq.setMaxItr(ui->maxItr->value());
    myEq.setdBr(ui->dBr->value());
    myEq.setTol(ui->tol->text().toDouble());

    QStringList outputs;
    for ( int i=0 ; i < ui->outputList->count() ; i++ )
    {
        outputs.append(ui->outputList->item(i)->text());
    }
    myEq.setOutputs(outputs);

    //  Now setting prop
    for ( int i=0 ; i < eq.at(iEq).getPropCount() ; i++ ) {
        myEq.setPropValue(propB.at(i)->text().toDouble(),i);
    }

    eq.replace(iEq,myEq);
    isEqSaved = true;
    ui->statusbar->showMessage("Ready");
}

// Adding an item to outputs
void gui::on_addOutput_clicked()
{
    QList<QListWidgetItem *> items = ui->output->selectedItems();
    if ( items.isEmpty() ) { return; }
    int i = ui->output->row(items.first());
    QListWidgetItem* item = ui->output->takeItem(i);
    ui->outputList->addItem(item);
    isEqSaved = false;
}

// Removing an item from outputs
void gui::on_clearOutput_clicked()
{
    QList<QListWidgetItem *> items = ui->outputList->selectedItems();
    if ( items.isEmpty() ) { return; }
    int i = ui->outputList->row(items.first());
    QListWidgetItem* item = ui->outputList->takeItem(i);
    ui->output->addItem(item);
    isEqSaved = false;
}

// Switching to a different equation on the list
void gui::on_eqList_itemSelectionChanged()
{
    saveEq(prevEq);
    prevEq = getEq();
    updateEq(prevEq);
}

// Making sure user really ment reseting the equation
void gui::on_resetEq_clicked()
{
    int iEq = getEq();
    if ( iEq < 0 ) return;
    if (QMessageBox::No == QMessageBox::question(0, "Reseting equation", "All modifications will be lost.\nAre you sure?", QMessageBox::Yes|QMessageBox::No)) return;
    eqClass myEq(ui->equations->item(iEq)->text());
    eq.replace(iEq,myEq);
    isEqSaved = true;
    updateEq(iEq);
}

// Minimum number of iteration changes
void gui::on_minItr_editingFinished()
{
    ui->maxItr->setMinimum(ui->minItr->value());
    isEqSaved = false;
}

// Maximum number of iteration chages
void gui::on_maxItr_editingFinished()
{
    ui->minItr->setMaximum(ui->maxItr->value());
    isEqSaved = false;
}

// All of these just trigger saving the equation
void gui::on_coupled_clicked() { isEqSaved = false; }
void gui::on_dBr_editingFinished() { isEqSaved = false; }
void gui::on_tol_textEdited(const QString &arg1) { isEqSaved = false; }
void gui::on_prop_1_box_textEdited(const QString &arg1) { isEqSaved = false; }
void gui::on_prop_2_box_textEdited(const QString &arg1) { isEqSaved = false; }
void gui::on_prop_3_box_textEdited(const QString &arg1) { isEqSaved = false; }
void gui::on_prop_4_box_textEdited(const QString &arg1) { isEqSaved = false; }
void gui::on_prop_5_box_textEdited(const QString &arg1) { isEqSaved = false; }

//********************************************************************************

// Opening BC window
void gui::on_addBc_clicked()
{
    // First calculating available faces that are not included in this equation
    int iEq = getEq();
    aviFace.clear();
    for ( int i=0 ; i < face.length() ; i++ ) {
        bool flag = true;
        foreach ( bcClass ibc , eq.at(iEq).bcs ) {
            if ( ibc.faIn == i ) {
                flag = false;
                break;
            }
        }
        if (flag) {
            aviFace.append(face.at(i));
            aviFace.last().faIn = i;
        }
    }

    if ( aviFace.isEmpty() ) {
        QMessageBox::warning(0,"Warning","No face is available to be added");
        return;
    }

    ui->statusbar->showMessage("Adding boundary condition");

    tmpBCs.clear();
    bc BC;
    BC.exec();
    if (tmpBCs.isEmpty()) return;

    eqClass tmpEq(eq.at(iEq));
    foreach ( bcClass tmpBc, tmpBCs ) {
        tmpEq.bcs.append(tmpBc);
    }
    eq.replace(iEq,tmpEq);
    saveEq(iEq);
    updateEq(iEq);
    ui->statusbar->showMessage("Ready");
}

// If nsd is changed, global parameter is also updated
void gui::on_nsd_currentIndexChanged(const QString &arg1)
{
    nsd = arg1.toInt();
}

// Modifying the BC - This is done by keeping a tmpBC and updating it if neccessary
void gui::on_modifyBc_clicked()
{
    int iEq = getEq();
    QList<QTableWidgetItem*> items = ui->bcList->selectedItems();
    if ( items.isEmpty() ) return;
    ui->statusbar->showMessage("Modifying boundary condition");

    eqClass tmpEq(eq.at(iEq));
    tmpBCs.clear();
    while (true) {
        QList<QTableWidgetItem*> items = ui->bcList->selectedItems();
        if ( items.isEmpty() ) break;
        int row = ui->bcList->row(items.first());
        tmpBCs.append(tmpEq.bcs.at(row));
        tmpEq.bcs.removeAt(row);
        ui->bcList->removeRow(row);
    }
    bc BC;
    BC.exec();
    foreach ( bcClass tmpBc, tmpBCs ) {
        tmpEq.bcs.append(tmpBc);
    }
    eq.replace(iEq,tmpEq);
    saveEq(iEq);
    updateEq(iEq);
    ui->statusbar->showMessage("Ready");
}

// Removing one/multiple BCs
void gui::on_clearBc_clicked()
{
    int iEq = getEq();
    eqClass tmpEq(eq.at(iEq));
    while (true) {
        QList<QTableWidgetItem*> items = ui->bcList->selectedItems();
        if ( items.isEmpty() ) break;
        int row = ui->bcList->row(items.first());
        tmpEq.bcs.removeAt(row);
        ui->bcList->removeRow(row);
    }
    eq.replace(iEq,tmpEq);
    saveEq(iEq);
    updateEq(iEq);
}

// Changing widgets when simulation is running or when it is stopped
void gui::change_running_stat(const bool stat) {
    if (stat) {
        QFile::remove(ui->stopTrigName->text());
        ui->statusbar->showMessage("Ready");
    } else {
        ui->statusbar->showMessage("Running simulation");
    }
    ui->pS->setTabEnabled(0,stat);
    ui->pS->setTabEnabled(1,stat);
    ui->pS->setTabEnabled(2,stat);
    ui->save_icon->setEnabled(stat);
    ui->new_icon->setEnabled(stat);
    ui->runSim->setEnabled(stat);
    ui->stopSim->setEnabled(!stat);
    ui->killSim->setEnabled(!stat);
}

// Lunch the simulation (calling mupfes)
void gui::on_runSim_clicked()
{
    if (!save_mfs(true)) return;
    QDir::setCurrent(QFileInfo(saveName).path());
    change_running_stat(false);
    ui->simHistory->clear();
    QFile::remove(ui->stopTrigName->text());

    connect (proc, SIGNAL(readyReadStandardOutput()), this, SLOT(printOutput()));
    connect (proc, SIGNAL(readyReadStandardError()), this, SLOT(printError()));
    connect (proc, SIGNAL(finished(int , QProcess::ExitStatus )), this, SLOT(change_running_stat()));

    QStringList arguments;
    arguments << saveName;
    proc->start("mupfes", arguments );
}

void gui::on_stopSim_clicked()
{
    if ( proc->state() == QProcess::NotRunning ) return;
    QString msg("Stopping simulation at the end of this time step");
    ui->statusbar->showMessage(msg);
    ui->simHistory->setTextColor(Qt::green);
    ui->simHistory->append(msg);
    ui->simHistory->setTextColor(Qt::black);
    QFile file(ui->stopTrigName->text());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;
    file.close();
}

void gui::printOutput() {
    QByteArray out = proc->read(1024);
    QStringList strLines = QString(out).split("\n");
    foreach (QString str, strLines) {
        if ( str != "" ) ui->simHistory->append(str);
    }
}

// If error occurs, it is printed on red
void gui::printError() {
    ui->statusbar->showMessage("Error occured");
    ui->simHistory->setTextColor(Qt::red);
    QByteArray out = proc->readAllStandardError();
    QStringList strLines = QString(out).split("\n");
    foreach (QString str, strLines) {
        if ( str != "" ) {
            str.prepend("ERROR: ");
            ui->simHistory->append(str);
        }
    }
    ui->simHistory->setTextColor(Qt::black);
    on_stopSim_clicked();
}

// Killing simulation immidiately
void gui::on_killSim_clicked()
{
    if ( proc->state() == QProcess::NotRunning ) return;
    if (QMessageBox::Yes == QMessageBox::question(0, "Killing simulations", "Simulation will stop immediately.\nAre you sure?", QMessageBox::Yes|QMessageBox::No)) proc->kill();
}

// Cleaning the simulation history window
void gui::on_clearSimHistory_clicked()
{
    ui->statusbar->showMessage("Ready");
    ui->simHistory->clear();
}

// New file and all the fields are reseted
void gui::on_new_icon_clicked()
{
    int ans = QMessageBox::question(0, "", "Save the simulation results?", QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
    if (ans == QMessageBox::Cancel) return;
    if (ans == QMessageBox::Yes) save_mfs(false);

    foreach ( eqClass iEq, eq ) {
        iEq.bcs.clear();
    }
    ui->bcList->clearContents();
    ui->bcList->setRowCount(0);
    eq.clear();
    ui->eqList->clear();
    updateEq(-1);
    face.clear();
    on_faceClearAll_clicked();
    ui->clearEq->setEnabled(true);
    ui->equations->item(0)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    ui->equations->item(2)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    ui->coordinates->clear();
    ui->connectivity->clear();
}

// Saving the parameters
void gui::on_save_icon_clicked()
{
    save_mfs(false);
}

// Implementation of saving the parameters into a file
bool gui::save_mfs(bool checkParam)
{
    saveEq(getEq());
    if (saveName == "") {
        saveName = QFileDialog::getSaveFileName(0,"Save the simulation",QDir::currentPath(),"MUPFES file (*.mfs)");
        if ( saveName.isEmpty()) return false;
        QFileInfo info = QFileInfo(saveName);
        QDir::setCurrent(info.path());
        if ( info.suffix() == "" ) {
            saveName.append(".mfs");
        }
    }
    QFile file(saveName);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(0,"Error","Unable to open the file for writing.");
        return false;
    }
    QTextStream out(&file);
    out << checkVersion << endL;
    out << "#----------------------------------------------------------------" << endL;
    out << "# Written by Mahdi Esmaily Moghadam" << endL;
    out << "# Report problems to memt63@gmail.com" << endL;
    out << "#" << endL;
    out << "# This is an input file for intializing mupfes.exe binary solver" << endL;
    out << "# This is directly generated by mupfes GUI application" << endL;
    out << "#----------------------------------------------------------------" << endL;
    out << "# General simulation parameters" << endL;
    //Write your stuff
    out << "Number of spatial dimensions: " << nsd << endL;
//   out << "Element type:  " << ui->eType->currentText() << endL;
    out << "Number of time steps: " << ui->nTimeSteps->text() << endL;
    out << "Time step size: " << ui->dt->text() << endL;
    out << "Continue previous simulation: " << ui->stFileFlag->isChecked() << endL;
    out << "Save results in a folder: " << ui->procFolder->isChecked() << endL;
    out << "Increment in saving restart files: " << ui->stFileIncr->text() << endL;
    if ( ui->saveResults_box->isChecked() ) {
        out << "Format of saved files: " << ui->saveFormat->currentText() << endL;
        out << "Name prefix of saved files: " << ui->saveName->text() << endL;
        out << "Increment in saving files: " << ui->sIncr->text() << endL;
        out << "Start saving after time step: " << ui->start_saving_ts->text() << endL;
        out << "Save averaged results: " << ui->save_average->isChecked() << endL;
    } else {
        out << "Format of saved files: none" << endL;
    }
    out << "Spectral radius of infinite time step: " << ui->rhoInf->text() << endL;
    out << "Searched file name to trigger stop: " << ui->stopTrigName->text() << endL;
//    out << "Name of simulation history file: " << ui->histName->text() << endL;
    out << "Verbose: " << ui->verb->isChecked() << endL;
    out << "Warning: " << ui->warn->isChecked() << endL;
    out << "Debug: " << ui->debug->isChecked() << endL;
    out << "#----------------------------------------------------------------" << endL;
    out << "# Mesh data" << endL;
    if (!QFileInfo(ui->coordinates->text()).exists() && checkParam) {
        QMessageBox::critical(0,"Error"," Coordinates file not found!");
        return false;
    }
    out << "Add mesh: msh {" << endL;
//    out << tabS << "Coordinates file path: " << ui->coordinates->text() << endL;
    out << tabS << "Coordinates file path: " << QDir::current().relativeFilePath(ui->coordinates->text()) << endL;
    if (!QFileInfo(ui->connectivity->text()).exists() && checkParam) {
        QMessageBox::critical(0,"Error"," Connectivity file not found!");
        return false;
    }
//    out << tabS << "Connectivity file path: " << ui->connectivity->text() << endL;
    out << tabS << "Connectivity file path: " << QDir::current().relativeFilePath(ui->connectivity->text()) << endL;
    foreach ( faceClass iFa, face ) {
        out << tabS << "Add face: " << iFa.name << " {" << endL;
        if (!QFileInfo(iFa.add).exists() && checkParam) {
            QString err("ebc/vtk file of face <");
            err.append(iFa.name);
            err.append("> not found!");
            QMessageBox::critical(0,"Error",err);
            return false;
        }
        if ( QFileInfo(iFa.add).suffix() == "ebc" ) {
//            out << tabS << tabS << "Connectivity file (ebc) path: " << iFa.add << endL;
            out << tabS << tabS << "Connectivity file (ebc) path: " << QDir::current().relativeFilePath(iFa.add) << endL;
        } else if ( QFileInfo(iFa.add).suffix() == "vtk" ) {
//            out << tabS << tabS << "vtk file path: " << iFa.add << endL;
            out << tabS << tabS << "vtk file path: " << QDir::current().relativeFilePath(iFa.add) << endL;
        } else {
            QMessageBox::critical(0,"Error","Unknown suffix in saving BC path");
        }
        out << tabS << "}" << endL;
    }
    out << "}" << endL;
    out << "#----------------------------------------------------------------" << endL;
    if ((eq.length()==0) && checkParam) {
        QMessageBox::critical(0,"Error"," There should be at least one equation.");
        return false;
    }
    out << "# Equations" << endL;
    foreach (eqClass iEq, eq) {
        out << "Add equation: " << iEq.getPhysName() << " {" << endL;
        out << tabS << "Coupled: " << iEq.getCoupled() << endL;
        out << tabS << "Min iterations: "  <<  iEq.getMinItr() << endL;
        out << tabS << "Max iterations: "  <<  iEq.getMaxItr() << endL;
        out << tabS << "Tolerance: "  <<  iEq.getTol() << endL;
        out << tabS << "Residual dB reduction: "  << iEq.getdBr() << endL;
        if ( iEq.getPhysName() == "FSI" ) {
            out << tabS << "Domain: 1 { " << endL;
            out << tabS << tabS << "Equation: fluid" << endL;
            out << tabS << tabS << "Density: " << iEq.getPropValue(0) << endL;
            out << tabS << tabS << "Viscosity: " << iEq.getPropValue(1) << endL;
            out << tabS << "}" << endL;
            out << tabS << "Domain: 2 { " << endL;
            out << tabS << tabS << "Equation: struct" << endL;
            out << tabS << tabS << "Density: " << iEq.getPropValue(2) << endL;
            out << tabS << tabS << "Elasticity modulus: " << iEq.getPropValue(3) << endL;
            out << tabS << tabS << "Poisson ratio: " << iEq.getPropValue(4) << endL;
            out << tabS << "}" << endL;
        } else {
            for ( int i=0 ; i < iEq.getPropCount() ; i++ ) {
                out << tabS << iEq.getPropName(i) << ": " << iEq.getPropValue(i) << endL  ;
            }
        }
        out << tabS << "Output: Spatial {" << endL;
        foreach ( QString outName , iEq.getOutputNames() ) {
            out << tabS << tabS << outName << ": t" << endL;
        }
        out << tabS << "}" << endL;

        foreach ( bcClass iBc , iEq.bcs ) {
            QString faceName = face.at(iBc.faIn).name;
            out << tabS << "Add BC: "  <<  faceName << " {" << endL;
            if ( iBc.bcGrp == "Dir" ) {
                out << tabS << tabS << "Type: Dirichlet " << endL;
            } else if ( iBc.bcGrp == "Neu" ) {
                out << tabS << tabS << "Type: Neumann" << endL;
            } else {
                if ( checkParam ) {
                    QString err("BC type must be selected for face ");
                    err.append(faceName);
                    QMessageBox::critical(0,"Error",err);
                    return false;
                }
            }
            out << tabS << tabS << "Time dependence: " << iBc.bcType << endL;
            if ( iBc.bcType == "Steady" ) {
                out << tabS << tabS << "Value: " << iBc.g << endL;
            } else if ( iBc.bcType == "Unsteady" ) {
//                out << tabS << tabS << "Temporal values file path: " << iBc.gtFile << endL;
                out << tabS << tabS << "Temporal values file path: " << QDir::current().relativeFilePath(iBc.gtFile) << endL;
            } else if ( iBc.bcType == "Resistance" ) {
                    out << tabS << tabS << "Value: " << iBc.r << endL;
            } else if ( iBc.bcType == "Coupled" ) {
                // Noting special is required
            } else if ( iBc.bcType == "General" ) {
//                out << tabS << tabS << "Temporal and spatial values file path: " << iBc.gmFile << endL;
                out << tabS << tabS << "Temporal and spatial values file path: " << QDir::current().relativeFilePath(iBc.gmFile) << endL;
            }

            out << tabS << tabS << "Profile: " << iBc.profile << endL;
            if ( iBc.profile == "User_defined" ) {
//                out << tabS << tabS << "Spatial profile file path: " << iBc.gxFile << endL;
                out << tabS << tabS << "Spatial profile file path: " << QDir::current().relativeFilePath(iBc.gxFile) << endL;
            }
            out << tabS << tabS << "Zero out perimeter: " << iBc.zperm << endL;
            out << tabS << tabS << "Impose flux: " << iBc.flux << endL;
            out << tabS << "}" << endL;
        }
        out << "}" << endL;
        out << endL;
    }
    file.close();
    return true;
}

// Read the file into the GUI
void gui::on_open_icon_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(0,"Open the simulation file",QDir::currentPath(),"MUPFES file (*.mfs)");
    if ( fileName.isEmpty()) return;
    QDir::setCurrent(fileName);
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;
    QTextStream in(&file);
    QString line = in.readLine();
    if ( line != checkVersion ) {
        if ( QMessageBox::question(0, "", "Version inconsistent.\n Continue anyway?", QMessageBox::Yes|QMessageBox::No) == QMessageBox::No) return;
    }
    //Read your stuff

    file.close();
}

// About from menu
void gui::on_actionAbout_triggered()
{
    QMessageBox::about(this, tr("About Application (gmupfes)"),
                       tr("Copyright, 2013, Mahdi Esmaily Moghadam\n"
                          "This is a graphical user interface for producing the input file for <b>mupfes</b> solver.\n"
                          "mupfes can be lunched sequentially from this application.\n "
                          "To lunch mupfes in parallel, save *.mfs file and call mupfes directly from command line.\n"
                          "Reports errors to memt63@gmail.com"));
}

// New file
void gui::on_actionNew_triggered()
{
    on_new_icon_clicked();
}

// Save to file
void gui::on_actionSave_triggered()
{
    save_mfs(false);
}

// Save as (using a different name)
void gui::on_actionSave_as_triggered()
{
    saveName = "";
    save_mfs(false);
}

// Exit the application
void gui::on_actionExit_triggered()
{
    int ans = QMessageBox::question(0, "", "Save the simulation results?", QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
    if (ans == QMessageBox::Cancel) return;
    if (ans == QMessageBox::Yes) save_mfs(false);
    exit(0);
}
