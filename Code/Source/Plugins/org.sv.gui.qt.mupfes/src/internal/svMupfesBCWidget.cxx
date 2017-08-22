//     Copyright, 2013
//     Mahdi Esmaily Moghadam

//     This program is distributed in the hope that it will be useful,
//     but WITHOUT ANY WARRANTY; without even the implied warranty of
//     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//--------------------------------------------------------------------

#include "bc.h"
#include "ui_bc.h"
#include "gui.h"
#include "QMessageBox"
#include "QErrorMessage"
#include "QFileDialog"
#include "QDebug"

bc::bc(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::bc)
{
    ui->setupUi(this);

    setWindowTitle("Boundary condition selection");

    QDoubleValidator *realVal=new QDoubleValidator;
    ui->steady_value->setValidator(realVal);
    ui->resistance_value->setValidator(realVal);

    if ( tmpBCs.isEmpty() ) { // In this case new BCs are going to be added
        for (int i=0; i < aviFace.length(); i++ ) {
            ui->faceList->addItem(aviFace.at(i).name);
        }
    } else { // In this case I am modifying BCs
        aviFace.clear();
        foreach (bcClass Bc , tmpBCs ) {
            aviFace.append(face.at(Bc.faIn));
            aviFace.last().faIn = Bc.faIn;
            ui->faceList->addItem(aviFace.last().name);
        }
        ui->faceList->selectAll();
        ui->faceList->setSelectionMode(QAbstractItemView::NoSelection);
        ui->flsp_label->setDisabled(true);
        ui->flsp->setDisabled(true);
    }
    ui->directionalBc->setMaximum(nsd);
    loadBc();
}

bc::~bc()
{
    delete ui;
}

bcClass::bcClass() {
    bcGrp = "NA";
    bcType = "Steady";
    profile = "Flat";
    eDrn = 0;
    cplBCPtr = -1;
    faIn = -1;
    r = 0.0;
    g = 0.0;
    gmFile = "";
    gtFile = "";
    gxFile = "";
    zperm = true;
    flux = false;
}

void bc::on_steady_box_clicked(bool checked)
{
    if ( checked ) {
        ui->unsteady_box->setChecked(false);
        ui->coupled_box->setChecked(false);
        ui->resistance_box->setChecked(false);
        ui->general_box->setChecked(false);
    } else {
        ui->steady_box->setChecked(true);
    }
}

void bc::on_unsteady_box_clicked(bool checked)
{
    if ( checked ) {
        ui->steady_box->setChecked(false);
        ui->coupled_box->setChecked(false);
        ui->resistance_box->setChecked(false);
        ui->general_box->setChecked(false);
    } else {
        ui->unsteady_box->setChecked(true);
    }

}

void bc::on_resistance_box_clicked(bool checked)
{
    if ( checked ) {
        ui->steady_box->setChecked(false);
        ui->unsteady_box->setChecked(false);
        ui->coupled_box->setChecked(false);
        ui->general_box->setChecked(false);
    } else {
        ui->resistance_box->setChecked(true);
    }
}

void bc::on_coupled_box_clicked(bool checked)
{
    if ( checked ) {
        ui->steady_box->setChecked(false);
        ui->unsteady_box->setChecked(false);
        ui->resistance_box->setChecked(false);
        ui->general_box->setChecked(false);
    } else {
        ui->coupled_box->setChecked(true);
    }
}

void bc::on_general_box_clicked(bool checked)
{
    if ( checked ) {
        ui->steady_box->setChecked(false);
        ui->unsteady_box->setChecked(false);
        ui->coupled_box->setChecked(false);
        ui->resistance_box->setChecked(false);
    } else {
        ui->general_box->setChecked(true);
    }
}

void bc::on_flat_profile_clicked(bool checked)
{
    if ( checked ) {
        ui->parabolic_profile->setChecked(false);
        ui->user_defined_profile->setChecked(false);
    } else {
        ui->flat_profile->setChecked(true);
    }
}

void bc::on_parabolic_profile_clicked(bool checked)
{
    if ( checked ) {
        ui->user_defined_profile->setChecked(false);
        ui->flat_profile->setChecked(false);
    } else {
        ui->parabolic_profile->setChecked(true);
    }
}

void bc::on_user_defined_profile_clicked(bool checked)
{
    if ( checked ) {
        ui->parabolic_profile->setChecked(false);
        ui->flat_profile->setChecked(false);
    } else {
        ui->user_defined_profile->setChecked(true);
    }
}

