#include "FBXCamera.h"
#include "../FBXAnimation/AnimationFrames.h"


Camera* Camera::instance = NULL;

const float Camera::screenNear = 0.1f;
const float Camera::screenDepth = 1000.0f;

Camera::Camera()
	: position(630.0f, 56.0f, 96.0f)
	, forward(0.0, 0, 0), right(0, 0, 0), up(0, 0, 0)
	, rotate(0, 0)
	, translationSpeed(100.0f), rotationSpeed(2.0f)
{
	D3DXMatrixIdentity(&view);
	D3DXMatrixIdentity(&projection);

	UpdateOrthoMatrix();
	UpdateProjectionMatrix();
	UpdateRotationMatrix();
	UpdateViewMatrix();

}

Camera::~Camera()
{
}

Camera * Camera::Get()
{
	if (instance == NULL)
		instance = new Camera();

	return instance;
}

void Camera::Delete()
{
	SAFE_DELETE(instance);
}

void Camera::UpdateRotationMatrix()
{
	D3DXMATRIX x, y;
	D3DXMatrixRotationX(&x, rotate.x);
	D3DXMatrixRotationY(&y, rotate.y);

	rotation = x * y;

	D3DXVec3TransformCoord(&forward, &D3DXVECTOR3(0, 0, 1), &rotation);
	D3DXVec3TransformCoord(&right, &D3DXVECTOR3(1, 0, 0), &rotation);
	D3DXVec3TransformCoord(&up, &D3DXVECTOR3(0, 1, 0), &rotation);
}

void Camera::UpdateOrthoMatrix()
{
	D3DInfo info;
	D3D::GetInfo(&info);

	D3DXMatrixOrthoLH(&ortho, (float)info.screenWidth, (float)info.screenHeight, screenNear, screenDepth);

}

void Camera::UpdateProjectionMatrix()
{
	D3DInfo info;
	D3D::GetInfo(&info);

	viewport.Width = (float)info.screenWidth;
	viewport.Height = (float)info.screenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	D3D::GetDeviceContext()->RSSetViewports(1, &viewport);

	fieldOfView = (float)D3DX_PI / 4.0f;
	screenAspect = (float)info.screenWidth / (float)info.screenHeight;

	D3DXMatrixPerspectiveFovLH(&projection, fieldOfView, screenAspect, screenNear, screenDepth);
}

void Camera::UpdateViewMatrix()
{
	D3DXMatrixLookAtLH(&view, &position, &(position + forward), &up);
}

void Camera::SetPosition(D3DXVECTOR3 pos)
{
	position = pos;
	UpdateViewMatrix();
}

void Camera::Update()
{
}

void Camera::Move(D3DXVECTOR3 translation)
{
	position += translation * Frames::TimeElapsed();
	UpdateViewMatrix();
}

void Camera::MoveUp()
{
	Move(up * translationSpeed);
}

void Camera::MoveDown()
{
	Move(-up * translationSpeed);
}

void Camera::MoveLeft()
{
	Move(-right * translationSpeed);
}

void Camera::MoveRight()
{
	Move(right * translationSpeed);
}

void Camera::MoveForward()
{
	Move(forward * translationSpeed);
}

void Camera::MoveBackward()
{
	Move(-forward * translationSpeed);
}

void Camera::Rotate(D3DXVECTOR2 rotate)
{
	this->rotate.x += rotate.x * Frames::TimeElapsed() * rotationSpeed;
	this->rotate.y += rotate.y * Frames::TimeElapsed() * rotationSpeed;

	UpdateRotationMatrix();
	UpdateViewMatrix();
}

void Camera::RotateUp()
{
	Rotate(D3DXVECTOR2(-1, 0));
}

void Camera::RotateDown()
{
	Rotate(D3DXVECTOR2(1, 0));
}

void Camera::RotateLeft()
{
	Rotate(D3DXVECTOR2(0, 1));
}

void Camera::RotateRight()
{
	Rotate(D3DXVECTOR2(0, -1));
}
void Camera::ResetViewPort()
{
	// Set the viewport.
	D3D::GetDeviceContext()->RSSetViewports(1, &viewport);

	return;
}
/*------------------------------------------------------------------------------------------------------
이름: RenderWaterReflection()
용도:
- *****호수용*******
- 호수에 사용될 반사 뷰 행렬을 만드는데 사용된다.
- regular view matrix와 차이점은 이것은 plane의 높이를 베이스로 Y축을 반전시킨다. pitch(X축)도 반전시킨다.

- 뷰행렬을 생성하는데 필요한 것: 카메라의 위치+카메라가 보는 방향+카메라의 up벡터
- 카메라가 보는 방향, up은 모두 벡터이며 카메라의 위치로 rotationMatrix로 변환되어야한다.
------------------------------------------------------------------------------------------------------*/
D3DXMATRIX Camera::RenderWaterReflection(float height)
{
	D3DXVECTOR3 w_up, w_position, w_lookAt;

	D3DXMATRIX w_rotation;
	//카메라의 좌표축
	w_up.x = 0.0f;
	w_up.y = 1.0f;
	w_up.z = 0.0f;

	w_position.x = position.x;
	w_position.y = -position.y + (height * 2.0f); // planer한 반사를 위해 카메라의 y축을 뒤집어준다.(하늘에서 쏴야하니까 height)
	w_position.z = position.z;



	D3DXMATRIX x, y;
	D3DXMatrixRotationX(&x, -rotate.x);
	D3DXMatrixRotationY(&y, rotate.y);

	w_rotation = x * y;

	D3DXVec3TransformCoord(&w_lookAt , &D3DXVECTOR3(0, 0, 1), &rotation);
	D3DXVec3TransformCoord(&w_up, &D3DXVECTOR3(0, 1, 0), &rotation);



	//보는 사람의 위치에 맞게 카메라의 위치를 이동시킨다.
	//벡터는 방향을 포함한다.
	w_lookAt = w_position + w_lookAt;

	//카메라의 위치, 카메라가 보는 방향, 카메라의 up벡터를 혼합해서 뷰행렬을 만든다.
	D3DXMatrixLookAtLH(&m_reflectionViewMatrix, &w_position, &w_lookAt, &w_up);

	return m_reflectionViewMatrix;
}


//void Camera::GetRay(D3DXVECTOR3 * origin, D3DXVECTOR3 * direction)
//{
//	D3DXVECTOR3 mouse = Mouse::Get()->GetPosition();
//
//	D3DInfo info;
//	D3D::GetInfo(&info);
//
//
//	D3DXVECTOR2 point;
//	point.x = ((2.0f * mouse.x) / info.screenWidth) - 1.0f;
//	point.y = (((2.0f * mouse.y) / info.screenHeight) - 1.0f) * -1.0f;
//
//	point.x = point.x / projection._11;
//	point.y = point.y / projection._22;
//
//	D3DXMATRIX invView;
//	D3DXMatrixInverse(&invView, NULL, &view);
//
//	D3DXVECTOR3 dir;
//	dir.x = (point.x * invView._11) + (point.y * invView._21) + invView._31;
//	dir.y = (point.x * invView._12) + (point.y * invView._22) + invView._32;
//	dir.z = (point.x * invView._13) + (point.y * invView._23) + invView._33;
//
//	D3DXVec3Normalize(&dir, &dir);
//
//	*origin = position;
//	*direction = dir;
//}
