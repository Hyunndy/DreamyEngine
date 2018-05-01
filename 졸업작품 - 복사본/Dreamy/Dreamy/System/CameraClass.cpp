////////////////////////////////////////////////////////////////////////////////
// Filename: cameraclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "cameraclass.h"
const float CameraClass::screenNear = 0.1f;
const float CameraClass::screenDepth = 1000.0f;

CameraClass::CameraClass()
{
	m_positionX = 0.0f;
	m_positionY = 0.0f;
	m_positionZ = 0.0f;

	m_rotationX = 0.0f;
	m_rotationY = 0.0f;
	m_rotationZ = 0.0f;
}


CameraClass::CameraClass(const CameraClass& other)
{
}


CameraClass::~CameraClass()
{
}


void CameraClass::SetPosition(D3DXVECTOR3 pos)
{
	m_positionX = pos.x;
	m_positionY = pos.y;
	m_positionZ = pos.z;

	return;
}


void CameraClass::SetRotation(D3DXVECTOR3 rot)
{
	m_rotationX = rot.x;
	m_rotationY = rot.y;
	m_rotationZ = rot.z;


	return;
}


D3DXVECTOR3 CameraClass::GetPosition()
{
	return D3DXVECTOR3(m_positionX, m_positionY, m_positionZ);
}


D3DXVECTOR3 CameraClass::GetRotation()
{
	return D3DXVECTOR3(m_rotationX, m_rotationY, m_rotationZ);
}

/*------------------------------------------------------------------------------------------------------
이름: Render()	
용도: 
 - 카메라의 위치와 회전 정보를 이용하여 뷰 행렬을 갱신한다.
 - 1. UP 벡터와 위치, 회전, 보는 방향를 초기화 한다.
 - 2. 화면의 원점에서 우선 카메라의 X,Y,Z의 회전 정도로 카메라의 회전을 수행한다.
 - 3. 회전이 잘 이루어지면 카메라를 3D 세계의 올바른 위치로 옮겨 놓는다.
 - 4. POSITION, LOOKAT, UP의 값이 잘 설정되어있다면 D3DXMatrixLookAtLH함수를 사용해 현재 카메라의 위치와 회전 정도를 표현하는 뷰 행렬을 만들어낸다.
------------------------------------------------------------------------------------------------------*/
void CameraClass::Render()
{
	D3DXVECTOR3 up, position, lookAt;
	float yaw, pitch, roll;
	D3DXMATRIX rotationMatrix;


	// 1
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	position.x = m_positionX;
	position.y = m_positionY;
	position.z = m_positionZ;

	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	//2
	//카메라를 얼마나 회전하고 싶은지 정한다.
	pitch = m_rotationX * 0.0174532925f;
	yaw = m_rotationY * 0.0174532925f;
	roll = m_rotationZ * 0.0174532925f;

	//3
	//2번에서 카메라를 얼마나 회전하고 싶은지 정한 각도들을 rotationMatrix에 넣어 회전 행렬을 만든다.
	D3DXMatrixRotationYawPitchRoll(&rotationMatrix, yaw, pitch, roll);
	
	// D3DXVec3TransformCoord(D3DXVECTOR3 *pOut,CONST D3DXVECTOR3 *pV,CONST D3DXMATRIX *pM) = 지정된 행렬에 의해 3d벡터를 변환 해, 그 결과를 w = 1에 투영한다.
	// 위에서 만든 rotationmatrix회전행렬로 카메라의 보는 방향과 up벡터를 변환한다.
	D3DXVec3TransformCoord(&lookAt, &lookAt, &rotationMatrix);
	D3DXVec3TransformCoord(&up, &up, &rotationMatrix);

	// 지금 있는곳에서 원하는 곳을 볼 수 있게 position에 더해준다.
	lookAt = position + lookAt;

	//4
	//최종적인 카메라의 뷰행렬을 생성한다.
	D3DXMatrixLookAtLH(&m_viewMatrix, &position, &lookAt, &up);

	return;
}

/*------------------------------------------------------------------------------------------------------
이름: GetViewMatrix()
용도:
 - 카메라의 뷰행렬을 얻는다.
 - 뷰 행렬은 HLSL정점 셰이더에서 사용하는 중요한 세 행렬 중 하나이다.
------------------------------------------------------------------------------------------------------*/
void CameraClass::GetViewMatrix(D3DXMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
	return;
}

