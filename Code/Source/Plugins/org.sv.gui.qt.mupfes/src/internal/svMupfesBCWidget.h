//     Copyright, 2013
//     Mahdi Esmaily Moghadam

//     This program is distributed in the hope that it will be useful,
//     but WITHOUT ANY WARRANTY; without even the implied warranty of
//     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//--------------------------------------------------------------------

#ifndef BC_H
#define BC_H

#include <QDialog>
#include "QAbstractButton"

class faceClass {
public:
    QString name;
    QString add;
    int faIn;

    faceClass() {}
    faceClass(QStringList all) { name = all.at(0); add = all.at(1); }
    faceClass(QString nameIn, QString addIn): name(nameIn), add(addIn) { }

    ~faceClass() {}
};

class bcClass {
public:
    QString bcGrp;
    QString bcType;
    QString tDep;
    QString profile;
    int maxnsd;
    int eDrn;
    int cplBCPtr;
    int faIn;
    double r;
    double g;
    QString gxFile;
    QString gmFile;
    QString gtFile;
    bool zperm;
    bool flux;

    bcClass();
    ~bcClass() {}
};

namespace Ui {
class bc;
}

class bc : public QDialog
{
    Q_OBJECT
    
public:
    explicit bc(QWidget *parent = 0);
    ~bc();
    
private slots:
    void on_steady_box_clicked(bool checked);

    void on_unsteady_box_clicked(bool checked);

    void on_coupled_box_clicked(bool checked);

    void on_resistance_box_clicked(bool checked);

    void on_general_box_clicked(bool checked);

    void on_flat_profile_clicked(bool checked);

    void on_parabolic_profile_clicked(bool checked);

    void on_user_defined_profile_clicked(bool checked);

    void on_buttonBox_clicked(QAbstractButton *button);

    void loadBc();

    void saveBc();

    void on_browseSPFP_clicked();

    void on_browseTPFP_clicked();

    void on_browseTSPFP_clicked();

    void on_bcGrp_currentIndexChanged(int index);

    void on_faceList_itemSelectionChanged();

    void on_flsp_textChanged(const QString &arg1);

private:
    Ui::bc *ui;
};

extern QList<faceClass> aviFace;
extern QList<faceClass> face;
extern QList<bcClass> tmpBCs;

#endif // BC_H
