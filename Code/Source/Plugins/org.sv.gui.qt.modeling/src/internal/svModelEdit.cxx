#include "svModelEdit.h"
#include "ui_svModelEdit.h"
#include "ui_svSegSelectionWidget.h"

#include "svModel.h"
#include "svModelUtils.h"

#include "cv_polydatasolid_utils.h"

#include <mitkNodePredicateDataType.h>
#include <mitkSurface.h>
#include <mitkUndoController.h>
#include <mitkSliceNavigationController.h>

#include <usModuleRegistry.h>

#include <QTreeView>
#include <QStandardItemModel>

#include <iostream>
using namespace std;

const QString svModelEdit::EXTENSION_ID = "org.sv.views.modeledit";

svModelEdit::svModelEdit() :
    ui(new Ui::svModelEdit)
{
    m_Model=NULL;
    m_ModelNode=NULL;

    m_SegSelectionWidget=NULL;

    m_RemovingNode=false;

    m_ModelSelectFaceObserverTag=0;
    m_ModelUpdateFaceObserverTag=0;
}

svModelEdit::~svModelEdit()
{
    delete ui;

    if(m_SegSelectionWidget) delete m_SegSelectionWidget;
}

void svModelEdit::CreateQtPartControl( QWidget *parent )
{
    m_Parent=parent;
    ui->setupUi(parent);

    parent->setMaximumWidth(450);

    connect(ui->btnUpdateModel, SIGNAL(clicked()), this, SLOT(ShowSegSelectionWidget()) );

    m_SegSelectionWidget=new svSegSelectionWidget();
    m_SegSelectionWidget->move(400,400);
    m_SegSelectionWidget->hide();
    connect(m_SegSelectionWidget->ui->buttonBox,SIGNAL(accepted()), this, SLOT(CreateModel()));
    connect(m_SegSelectionWidget->ui->buttonBox,SIGNAL(rejected()), this, SLOT(HideSegSelectionWidget()));

    connect(ui->listWidget,SIGNAL(clicked(const QModelIndex&)), this, SLOT(SelectItem(const QModelIndex&)) );

    connect(ui->btnBlend, SIGNAL(clicked()), this, SLOT(BlendModel()) );
}

void svModelEdit::Visible()
{
    OnSelectionChanged(GetDataManagerSelection());
}

void svModelEdit::Hidden()
{
    ClearAll();
}

int svModelEdit::GetTimeStep()
{
    mitk::SliceNavigationController* timeNavigationController = NULL;
    if(m_DisplayWidget)
    {
        timeNavigationController=m_DisplayWidget->GetTimeNavigationController();
    }

    if(timeNavigationController)
        return timeNavigationController->GetTime()->GetPos();
    else
        return 0;

}

void svModelEdit::OnSelectionChanged(std::vector<mitk::DataNode*> nodes )
{
//    if(!IsActivated())
    if(!IsVisible())
    {
        return;
    }

    if(nodes.size()==0)
    {
        ClearAll();
        m_Parent->setEnabled(false);
        return;
    }

    mitk::DataNode::Pointer modelNode=nodes.front();

    if(m_ModelNode==modelNode)
    {
        return;
    }

    ClearAll();

    m_ModelNode=modelNode;
    m_Model=dynamic_cast<svModel*>(modelNode->GetData());
    if(!m_Model)
    {
        ClearAll();
        m_Parent->setEnabled(false);
        return;
    }

    m_Parent->setEnabled(true);

    ui->labelModelName->setText(QString::fromStdString(m_ModelNode->GetName()));

    svModelElement* modelElement=m_Model->GetModelElement();
    if(modelElement)
        ui->labelModelType->setText(QString::fromStdString(modelElement->GetType()));

    UpdateGUI();

    m_DataInteractor = svModelDataInteractor::New();
    m_DataInteractor->LoadStateMachine("svModelInteraction.xml", us::ModuleRegistry::GetModule("svModel"));
    m_DataInteractor->SetEventConfig("svModelConfig.xml", us::ModuleRegistry::GetModule("svModel"));
    m_DataInteractor->SetDataNode(m_ModelNode);

    //Add Observers
    itk::SimpleMemberCommand<svModelEdit>::Pointer modelSelectFaceCommand = itk::SimpleMemberCommand<svModelEdit>::New();
    modelSelectFaceCommand->SetCallbackFunction(this, &svModelEdit::UpdateGUI);
    m_ModelSelectFaceObserverTag = m_Model->AddObserver( svModelSelectFaceEvent(), modelSelectFaceCommand);

    itk::SimpleMemberCommand<svModelEdit>::Pointer modelUpdateFaceCommand = itk::SimpleMemberCommand<svModelEdit>::New();
    modelUpdateFaceCommand->SetCallbackFunction(this, &svModelEdit::UpdateFacesAndNodes);
    m_ModelUpdateFaceObserverTag = m_Model->AddObserver( svModelSetVtkPolyDataEvent(), modelUpdateFaceCommand);

    mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}

