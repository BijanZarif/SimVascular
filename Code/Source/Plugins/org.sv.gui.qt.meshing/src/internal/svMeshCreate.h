#ifndef SVMESHCREATE_H
#define SVMESHCREATE_H

#include "svDataNodeOperationInterface.h"
#include <mitkDataStorage.h>
#include <QWidget>

namespace Ui {
class svMeshCreate;
}

class svMeshCreate : public QWidget
{
    Q_OBJECT

public:

    svMeshCreate(mitk::DataStorage::Pointer dataStorage, mitk::DataNode::Pointer selectedNode, int timeStep);

    virtual ~svMeshCreate();

public slots:

    void CreateMesh();

    void Cancel();

    void SetFocus();

    void Activated();

    void SetupMeshType(int idx);

protected:

    Ui::svMeshCreate *ui;

    QWidget* m_Parent;

    mitk::DataNode::Pointer m_MeshFolderNode;

    mitk::DataNode::Pointer m_ModelFolderNode;

    mitk::DataStorage::Pointer m_DataStorage;

    mitk::DataNode::Pointer m_SelecteNode;

    int m_TimeStep;

    svDataNodeOperationInterface* m_Interface;
};

#endif // SVMESHCREATE_H
