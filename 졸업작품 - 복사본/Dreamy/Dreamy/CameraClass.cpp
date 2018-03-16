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
- *****�ſ��*******
 - ���� Render�Լ����� ������� �����ϴ°Ͱ� ���� ������� �ݻ� ����� �����.
 - �� ���� �������� Z�࿡ �ִ� �������� ī�޶� ���� distance�� ���ڷ� �޾� �ݻ縦 ���� �� ���� ������Ų��.
 - �̷��� �Ͽ� ���̴��� ����� �ݻ� �� ����� ����� ����.
------------------------------------------------------------------------------------------------------*/
void CameraClass::RenderReflection(float height)//float distance, float object)
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
	//position.y = -object + (distance * 2.0f);
	position.y = -m_positionY + (height *2.0f);
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

/*------------------------------------------------------------------------------------------------------
�̸�: RenderWaterReflection()
�뵵:
- *****ȣ����*******
- ȣ���� ���� �ݻ� �� ����� ����µ� ���ȴ�.
- regular view matrix�� �������� �̰��� plane�� ���̸� ���̽��� Y���� ������Ų��. pitch(X��)�� ������Ų��.

- ������� �����ϴµ� �ʿ��� ��: ī�޶��� ��ġ+ī�޶� ���� ����+ī�޶��� up����
- ī�޶� ���� ����, up�� ��� �����̸� ī�޶��� ��ġ�� rotationMatrix�� ��ȯ�Ǿ���Ѵ�.
------------------------------------------------------------------------------------------------------*/
void CameraClass::RenderWaterReflection(float height)
{
	D3DXVECTOR3 up, position, lookAt;
	float yaw, pitch, roll;
	D3DXMATRIX rotationMatrix;
	//ī�޶��� ��ǥ��
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	position.x = m_positionX;
	position.y = -m_positionY + (height * 2.0f); // planer�� �ݻ縦 ���� ī�޶��� y���� �������ش�.(�ϴÿ��� �����ϴϱ� height)
	position.z = m_positionZ;

	// ī�޶� ���� ��
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	//��ǥ����� ȸ�������ش�.
	//X���� ���������ش�.
	pitch = -m_rotationX * 0.0174532925f;
	yaw = m_rotationY * 0.0174532925f;
	roll = m_rotationZ * 0.0174532925f;

	//rotation����� ������.
	D3DXMatrixRotationYawPitchRoll(&rotationMatrix, yaw, pitch, roll);

	//���� ���� ���͸� rotation matrix�� ��ȯ��Ų��. 
	D3DXVec3TransformCoord(&lookAt, &lookAt, &rotationMatrix);
	D3DXVec3TransformCoord(&up, &up, &rotationMatrix);

	
	//���� ����� ��ġ�� �°� ī�޶��� ��ġ�� �̵���Ų��.
	//���ʹ� ������ �����Ѵ�.
	lookAt = position + lookAt;

	//ī�޶��� ��ġ, ī�޶� ���� ����, ī�޶��� up���͸� ȥ���ؼ� ������� �����.
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
