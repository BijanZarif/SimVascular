#include "svMupfesJob.h"

svMupfesJob::svMupfesJob()
{
}

svMupfesJob::svMupfesJob(const svMupfesJob &other)
{
    *this=other;
}

svMupfesJob::~svMupfesJob()
{
}

svMupfesJob* svMupfesJob::Clone()
{
    return new svMupfesJob(*this);
}

//void svMupfesJob::SetDomains(std::map<std::string,Domain> domains)
//{
//    m_Domains=domains;
//}

//std::map<std::string,svMupfesJob::Domain> svMupfesJob::GetDomains()
//{
//    return m_Domains;
//}

//void svMupfesJob::SetDomainFolderName(const std::string& domainName, std::string folderName)
//{
//    m_Domains[domainName].folderName=folderName;
//}

//std::string svMupfesJob::GetDomainFolderName(const std::string& domainName)
//{
//    return m_Domains[domainName].folderName;
//}

//void svMupfesJob::SetDomainFileName(const std::string& domainName, std::string fileName)
//{
//    m_Domains[domainName].fileName=fileName;
//}

//std::string svMupfesJob::GetDomainFileName(const std::string& domainName)
//{
//    return m_Domains[domainName].fileName;
//}

//void svMupfesJob::SetDomainFaceFolderName(const std::string& domainName, std::string faceFolderName)
//{
//    m_Domains[domainName].faceFolderName=faceFolderName;
//}

//std::string svMupfesJob::GetDomainFaceFolderName(const std::string& domainName)
//{
//    return m_Domains[domainName].faceFolderName;
//}

//void svMupfesJob::SetDomainFaceNames(const std::string& domainName, std::vector<std::string> faceNames)
//{
//    m_Domains[domainName].faceNames=faceNames;
//}

//std::vector<std::string> svMupfesJob::GetDomainFaceNames(const std::string& domainName)
//{
//    return m_Domains[domainName].faceNames;
//}