void svModelEdit::UpdateGUI()
{
    ui->listWidget->clear();
    if(!m_Model) return;

    svModelElement* modelElement=m_Model->GetModelElement();
    if(!modelElement) return;

    std::vector<svModelElement::svFace*> faces=modelElement->GetFaces();

    for(int i=0;i<faces.size();i++)
    {

        svModelElement::svFace* face=faces[i];
        if(face)
        {
            QString item=QString::fromStdString(face->name);
            ui->listWidget->addItem(item);
        }
    }

    int selectedIndex=modelElement->GetSelectedFaceIndex();
    if(selectedIndex>-1)
    {
        QModelIndex mIndex=ui->listWidget->model()->index(selectedIndex,0);
        ui->listWidget->selectionModel()->select(mIndex, QItemSelectionModel::ClearAndSelect);
        SelectItem(mIndex);
    }

}

void svModelEdit::SelectItem(const QModelIndex & idx){

    int index=idx.row();

    QListWidgetItem* item=ui->listWidget->item(index);
    if(!item) return;

    std::string selectedName=item->text().toStdString();

    if(m_Model)
    {
        int timeStep=GetTimeStep();
        svModelElement* modelElement=m_Model->GetModelElement(timeStep);
        if(modelElement==NULL) return;

        mitk::DataStorage::SetOfObjects::ConstPointer nodes=GetDataStorage()->GetDerivations(m_ModelNode);
        for(int i=0;i<nodes->size();i++)
        {
            mitk::DataNode::Pointer node=nodes->GetElement(i);
            if(node->GetName()==selectedName)
                node->SetColor(1,1,0);
            else
                node->SetColor(1,1,1);
        }

        mitk::RenderingManager::GetInstance()->RequestUpdateAll();
    }

}


void svModelEdit::NodeChanged(const mitk::DataNode* node)
{
//    if(!IsVisible())
//    {
//        return;
//    }

//    if(m_RemovingNode)
//    {
//        return;
//    }

//    mitk::NodePredicateDataType::Pointer isModel = mitk::NodePredicateDataType::New("svModel");
//    mitk::DataStorage::SetOfObjects::ConstPointer rs=GetDataStorage()->GetSources(node,isModel);
//    if(rs->size()<1) return;

//    mitk::DataNode::Pointer modelNode=rs->GetElement(0);

//    const svModel* model = dynamic_cast<const svModel*>(modelNode->GetData());
//    if(!model) return;
//    svModelElement* modelElement=model->GetModelElement();
//    if(!modelElement) return;

//    rs=GetDataStorage()->GetDerivations(modelNode);
//    std::vector<svModelElement::svFace*> faces=modelElement->GetFaces();

//    for(int i=0;i<rs->size();i++)
//        faces[i]->name=rs->GetElement(i)->GetName();

//    UpdateGUI();

}

void svModelEdit::NodeAdded(const mitk::DataNode* node)
{
}

void svModelEdit::NodeRemoved(const mitk::DataNode* node)
{
//    OnSelectionChanged(GetDataManagerSelection());
}

void svModelEdit::ClearAll()
{
    if(m_Model && m_ModelSelectFaceObserverTag)
    {
        m_Model->RemoveObserver(m_ModelSelectFaceObserverTag);
    }

    if(m_Model && m_ModelUpdateFaceObserverTag)
    {
        m_Model->RemoveObserver(m_ModelUpdateFaceObserverTag);
    }

    if(m_ModelNode)
    {
        m_ModelNode->SetDataInteractor(NULL);
        m_DataInteractor=NULL;
    }

    m_Model=NULL;
    m_ModelNode=NULL;

    ui->labelModelName->setText("");
    ui->labelModelType->setText("");
    ui->listWidget->clear();
}

