#ifndef SVMUPFESJOB_H
#define SVMUPFESJOB_H

#include <svMupfesExports.h>

#include "eqclass.h"

#include <map>
#include <vector>
#include <sstream>
#include <iostream>
#include <string>

struct svDomain
{
    std::string name;
    std::string folderName;
    std::string fileName;
    std::string type; //fluid, or struct for FSI

    std::string faceFolderName;
    std::vector<std::string> faceNames;
//        std::map<std::string, std::string> faces;

    svDomain()
        : name("")
        , folderName("")
        , fileName("")
        , type("")
        , faceFolderName("mesh-surfaces")
    {

    }

};

class SVMUPFES_EXPORT svMupfesJob
{

public:

    svMupfesJob();

    svMupfesJob(const svMupfesJob &other);

    virtual ~svMupfesJob();

    virtual svMupfesJob* Clone();


//    struct Physics
//    {
//        std::string type;
//        std::string domainName;

//        std::map<std::string, std::string> generalParams;

//        std::map<std::string, std::string> lsParams;

//        std::map<std::string, std::map<std::string, bool>> outputControls;

//        std::map<std::string,std::map<std::string,std::string>> faceBCs;
//    };


//    void SetDomains(std::map<std::string,Domain> domains);
//    std::map<std::string,Domain> GetDomains();
//    void SetDomainFolderName(const std::string& domainName, std::string folderName);
//    std::string GetDomainFolderName(const std::string& domainName);
//    void SetDomainFileName(const std::string& domainName, std::string fileName);
//    std::string GetDomainFileName(const std::string& domainName);
//    void SetDomainFaceFolderName(const std::string& domainName, std::string faceFolderName);
//    std::string GetDomainFaceFolderName(const std::string& domainName);
//    void SetDomainFaceNames(const std::string& domainName, std::vector<std::string> faceNames);
//    std::vector<std::string> GetDomainFaceNames(const std::string& domainName);




  public:

    int timeSteps;
    double stepSize;
    bool continuePrevious;
    bool saveInFoder;
    int restartInc;
    std::string resultPrefix;
    int resultInc;
    int startSavingStep;
    bool saveAvgResult;
    double rhoInf;
    std::string stopFileName;
    bool verbose;
    bool warn;
    bool debug;


    std::map<std::string,svDomain> m_Domains;

    std::map<std::string, eqClass> m_Eqs;



//    std::map<std::string,std::map<std::string,std::string> > m_CapProps;
//    std::map<std::string,std::string> m_WallProps;
//    std::map<std::string,std::map<std::string,std::string> > m_VarProps;
//    std::map<std::string,std::string> m_SolverProps;
//    std::map<std::string,std::string> m_RunProps;


  };


#endif // svMupfesJob_H