void bc::on_buttonBox_clicked(QAbstractButton *button)
{
    QPushButton *b = qobject_cast<QPushButton*>(button);

    if (b == ui->buttonBox->button(QDialogButtonBox::Ok)) {
        saveBc();
    } else if (b == ui->buttonBox->button(QDialogButtonBox::Cancel)) {
        // Doing nothing;
    } else if (b == ui->buttonBox->button(QDialogButtonBox::Reset)) {
        ui->flsp->clear();
        loadBc();
    }
}

void bc::loadBc() {
    QMessageBox messageBox;
    int nBc = tmpBCs.length();
    bcClass* tmpBc;
    bcClass item;
    if ( nBc == 0 ) {
        // Doing nothing here
    } else if ( nBc == 1 ) {
        item = tmpBCs.first();
    } else {
        item = tmpBCs.first();
        item.faIn = bcClass().faIn;
        foreach ( bcClass newItem, tmpBCs ) {
            if ( newItem.bcGrp != item.bcGrp ) item.bcGrp = bcClass().bcGrp;
            if ( newItem.bcType != item.bcType ) item.bcType = bcClass().bcType;
            if ( newItem.profile != item.profile ) item.profile = bcClass().profile;
            if ( newItem.eDrn != item.eDrn ) item.eDrn = bcClass().eDrn;
            if ( newItem.cplBCPtr != item.cplBCPtr ) item.cplBCPtr = bcClass().cplBCPtr;
            if ( newItem.r != item.r ) item.r = bcClass().r;
            if ( newItem.g != item.g ) item.g = bcClass().g;
            if ( newItem.gmFile != item.gmFile ) item.gmFile = bcClass().gmFile;
            if ( newItem.gtFile != item.gtFile ) item.gtFile = bcClass().gtFile;
            if ( newItem.gxFile != item.gxFile ) item.gxFile = bcClass().gxFile;
            if ( newItem.zperm != item.zperm ) item.zperm = bcClass().zperm;
            if ( newItem.flux != item.flux ) item.flux = bcClass().flux;
        }
    }
    tmpBc = &item;

    if ( tmpBc->eDrn > 0 ) {
        ui->directionalBc_box->setChecked(true);
        ui->directionalBc->setValue(tmpBc->eDrn);
    } else {
        ui->directionalBc_box->setChecked(false);
    }

    int curIndex = -1;
    if ( tmpBc->bcGrp == "NA" ) {
        // this is the default value
    } else if ( tmpBc->bcGrp == "Dir" ) {
        curIndex = 0;
    } else if ( tmpBc->bcGrp == "Neu" ) {
        curIndex = 1;
    } else {
        messageBox.critical(0,"Error","ERROR-101: unexpected bcGrp");
    }
    ui->bcGrp->setCurrentIndex(curIndex);
    on_bcGrp_currentIndexChanged(curIndex);

    if ( tmpBc->bcType == "Steady" ) {
        ui->steady_box->setChecked(true);
        on_steady_box_clicked(true);
        ui->steady_value->setText(QString::number(tmpBc->g));
    } else if ( tmpBc->bcType == "Unsteady" ) {
        ui->unsteady_box->setChecked(true);
        on_unsteady_box_clicked(true);
        ui->TPFP->setText(tmpBc->gtFile);
    } else if ( tmpBc->bcType == "Coupled" ) {
        ui->coupled_box->setChecked(true);
        on_coupled_box_clicked(true);
    } else if ( tmpBc->bcType == "Resistance" ) {
        ui->resistance_box->setChecked(true);
        on_resistance_box_clicked(true);
        ui->resistance_value->setText(QString::number(tmpBc->r));
    } else if ( tmpBc->bcType == "General" ) {
        ui->general_box->setChecked(true);
        on_general_box_clicked(true);
        ui->TSPFP->setText(tmpBc->gmFile);
    } else {
        messageBox.critical(0,"Error","ERROR-102: unexpected bcType");
    }

    if ( tmpBc->profile == "Flat" ) {
        ui->flat_profile->setChecked(true);
        on_flat_profile_clicked(true);
    } else if ( tmpBc->profile == "Parabolic" ) {
        ui->parabolic_profile->setChecked(true);
        on_parabolic_profile_clicked(true);
    } else if ( tmpBc->profile == "User_defined" ) {
        ui->user_defined_profile->setChecked(true);
        on_user_defined_profile_clicked(true);
        ui->SPFP->setText(tmpBc->gxFile);
    } else {
        messageBox.critical(0,"Error","ERROR-103: unexpected profile");
    }
    ui->zero_out_perimeter->setChecked(tmpBc->zperm);
    ui->impose_flux->setChecked(tmpBc->flux);
}