void svModelEdit::ShowSegSelectionWidget()
{

    mitk::NodePredicateDataType::Pointer isProjFolder = mitk::NodePredicateDataType::New("svProjectFolder");
    mitk::DataStorage::SetOfObjects::ConstPointer rs=GetDataStorage()->GetSubset(isProjFolder);

    if(rs->size()<1) return;

    mitk::DataNode::Pointer projFolderNode=rs->GetElement(0);

    rs=GetDataStorage()->GetDerivations (projFolderNode,mitk::NodePredicateDataType::New("svSegmentationFolder"));
    if(rs->size()<1) return;

    mitk::DataNode::Pointer segFolderNode=rs->GetElement(0);
    rs=GetDataStorage()->GetDerivations(segFolderNode);
    if(rs->size()<1) return;

    std::vector<mitk::DataNode::Pointer> segNodes;
    for(int i=0;i<rs->size();i++)
        segNodes.push_back(rs->GetElement(i));

    int segNum=segNodes.size();

    QStandardItemModel *model;
    // QStandardItemModel(int rows, int columns, QObject * parent = 0)
    model = new QStandardItemModel(segNum,2,this);

    svModelElement* modelElement=m_Model->GetModelElement();

    for(int row = 0; row < segNum; row++)
    {
        for(int col = 0; col < 2; col++)
        {
            QModelIndex index
                    = model->index(row,col,QModelIndex());
            //            if(col==0)
            //            {
            //                model->setData(index,QString::fromStdString(segNodes[row]->GetName()));
            //            }
            //            else if(col==1)
            //            {
            //                if(modelElement&&modelElement->HasSeg(segNodes[row]->GetName()))
            //                    model->setData(index,true);
            //                else
            //                    model->setData(index,false);
            //            }
            if(col==0)
            {
                QStandardItem* item= new QStandardItem(QString::fromStdString(segNodes[row]->GetName()));
                model->setItem(row,col,item);
            }
            else if(col==1)
            {
                if(modelElement&&modelElement->HasSeg(segNodes[row]->GetName()))
                {
                    QStandardItem* item= new QStandardItem(true);
                    item->setCheckable(true);
                    item->setCheckState(Qt::Checked);
                    model->setItem(row,col,item);
                }
                else
                {
                    QStandardItem* item= new QStandardItem(false);
                    item->setCheckable(true);
                    item->setCheckState(Qt::Unchecked);
                    model->setItem(row,col,item);
                }

            }
        }
    }

    QStringList headers;
    headers << "Segmentation" << "Use";
    model->setHorizontalHeaderLabels(headers);

    m_SegSelectionWidget->ui->tableView->setModel(model);
    //    m_SegSelectionWidget->ui->tableView->setColumnWidth(0,150);
    m_SegSelectionWidget->ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_SegSelectionWidget->show();
}

void svModelEdit::HideSegSelectionWidget()
{
    m_SegSelectionWidget->hide();
}

void svModelEdit::CreateModel()
{
    std::vector<std::string> segNames;
    int rowCount=m_SegSelectionWidget->ui->tableView->model()->rowCount(QModelIndex());
    for(int i=0;i<rowCount;i++)
    {
        QModelIndex index= m_SegSelectionWidget->ui->tableView->model()->index(i,1, QModelIndex());
        if(index.data(Qt::CheckStateRole) == Qt::Checked){
            QModelIndex idx= m_SegSelectionWidget->ui->tableView->model()->index(i,0, QModelIndex());
            segNames.push_back(idx.data().toString().toStdString());
        }
    }

    mitk::NodePredicateDataType::Pointer isProjFolder = mitk::NodePredicateDataType::New("svProjectFolder");
    mitk::DataStorage::SetOfObjects::ConstPointer rs=GetDataStorage()->GetSubset(isProjFolder);

    if(rs->size()<1) return;

    mitk::DataNode::Pointer projFolderNode=rs->GetElement(0);

    rs=GetDataStorage()->GetDerivations (projFolderNode,mitk::NodePredicateDataType::New("svSegmentationFolder"));
    if(rs->size()<1) return;

    //    mitk::DataNode * 	GetNamedDerivedNode (const char *name, const mitk::DataNode *sourceNode, bool onlyDirectDerivations=true) const

    mitk::DataNode::Pointer segFolderNode=rs->GetElement(0);
    //    rs=GetDataStorage()->GetDerivations(segFolderNode);
    //    if(rs->size()<1) return;

    std::vector<mitk::DataNode::Pointer> segNodes;
    //    for(int i=0;i<rs->size();i++)
    //        segNodes.push_back(rs->GetElement(i));

    for(int i=0;i<segNames.size();i++)
    {
        mitk::DataNode::Pointer node=GetDataStorage()->GetNamedDerivedNode(segNames[i].c_str(),segFolderNode);
        if(node.IsNotNull())
            segNodes.push_back(node);
    }

    svModelElement* modelElement=svModelUtils::CreateSolidModelElement(segNodes);

    m_Model->SetModelElement(modelElement);

    m_RemovingNode=true;

    mitk::DataStorage::SetOfObjects::ConstPointer nodesToRemove=GetDataStorage()->GetDerivations(m_ModelNode,nullptr,false);
    if( !nodesToRemove->empty())
    {
        GetDataStorage()->Remove(nodesToRemove);
    }

    std::vector<svModelElement::svFace*> faces=modelElement->GetFaces();

    for(int i=0;i<faces.size();i++)
    {
        mitk::Surface::Pointer surface=mitk::Surface::New();
        surface->SetVtkPolyData((faces[i]->vpd));
        mitk::DataNode::Pointer surfaceNode = mitk::DataNode::New();
        surfaceNode->SetData(surface);
        surfaceNode->SetName(faces[i]->name);
        surfaceNode->SetBoolProperty("pickable", true);
        faces[i]->node=surfaceNode;
        GetDataStorage()->Add(surfaceNode,m_ModelNode);

    }

    m_RemovingNode=false;

    HideSegSelectionWidget();

    OnSelectionChanged(GetDataManagerSelection());
}

