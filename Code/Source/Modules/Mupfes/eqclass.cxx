//     Copyright, 2013
//     Mahdi Esmaily Moghadam

//     This program is distributed in the hope that it will be useful,
//     but WITHOUT ANY WARRANTY; without even the implied warranty of
//     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//--------------------------------------------------------------------

#include "eqclass.h"
#include "QMessageBox"

eqClass::eqClass(const QString& eq)
{
    for (int i=0; i < maxOutput ; i++ ) isOutsputed[i] = false;
    for (int i=0; i < maxProp ; i++ ) propVal[i] = 0.0;
    if ( eq == "none") {
        return;
    } else if ( eq == "Incomp. fluid" ) {
        outputNames << "Displacement" << "Velocity" << "Acceleration" << "Pressure" << "WSS" << "Vorticity";
        physName = "fluid";
        isOutsputed[1]=true;
        isOutsputed[3]=true;
        propNames << "Density" << "Viscosity";
    } else if ( eq == "Structure" ) {
        outputNames << "Displacement" << "Velocity" << "Acceleration";
        physName = "struct";
        isOutsputed[0]=true;
        isOutsputed[1]=true;
        propNames << "Density" << "Elasticity modulus" << "Poisson ratio";
    } else if ( eq == "FSI" ) {
        outputNames << "Displacement" << "Velocity" << "Acceleration" << "Pressure" << "WSS" << "Vorticity";
        physName = "FSI";
        isOutsputed[0]=true;
        isOutsputed[1]=true;
        isOutsputed[3]=true;
        propNames << "Fluid density" << "Viscosity" << "Solid density" << "Elasticity modulus" << "Poisson ratio";
    } else if ( eq == "Mesh motion" ) {
        outputNames << "Displacement" << "Velocity" << "Acceleration";
        physName = "mesh";
        isOutsputed[0]=true;
        propNames << "Poisson ratio";
    } else if ( eq == "Transport" ) {
        outputNames << "Temperature";
        physName = "heatF";
        isOutsputed[0]=true;
        propNames << "Conductivity";
    } else if ( eq == "Linear elasticity" ) {
        outputNames << "Displacement" << "Velocity" << "Acceleration";
        physName = "lElas";
        isOutsputed[0]=true;
        isOutsputed[1]=true;
        propNames << "Density" << "Elasticity modulus" << "Poisson ratio";
    } else if ( eq == "Heat (Laplace)" ) {
        outputNames << "Temperature";
        physName = "heatS";
        isOutsputed[0]=true;
        propNames << "Conductivity";
    } else if ( eq == "Particles (BBO)" ) {
        outputNames << "Velocity" << "Acceleration";
        physName = "BBO";
        isOutsputed[0]=true;
        isOutsputed[1]=true;
        propNames << "Particle density" << "Particle diameter" << "Force";
    } else if ( eq == "Electromagnetic" ) {
        outputNames << "Potential";
        physName = "elcMag";
        isOutsputed[0]=true;
        propNames << "Force" << "Contact distance";
    } else {
        QMessageBox messageBox;
        QString mssg("Equation <");
        mssg.append(eq);
        mssg.append("> not defined!");
        messageBox.critical(0,"Error",mssg);
        return;
    }
    coupled = true;
    maxItr = 5;
    minItr = 1;
    tol = 1e-4;
    dBr = -20.0;
    return;
}

eqClass::~eqClass()
{
    outputNames.clear();
}

int eqClass::searchOutput(const QString &outputName) const
{
    for (int i=0 ; i < outputNames.length() ; i++) if ( outputName == outputNames.at(i) ) return i;
    QMessageBox messageBox;
    QString mssg("Output <");
    mssg.append(outputName);
    mssg.append("> not found!");
    messageBox.critical(0,"Error",mssg);
    return 0;
}

const QStringList eqClass::getOutputNames() const
{
    QStringList res;
    for (int i=0 ; i < outputNames.length() ; i++) {
        if ( isOutsputed[i] ) res.append(outputNames.at(i));
    }
    return res;
}

const QStringList eqClass::getOutputCandidates() const
{
    QStringList res;
    for (int i=0 ; i < outputNames.length() ; i++) {
        if ( !isOutsputed[i] ) res.append(outputNames.at(i));
    }
    return res;
}

void eqClass::setOutputs(const QStringList& outputNameList) {
    for ( int i=0 ; i < maxOutput ; i++ ) isOutsputed[i] = false;
    for ( int i=0 ; i < outputNameList.length() ; i++ ) setOutput(outputNameList.at(i),true);
}