/*------------------------------------------------------------------------------------------------------
이름: RenderReflection()
용도:
- *****거울용*******
 - 보통 Render함수에서 뷰행렬을 생성하는것과 같은 방법으로 반사 행렬을 만든다.
 - 한 가지 차이점은 Z축에 있는 원점과의 카메라 간의 distance를 인자로 받아 반사를 위해 이 값을 역전시킨다.
 - 이렇게 하여 셰이더에 사용할 반사 뷰 행렬을 만들어 낸다.
------------------------------------------------------------------------------------------------------*/
void CameraClass::RenderReflection(float height)//float distance, float object)
{
	D3DXVECTOR3 up, position, lookAt;

	float radians;


	// 위쪽을 가리키는 벡터를 생성한다.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// 월드에 카메라 위치를 설정합니다.
	// 평면 반사를 위해 카메라의 Y값을 역전시킵니다.
	position.x = m_positionX;
	//position.y = -object + (distance * 2.0f);
	position.y = -m_positionY + (height *2.0f);
	position.z = m_positionZ; 
	
	
	// 회전을 라디안 값으로 계산합니다.
	radians = m_rotationY * 0.0174532925f;
	// 카메라가 보는 방향을 설정합니다. 
	lookAt.x = sinf(radians) + m_positionX; 
	lookAt.y = position.y; 
	lookAt.z = cosf(radians) + m_positionZ;


	//위 세 벡터를 이용해 뷰 행렬을 생성한다.
	D3DXMatrixLookAtLH(&m_reflectionViewMatrix, &position, &lookAt, &up);

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
void CameraClass::RenderWaterReflection(float height)
{
	D3DXVECTOR3 up, position, lookAt;
	float yaw, pitch, roll;
	D3DXMATRIX rotationMatrix;
	//카메라의 좌표축
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	position.x = m_positionX;
	position.y = -m_positionY + (height * 2.0f); // planer한 반사를 위해 카메라의 y축을 뒤집어준다.(하늘에서 쏴야하니까 height)
	position.z = m_positionZ;

	// 카메라가 보는 곳
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	//좌표축들을 회전시켜준다.
	//X축을 반전시켜준다.
	pitch = -m_rotationX * 0.0174532925f;
	yaw = m_rotationY * 0.0174532925f;
	roll = m_rotationZ * 0.0174532925f;

	//rotation행렬을 만들어낸다.
	D3DXMatrixRotationYawPitchRoll(&rotationMatrix, yaw, pitch, roll);

	//보는 방향 벡터를 rotation matrix로 전환시킨다. 
	D3DXVec3TransformCoord(&lookAt, &lookAt, &rotationMatrix);
	D3DXVec3TransformCoord(&up, &up, &rotationMatrix);

	
	//보는 사람의 위치에 맞게 카메라의 위치를 이동시킨다.
	//벡터는 방향을 포함한다.
	lookAt = position + lookAt;

	//카메라의 위치, 카메라가 보는 방향, 카메라의 up벡터를 혼합해서 뷰행렬을 만든다.
	D3DXMatrixLookAtLH(&m_reflectionViewMatrix, &position, &lookAt, &up);

	return;
}

void CameraClass::GetWaterReflectionViewMatrix(D3DXMATRIX& viewMatrix)
{
	viewMatrix =  m_WaterReflectionViewMatrix;
}

D3DXMATRIX CameraClass::GetReflectionViewMatrix()
{
	return m_reflectionViewMatrix;
	
}

D3DXMATRIX CameraClass::GetProjectionMatrix()
{
	return projection;
}

void CameraClass::UpdateProjectionMatrix()
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

void CameraClass::GenerateBaseViewMatrix()
{
	D3DXVECTOR3 up, position, lookAt;
	float yaw, pitch, roll;
	D3DXMATRIX rotationMatrix;


	// Setup the vector that points upwards.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// Setup the position of the camera in the world.
	position.x = m_positionX;
	position.y = m_positionY;
	position.z = m_positionZ;

	// Setup where the camera is looking by default.
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
	pitch = m_rotationX * 0.0174532925f;
	yaw = m_rotationY * 0.0174532925f;
	roll = m_rotationZ * 0.0174532925f;

	// Create the rotation matrix from the yaw, pitch, and roll values.
	D3DXMatrixRotationYawPitchRoll(&rotationMatrix, yaw, pitch, roll);

	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
	D3DXVec3TransformCoord(&lookAt, &lookAt, &rotationMatrix);
	D3DXVec3TransformCoord(&up, &up, &rotationMatrix);

	// Translate the rotated camera position to the location of the viewer.
	lookAt = position + lookAt;

	// Finally create the view matrix from the three updated vectors.
	D3DXMatrixLookAtLH(&m_baseViewMatrix, &position, &lookAt, &up);

	return;
}


void CameraClass::GetBaseViewMatrix(D3DXMATRIX& viewMatrix)
{
	viewMatrix = m_baseViewMatrix;
	return;
}
