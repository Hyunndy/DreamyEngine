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
�̸�: Render()	
�뵵: 
 - ī�޶��� ��ġ�� ȸ�� ������ �̿��Ͽ� �� ����� �����Ѵ�.
 - 1. UP ���Ϳ� ��ġ, ȸ��, ���� ���⸦ �ʱ�ȭ �Ѵ�.
 - 2. ȭ���� �������� �켱 ī�޶��� X,Y,Z�� ȸ�� ������ ī�޶��� ȸ���� �����Ѵ�.
 - 3. ȸ���� �� �̷������ ī�޶� 3D ������ �ùٸ� ��ġ�� �Ű� ���´�.
 - 4. POSITION, LOOKAT, UP�� ���� �� �����Ǿ��ִٸ� D3DXMatrixLookAtLH�Լ��� ����� ���� ī�޶��� ��ġ�� ȸ�� ������ ǥ���ϴ� �� ����� ������.
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
	//ī�޶� �󸶳� ȸ���ϰ� ������ ���Ѵ�.
	pitch = m_rotationX * 0.0174532925f;
	yaw = m_rotationY * 0.0174532925f;
	roll = m_rotationZ * 0.0174532925f;

	//3
	//2������ ī�޶� �󸶳� ȸ���ϰ� ������ ���� �������� rotationMatrix�� �־� ȸ�� ����� �����.
	D3DXMatrixRotationYawPitchRoll(&rotationMatrix, yaw, pitch, roll);
	
	// D3DXVec3TransformCoord(D3DXVECTOR3 *pOut,CONST D3DXVECTOR3 *pV,CONST D3DXMATRIX *pM) = ������ ��Ŀ� ���� 3d���͸� ��ȯ ��, �� ����� w = 1�� �����Ѵ�.
	// ������ ���� rotationmatrixȸ����ķ� ī�޶��� ���� ����� up���͸� ��ȯ�Ѵ�.
	D3DXVec3TransformCoord(&lookAt, &lookAt, &rotationMatrix);
	D3DXVec3TransformCoord(&up, &up, &rotationMatrix);

	// ���� �ִ°����� ���ϴ� ���� �� �� �ְ� position�� �����ش�.
	lookAt = position + lookAt;

	//4
	//�������� ī�޶��� ������� �����Ѵ�.
	D3DXMatrixLookAtLH(&m_viewMatrix, &position, &lookAt, &up);

	return;
}

/*------------------------------------------------------------------------------------------------------
�̸�: GetViewMatrix()
�뵵:
 - ī�޶��� ������� ��´�.
 - �� ����� HLSL���� ���̴����� ����ϴ� �߿��� �� ��� �� �ϳ��̴�.
------------------------------------------------------------------------------------------------------*/
void CameraClass::GetViewMatrix(D3DXMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
	return;
}

/*------------------------------------------------------------------------------------------------------
�̸�: RenderReflection()
�뵵:
 - ���� Render�Լ����� ������� �����ϴ°Ͱ� ���� ������� �ݻ� ����� �����.
 - �� ���� �������� Z�࿡ �ִ� �������� ī�޶� ���� distance�� ���ڷ� �޾� �ݻ縦 ���� �� ���� ������Ų��.
 - �̷��� �Ͽ� ���̴��� ����� �ݻ� �� ����� ����� ����.

------------------------------------------------------------------------------------------------------*/
void CameraClass::RenderReflection(float distance, float object)
{
	D3DXVECTOR3 up, position, lookAt;

	float radians;


	// ������ ����Ű�� ���͸� �����Ѵ�.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// ���忡 ī�޶� ��ġ�� �����մϴ�.
	// ��� �ݻ縦 ���� ī�޶��� Y���� ������ŵ�ϴ�.
	position.x = m_positionX;
	position.y = -object + (distance * 2.0f);
	position.z = m_positionZ; 
	
	
	// ȸ���� ���� ������ ����մϴ�.
	radians = m_rotationY * 0.0174532925f;
	// ī�޶� ���� ������ �����մϴ�. 
	lookAt.x = sinf(radians) + m_positionX; 
	lookAt.y = position.y; 
	lookAt.z = cosf(radians) + m_positionZ;


	//�� �� ���͸� �̿��� �� ����� �����Ѵ�.
	D3DXMatrixLookAtLH(&m_reflectionViewMatrix, &position, &lookAt, &up);

	return;
}

D3DXMATRIX CameraClass::GetReflectionViewMatrix()
{
	return m_reflectionViewMatrix;
}