////////////////////////////////////////////////////////////////////////////////
// Filename: cameraclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "cameraclass.h"


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


void CameraClass::SetPosition(float x, float y, float z)
{
	m_positionX = x;
	m_positionY = y;
	m_positionZ = z;
	return;
}


void CameraClass::SetRotation(float x, float y, float z)
{
	m_rotationX = x;
	m_rotationY = y;
	m_rotationZ = z;
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

/*
	카메라의 위치와 회전 정보를 이용하여 뷰 행렬을 갱신한다.

	1. UP 벡터와 위치, 회전, 보는 방향를 초기화 한다.
	2. 화면의 원점에서 우선 카메라의 X,Y,Z의 회전 정도로 카메라의 회전을 수행한다.
	3. 회전이 잘 이루어지면 카메라를 3D 세계의 올바른 위치로 옮겨 놓는다.
	4. POSITION, LOOKAT, UP의 값이 잘 설정되어있다면 D3DXMatrixLookAtLH함수를 사용해 현재 카메라의 위치와 회전 정도를 표현하는 뷰 행렬을 만들어낸다.
*/
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

// 카메라의 뷰행렬을 얻는다.
// 뷰 행렬은 HLSL정점 셰이더에서 사용하는 중요한 세 행렬 중 하나이다.
void CameraClass::GetViewMatrix(D3DXMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
	return;
}