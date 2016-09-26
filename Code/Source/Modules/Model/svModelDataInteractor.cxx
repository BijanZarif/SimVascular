#define SVMODELDATAINTERACTOR_DBG MITK_DEBUG("svModelDataInteractor") << __LINE__ << ": "

#include "svModelDataInteractor.h"
#include "svModel.h"

#include "mitkInteractionPositionEvent.h"
#include "mitkInternalEvent.h"

#include "mitkBaseRenderer.h"
#include "mitkRenderingManager.h"
#include "mitkPlaneGeometry.h"
#include "mitkInternalEvent.h"
#include "mitkDispatcher.h"
#include "mitkBaseRenderer.h"
#include "mitkUndoController.h"

#include <iostream>
using namespace std;

svModelDataInteractor::svModelDataInteractor()
    : mitk::DataInteractor()
    , m_Model(NULL)
    , m_SelectedFaceIndex(-1)
{
}

svModelDataInteractor::~svModelDataInteractor()
{
}

void svModelDataInteractor::ConnectActionsAndFunctions()
{
    CONNECT_CONDITION("isOverObject", CheckOverObject);

    CONNECT_FUNCTION("selectObject",SelectObject);
}

void svModelDataInteractor::DataNodeChanged()
{
    if(this->GetDataNode() != nullptr)
    {
        m_Model = dynamic_cast<svModel*>(this->GetDataNode()->GetData());

        if (m_Model == NULL)
            MITK_ERROR << "svModelDataInteractor::DataNodeChanged(): DataNode has to contain a model.";
    }
    else
        m_Model = NULL;
}

bool svModelDataInteractor::CheckOverObject(const mitk::InteractionEvent* interactionEvent)
{
    const mitk::InteractionPositionEvent* positionEvent = dynamic_cast<const mitk::InteractionPositionEvent*>(interactionEvent);
    if(positionEvent == NULL)
        return false;

    if(m_Model==NULL)
        return false;

    svModelElement* modelElement=m_Model->GetModelElement();
    if(modelElement==NULL)
        return false;

    mitk::Point2D currentPickedDisplayPoint = positionEvent->GetPointerPositionOnScreen();
    mitk::Point3D currentPickedPoint;

    mitk::DataNode* pickedNode=interactionEvent->GetSender()->PickObject(currentPickedDisplayPoint, currentPickedPoint);

    std::vector<svModelElement::svFace*> faces=modelElement->GetFaces();

    for(int i=0;i<faces.size();i++)
    {
        if(faces[i]->node!=NULL && faces[i]->node==pickedNode)
        {
            m_SelectedFaceIndex=i;
            return true;
        }
    }

    return false;
}

void svModelDataInteractor::SelectObject(mitk::StateMachineAction*, mitk::InteractionEvent* interactionEvent)
{
    const mitk::InteractionPositionEvent* positionEvent = dynamic_cast<const mitk::InteractionPositionEvent*>(interactionEvent);
    if(positionEvent == NULL)
        return;

    if(m_Model==NULL)
        return;

    svModelElement* modelElement=m_Model->GetModelElement();
    if(modelElement==NULL)
        return;

    modelElement->SetSelectedFaceIndex(m_SelectedFaceIndex);

    m_Model->InvokeEvent( svModelSelectFaceEvent() );

//    this->GetDataNode()->SetColor(1.0, 0.0, 0.0);
//    interactionEvent->GetSender()->GetRenderingManager()->RequestUpdateAll();
}

