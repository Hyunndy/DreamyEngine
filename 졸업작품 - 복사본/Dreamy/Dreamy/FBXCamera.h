#pragma once
#include "D3D.h"

class Camera
{
public:
	Camera();
	~Camera();

	static Camera* Get();
	static void Delete();

	void GetPosition(D3DXVECTOR3* position)
	{
		*position = this->position;
	}

	void UpdateRotationMatrix();
	void UpdateOrthoMatrix();
	void UpdateProjectionMatrix();
	void UpdateViewMatrix();
	void Update();

	void Move(D3DXVECTOR3 translation);
	void MoveUp();
	void MoveDown();
	void MoveLeft();
	void MoveRight();
	void MoveForward();
	void MoveBackward();

	void Rotate(D3DXVECTOR2 rotate);
	void RotateUp();
	void RotateDown();
	void RotateLeft();
	void RotateRight();

	void SetPosition(D3DXVECTOR3 pos);



	void GetView(D3DXMATRIX* view)
	{
		memcpy(view, this->view, sizeof(D3DXMATRIX));
	}

	void GetProjection(D3DXMATRIX* projection)
	{
		memcpy(projection, this->projection, sizeof(D3DXMATRIX));
	}

	void GetOrtho(D3DXMATRIX* ortho)
	{
		memcpy(ortho, this->ortho, sizeof(D3DXMATRIX));
	}

	void GetRay(D3DXVECTOR3* origin, D3DXVECTOR3* direction);

private:
	static Camera* instance;

	const static float screenNear; //거리
	const static float screenDepth; //깊이

	D3DXVECTOR3 position; //위치
	D3DXVECTOR3 forward; //전방
	D3DXVECTOR3 right; //우측
	D3DXVECTOR3 up; //위쪽
	D3DXVECTOR2 rotate; //회전

	D3DXMATRIX view;
	D3DXMATRIX rotation;

	float translationSpeed;
	float rotationSpeed;


	D3D11_VIEWPORT viewport;
	float fieldOfView; //시야각
	float screenAspect; //화면비율

	D3DXMATRIX projection;
	D3DXMATRIX ortho;
};