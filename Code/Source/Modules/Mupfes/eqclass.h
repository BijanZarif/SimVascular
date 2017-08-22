//     Copyright, 2013
//     Mahdi Esmaily Moghadam

//     This program is distributed in the hope that it will be useful,
//     but WITHOUT ANY WARRANTY; without even the implied warranty of
//     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//--------------------------------------------------------------------

#ifndef EQCLASS_H
#define EQCLASS_H
#define maxOutput 10
#define maxProp 10

#include "QStringList"

//class faceClass {
//public:
//    QString name;
//    QString add;
//    int faIn;

//    faceClass() {}
//    faceClass(QStringList all) { name = all.at(0); add = all.at(1); }
//    faceClass(QString nameIn, QString addIn): name(nameIn), add(addIn) { }

//    ~faceClass() {}
//};

class bcClass {
public:
//    QString faceName;

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

    bcClass()
    {
//        faceName="";
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

    ~bcClass() {}
};

class eqClass
{
public:

    // constructor and destructor
    eqClass(const QString& eq="none");
    eqClass(const eqClass& iEq) { *this = iEq; }
    ~eqClass();

    // get and set functions for few common data between equations
    void setCoupled(const bool& coupledIn) { coupled = coupledIn; }
    void setMaxItr(const int& maxItrIn) { maxItr = maxItrIn; }
    void setMinItr(const int& minItrIn) { minItr = minItrIn; }
    void setTol(const double& tolIn) { tol = tolIn; }
    void setdBr(const double& dBrIn) { dBr = dBrIn; }

    bool getCoupled() const { return coupled; }
    int getMaxItr() const { return maxItr; }
    int getMinItr() const { return minItr; }
    double getTol() const { return tol; }
    double getdBr() const { return dBr; }
    QString getPhysName() const { return physName; }
    QString getDomainName() const { return domainName; }

    // output related functions
    bool getOutput(const QString& outputName ) const { return isOutsputed[searchOutput(outputName)]; }
    void setOutput(const QString& outputName, const bool flag) { isOutsputed[searchOutput(outputName)] = flag; }
    void setOutputs( const QStringList& outputNameList );
    const QStringList getOutputNames() const;
    const QStringList getOutputCandidates() const;

    // properties related functions
    int getPropCount() const { return propNames.length(); }
    const QString getPropName(const int i ) const { return propNames.at(i); }
    double getPropValue( const int i ) const { return propVal[i]; }
    void setPropValue( const double val, const int i ) { propVal[i] = val; }

    //common properties
    bool coupled;
    int maxItr;
    int minItr;
    double tol;
    double dBr;
    QString physName;
    // Outputs
    QStringList outputNames;
    bool isOutsputed[maxOutput];
    //equation specific properties
    QStringList propNames;
    double propVal[maxProp];

//    QList<bcClass> bcs;
    std::map<std::string, bcClass> faceBCs;

    QString domainName;

    int searchOutput(const QString& outputName) const;

};

#endif // EQCLASS_H