void svModelEdit::BlendModel()
{
    if(m_Model==NULL) return;

    int timeStep=GetTimeStep();
    svModelElement* modelElement=m_Model->GetModelElement(timeStep);

    if(modelElement==NULL) return;

    vtkPolyData* oldVpd=modelElement->GetVtkPolyDataModel();
    if(oldVpd==NULL) return;

    svModelUtils::svBlendParam* param=new svModelUtils::svBlendParam();

    param->numblenditers=ui->lineEditBlend->text().trimmed().toInt();
    param->numsubblenditers=ui->lineEditSubBlend->text().trimmed().toInt();
    param->numcgsmoothiters=ui->lineEditConstrSmooth->text().trimmed().toInt();
    param->numlapsmoothiters=ui->lineEditLapSmooth->text().trimmed().toInt();
    param->numsubdivisioniters=ui->lineEditSubdivision->text().trimmed().toInt();
    param->targetdecimation=ui->lineEditDecimation->text().trimmed().toDouble();

    QString content=ui->plainTextEditBlend->toPlainText();

    QStringList list = content.split("\n");
//    vtkPolyData* newVpd=NULL;
    vtkPolyData* lastVpd=oldVpd;

//    newVpd=svModelUtils::CreateSolidModelPolyDataByBlend(oldVpd, 1, 2, 0.5, param);
    for(int i=0;i<list.size();i++)
    {
        QStringList list2 = list[i].split(QRegExp("[(),{}\\s+]"), QString::SkipEmptyParts);
        if(list2.size()!=3) continue;

        int faceID1=list2[0].trimmed().toInt();
        int faceID2=list2[1].trimmed().toInt();
        double radius=list2[2].trimmed().toDouble();

        cout<<faceID1<<"...."<<faceID2<<"....."<<radius<<endl;

        if(lastVpd==NULL) break;

        lastVpd=svModelUtils::CreateSolidModelPolyDataByBlend(lastVpd, faceID1, faceID2, radius, param);

    }

    vtkPolyData* newVpd=lastVpd;

    if(newVpd==NULL) return;

    svModelOperation* doOp = new svModelOperation(svModelOperation::OpSETVTKPOLYDATA,timeStep,newVpd);
    svModelOperation* undoOp = new svModelOperation(svModelOperation::OpSETVTKPOLYDATA,timeStep,oldVpd);
    mitk::OperationEvent *operationEvent = new mitk::OperationEvent(m_Model, doOp, undoOp, "Set VtkPolyData");
    mitk::UndoController::GetCurrentUndoModel()->SetOperationEvent( operationEvent );

    m_Model->ExecuteOperation(doOp);

    OnSelectionChanged(GetDataManagerSelection());
}

void svModelEdit::UpdateFacesAndNodes()
{
    if(m_Model==NULL||m_ModelNode.IsNull()) return;

    int timeStep=GetTimeStep();
    svModelElement* modelElement=m_Model->GetModelElement(timeStep);

    if(modelElement==NULL) return;

    m_RemovingNode=true;

    mitk::DataStorage::SetOfObjects::ConstPointer nodesToRemove=GetDataStorage()->GetDerivations(m_ModelNode,nullptr,false);
    if( !nodesToRemove->empty())
    {
        GetDataStorage()->Remove(nodesToRemove);
    }

    std::vector<svModelElement::svFace*> faces=modelElement->GetFaces();

    for(int i=0;i<faces.size();i++)
    {
        vtkPolyData *facepd = vtkPolyData::New();
        int faceid=i+1;
        PlyDtaUtils_GetFacePolyData(modelElement->GetVtkPolyDataModel(), &faceid, facepd);
        faces[i]->vpd=facepd;
        mitk::Surface::Pointer surface=mitk::Surface::New();
        surface->SetVtkPolyData((faces[i]->vpd));
        mitk::DataNode::Pointer surfaceNode = mitk::DataNode::New();
        surfaceNode->SetData(surface);
        surfaceNode->SetName(faces[i]->name);
        surfaceNode->SetBoolProperty("pickable", true);
        faces[i]->node=surfaceNode;
        GetDataStorage()->Add(surfaceNode,m_ModelNode);
    }

    m_RemovingNode=false;

    OnSelectionChanged(GetDataManagerSelection());
}
