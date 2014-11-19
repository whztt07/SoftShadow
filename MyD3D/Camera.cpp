#include "Camera.h"

Camera::Camera()
{
    memset(Transform.M, 0, sizeof(Transform));
    memset(LastRot.M, 0, sizeof(LastRot));
    memset(ThisRot.M, 0, sizeof(ThisRot));

    Transform.M[0] = Transform.M[5] = Transform.M[10] = Transform.M[15] = 1.0f;
    LastRot.M[0] = LastRot.M[4] = LastRot.M[8] = 1.0f;
    ThisRot.M[0] = ThisRot.M[4] = ThisRot.M[8] = 1.0f;

    isClicked  = false;										// NEW: Clicking The Mouse?
    isRClicked = false;										// NEW: Clicking The Right Mouse Button?
    isDragging = false;					                    // NEW: Dragging The Mouse?
}

//--------------------------------------------------------------------------------------
// Calculates the projection matrix based on input params
//--------------------------------------------------------------------------------------
void Camera::SetProjParams( float fFOV, float fAspect, float fNearPlane, float fFarPlane )
{
    // Set attributes for the projection matrix
    m_fFOV = fFOV;
    m_fAspect = fAspect;
    m_fNearPlane = fNearPlane;
    m_fFarPlane = fFarPlane;

    m_mProj = XMMatrixPerspectiveFovLH(fFOV, fAspect, fNearPlane, fFarPlane );
}

//--------------------------------------------------------------------------------------
// Client can call this to change the position and direction of camera
//--------------------------------------------------------------------------------------
void Camera::SetViewParams( XMVECTOR* pvEyePt, XMVECTOR* pvLookatPt, XMVECTOR* pvUp )
{
    if( NULL == pvEyePt || NULL == pvLookatPt )
        return;

    m_vEye = *pvEyePt;
    m_vLookAt = *pvLookatPt;
    m_vUp = *pvUp;

    // Calc the view matrix
    m_mView = XMMatrixLookAtLH(m_vEye, m_vLookAt, m_vUp );
}

//--------------------------------------------------------------------------------------
// Update camera view matrix per frame
//------------------------------------------------------------------------------------
void Camera::UpdateView()
{
    if (isRClicked)													// If Right Mouse Clicked, Reset All Rotations
    {
		Matrix3fSetIdentity(&LastRot);								// Reset Rotation
		Matrix3fSetIdentity(&ThisRot);								// Reset Rotation
        Matrix4fSetRotationFromMatrix3f(&Transform, &ThisRot);		// Reset Rotation
    }

    if (!isDragging)												// Not Dragging
    {
        if (isClicked)												// First Click
        {
			isDragging = true;										// Prepare For Dragging
			LastRot = ThisRot;										// Set Last Static Rotation To Last Dynamic One
			ArcBall.click(&MousePt);								// Update Start Vector And Prepare For Dragging
        }
    }
    else
    {
        if (isClicked)												// Still Clicked, So Still Dragging
        {
            Quat4fT     ThisQuat;

            ArcBall.drag(&MousePt, &ThisQuat);						// Update End Vector And Get Rotation As Quaternion
            Matrix3fSetRotationFromQuat4f(&ThisRot, &ThisQuat);		// Convert Quaternion Into Matrix3fT
            Matrix3fMulMatrix3f(&ThisRot, &LastRot);				// Accumulate Last Rotation Into This One
            Matrix4fSetRotationFromMatrix3f(&Transform, &ThisRot);	// Set Our Final Transform's Rotation From This One
        }
        else														// No Longer Dragging
            isDragging = false;
    }
}