void bc::saveBc() {
    if ( ui->faceList->selectedItems().isEmpty() ) return;

    bcClass* tmpBc = new(bcClass);

    if ( ui->directionalBc_box->isChecked() ) {
        tmpBc->eDrn = ui->directionalBc->value();
    } else {
        tmpBc->eDrn = -1;
    }
    if ( ui->bcGrp->currentIndex() == -1 ) {
        tmpBc->bcGrp = "NA";
    } else if ( ui->bcGrp->currentIndex() == 0 ) {
        tmpBc->bcGrp = "Dir";
    } else if ( ui->bcGrp->currentIndex() == 1 ) {
        tmpBc->bcGrp = "Neu";
    }

    if ( ui->steady_box->isChecked() ) {
        tmpBc->bcType = "Steady";
        tmpBc->g = ui->steady_value->text().toDouble();
    } else if ( ui->unsteady_box->isChecked() ) {
        tmpBc->bcType = "Unsteady";
        tmpBc->gtFile = ui->TPFP->text();
    } else if ( ui->coupled_box->isChecked() ) {
        tmpBc->bcType = "Coupled";
    } else if ( ui->resistance_box->isChecked() ) {
        tmpBc->bcType = "Resistance";
        tmpBc->r = ui->resistance_value->text().toDouble();
    } else if ( ui->general_box->isChecked() ) {
        tmpBc->bcType = "General";
        tmpBc->gmFile = ui->TSPFP->text();
    }

    if ( ui->flat_profile->isChecked() ) {
        tmpBc->profile = "Flat";
    } else if ( ui->parabolic_profile->isChecked()  ) {
        tmpBc->profile = "Parabolic";
    } else if ( ui->user_defined_profile->isChecked()  ) {
        tmpBc->profile = "User_defined";
        tmpBc->gxFile = ui->SPFP->text();
    }

    tmpBc->zperm = ui->zero_out_perimeter->isChecked();
    tmpBc->flux = ui->impose_flux->isChecked();

    tmpBCs.clear();
    foreach ( QListWidgetItem* item , ui->faceList->selectedItems() ) {
        int row = ui->faceList->row(item);
        tmpBc->faIn = aviFace.at(row).faIn;
        tmpBCs.append(*tmpBc);
    }
}

void bc::on_browseTPFP_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("Open File"),QDir::currentPath(),tr("Temporal values file (*.*)"));
    if ( fileName.isEmpty()) return;
    ui->TPFP->setText(fileName);
    QFileInfo info = QFileInfo(fileName);
    QDir::setCurrent(info.path());
}

void bc::on_browseSPFP_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("Open File"),QDir::currentPath(),tr("Spatial profile file (*.*)"));
    if ( fileName.isEmpty()) return;
    ui->SPFP->setText(fileName);
    QFileInfo info = QFileInfo(fileName);
    QDir::setCurrent(info.path());
}

void bc::on_browseTSPFP_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("Open File"),QDir::currentPath(),tr("Temporal and spatial values file (*.*)"));
    if ( fileName.isEmpty()) return;
    ui->TSPFP->setText(fileName);
    QFileInfo info = QFileInfo(fileName);
    QDir::setCurrent(info.path());
}

void bc::on_bcGrp_currentIndexChanged(int index)
{
    if ( index == -1 ) {
        ui->time_dependance_box->setEnabled(false);
        ui->profile_box->setEnabled(false);
    } else {
        ui->time_dependance_box->setEnabled(true);
        ui->profile_box->setEnabled(true);
        if ( index == 0 ) {
            ui->zero_out_perimeter->setChecked(true);
        } else if ( index == 1 ) {
            ui->zero_out_perimeter->setChecked(false);
        }
    }
}

void bc::on_faceList_itemSelectionChanged()
{
    if ( ui->faceList->selectedItems().isEmpty() ) {
        ui->bcGrp->setEnabled(false);
        ui->bcGrp->setCurrentIndex(-1);
    } else {
        ui->bcGrp->setEnabled(true);
    }
}

void bc::on_flsp_textChanged(const QString &arg1)
{
    QString searchPattern(arg1);
    searchPattern.append("*");
    searchPattern.prepend("*");
    QRegExp rx(searchPattern);
    rx.setPatternSyntax(QRegExp::Wildcard);

    for ( int i=0 ; i<aviFace.length() ; i++ ) {
        if (rx.exactMatch(aviFace.at(i).name)) {
            // This is the case that face was not displayed before and now it must be displayed
            if ( ui->faceList->isRowHidden(i) ) {
                ui->faceList->setRowHidden(i,false);
            }
        } else {
            // This is the case that face used to be displayed and now it should be removed from the list
            if ( !ui->faceList->isRowHidden(i) ) {
                ui->faceList->setRowHidden(i,true);
            }
        }
    }
}
