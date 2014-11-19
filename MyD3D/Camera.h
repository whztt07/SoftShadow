#pragma once

#include "common.h"
#include "ArcBall.h"

class Camera
{
public:
    Camera();

    XMVECTOR m_vEye;
    XMVECTOR m_vLookAt;
    XMVECTOR m_vUp;
    XMMATRIX m_mView;
    
    float m_fFOV;
    float m_fAspect;
    float m_fNearPlane;
    float m_fFarPlane;
    XMMATRIX m_mProj;

    void SetViewParams( XMVECTOR* pvEyePt, XMVECTOR* pvLookatPt, XMVECTOR* pvUp );
    void SetProjParams(float fFOV, float fAspect, float fNearPlane, float fFarPlane);

    ArcBall_t   ArcBall;
    Matrix4fT   Transform;      // NEW: Final Transform
    Matrix3fT   LastRot;        // NEW: Last Rotation
    Matrix3fT   ThisRot;        // NEW: This Rotation
    Point2fT    MousePt;		// NEW: Current Mouse Point
    bool        isClicked;		// NEW: Clicking The Mouse?
    bool        isRClicked;		// NEW: Clicking The Right Mouse Button?
    bool        isDragging;		// NEW: Dragging The Mouse?

    void UpdateView();
};