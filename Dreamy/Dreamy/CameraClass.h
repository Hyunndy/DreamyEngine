////////////////////////////////////////////////////////////////////////////////
// Filename: cameraclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _CAMERACLASS_H_
#define _CAMERACLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3dx10math.h>

/*
	�츮�� ��� ����� �������� ���� ������ DirectX11���� �����ϴ� ī�޶� Ŭ����.
	ī�޶� Ŭ������ ī�޶��� ��ġ, ���� ȸ�� ���¸� ��� ������ �Ѵ�.
	�� ������ �̿��Ͽ� �������ÿ� HLSL���̴����� ����� �� ����� �����Ѵ�.
*/

////////////////////////////////////////////////////////////////////////////////
// Class name: CameraClass
////////////////////////////////////////////////////////////////////////////////
class CameraClass
{
public:
	CameraClass();
	CameraClass(const CameraClass&);
	~CameraClass();

	void SetPosition(float, float, float);
	void SetRotation(float, float, float);

	D3DXVECTOR3 GetPosition();
	D3DXVECTOR3 GetRotation();

	//ī�޶��� ��ġ�� ȸ�� ���¿� ����� �� ����� �����Ѵ�.
	void Render();

	//���̴����� �������� ����� �� �ֵ��� ī�޶� ��ü�� �� ����� �޾ƿ´�.
	void GetViewMatrix(D3DXMATRIX&);

private:
	float m_positionX, m_positionY, m_positionZ;
	float m_rotationX, m_rotationY, m_rotationZ;
	D3DXMATRIX m_viewMatrix;
};

#endif