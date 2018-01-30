#include "FrustumClass.h"


FrustumClass::FrustumClass(){}

FrustumClass::FrustumClass(const FrustumClass& other){}

FrustumClass::~FrustumClass(){}



/*-------------------------------------------------------------------------------------------------------
이름:ConstructFrustum()
용도:
- 갱신한 카메라 위치를 기초로 m_planes변수를 이용해 시야 절두체의 6개 평면을 계산하고 저장한다.
- 카메라가 한 번 그리고 난 뒤로 GraphicsClass에 의해 각 프레임 마다 ConstFrustum()이 호출된다.

- GraphicsClass에서 화면의 depth, 투영 행렬, 뷰 행렬을 전달한다.
- 이 입력들을 이용해 현재 프레임의 시야 절두체 행렬을 생성한다.
- 새로운 시야 절두체 행렬로 시야 절두체를 구성하는 여섯 개의 평면을 계산해낸다.

- 뷰 행렬과 업데이트된 투영행렬을 Multiply해서 시야 절두체 행렬을 만드는듯!
---------------------------------------------------------------------------------------------------------*/
void FrustumClass::ConstructFrustum(float screenDepth, D3DXMATRIX projectionMatrix, D3DXMATRIX viewMatrix)
{
	float zMinimum, r;
	D3DXMATRIX matrix;
	
	// Calculate the minimum Z distance in the frustum.
	zMinimum = -projectionMatrix._43 / projectionMatrix._33;
	r = screenDepth / (screenDepth - zMinimum);
	projectionMatrix._33 = r;
	projectionMatrix._43 = -r * zMinimum; 
	
	// Create the frustum matrix from the view matrix and updated projection matrix.
	D3DXMatrixMultiply(&matrix, &viewMatrix, &projectionMatrix);
	
	// 시야 절두체의 6개의 평면 정의.
	// Calculate near plane of frustum. 
	m_planes[0].a = matrix._14 + matrix._13;
	m_planes[0].b = matrix._24 + matrix._23;
	m_planes[0].c = matrix._34 + matrix._33;
	m_planes[0].d = matrix._44 + matrix._43;
	D3DXPlaneNormalize(&m_planes[0], &m_planes[0]);
	
	// Calculate far plane of frustum.
	m_planes[1].a = matrix._14 - matrix._13;
	m_planes[1].b = matrix._24 - matrix._23;
	m_planes[1].c = matrix._34 - matrix._33;
	m_planes[1].d = matrix._44 - matrix._43;
	D3DXPlaneNormalize(&m_planes[1], &m_planes[1]);
	
	// Calculate left plane of frustum.
	m_planes[2].a = matrix._14 + matrix._11;
	m_planes[2].b = matrix._24 + matrix._21;
	m_planes[2].c = matrix._34 + matrix._31;
	m_planes[2].d = matrix._44 + matrix._41;
	D3DXPlaneNormalize(&m_planes[2], &m_planes[2]);
	
	// Calculate right plane of frustum. 
	m_planes[3].a = matrix._14 - matrix._11;
	m_planes[3].b = matrix._24 - matrix._21;
	m_planes[3].c = matrix._34 - matrix._31; 
	m_planes[3].d = matrix._44 - matrix._41; 
	D3DXPlaneNormalize(&m_planes[3], &m_planes[3]); 
	
	// Calculate top plane of frustum.
	m_planes[4].a = matrix._14 - matrix._12; 
	m_planes[4].b = matrix._24 - matrix._22; 
	m_planes[4].c = matrix._34 - matrix._32;
	m_planes[4].d = matrix._44 - matrix._42; 
	D3DXPlaneNormalize(&m_planes[4], &m_planes[4]); 
	
	// Calculate bottom plane of frustum.
	m_planes[5].a = matrix._14 + matrix._12; 
	m_planes[5].b = matrix._24 + matrix._22; 
	m_planes[5].c = matrix._34 + matrix._32; 
	m_planes[5].d = matrix._44 + matrix._42;
	D3DXPlaneNormalize(&m_planes[5], &m_planes[5]); 
	
	return;
}

/*-------------------------------------------------------------------------------------------------------
이름: Check경계볼륨()
용도: 
- 각 도형들이 경계 볼륨 안에 있는지, 밖에 있는지, 교차하고 있는지 검사한다.
- 절두체의 한 면-경계볼륨간의 연산이므로 for문으로 6번 돌린다.
---------------------------------------------------------------------------------------------------------*/
bool FrustumClass::CheckPoint(float x, float y, float z)
{
	int i;
	
	// Check if the point is inside all six planes of the view frustum.
	for(i=0; i<6; i++) 
	{ 
		if(D3DXPlaneDotCoord(&m_planes[i], &D3DXVECTOR3(x, y, z)) < 0.0f)
			return false;
	}
	
	return true;
}

bool FrustumClass::CheckCube(float xCenter, float yCenter, float zCenter, float radius)
{
	int i;
	
	// Check if any one point of the cube is in the view frustum.
	for(i=0; i<6; i++)
	{ 
		if(D3DXPlaneDotCoord(&m_planes[i], &D3DXVECTOR3((xCenter - radius), (yCenter - radius), (zCenter - radius))) >= 0.0f)
		{ continue; }
		if(D3DXPlaneDotCoord(&m_planes[i], &D3DXVECTOR3((xCenter + radius), (yCenter - radius), (zCenter - radius))) >= 0.0f)
		{ continue; }
		if(D3DXPlaneDotCoord(&m_planes[i], &D3DXVECTOR3((xCenter - radius), (yCenter + radius), (zCenter - radius))) >= 0.0f)
		{ continue; } 
		if(D3DXPlaneDotCoord(&m_planes[i], &D3DXVECTOR3((xCenter + radius), (yCenter + radius), (zCenter - radius))) >= 0.0f)
		{ continue; }
		if(D3DXPlaneDotCoord(&m_planes[i], &D3DXVECTOR3((xCenter - radius), (yCenter - radius), (zCenter + radius))) >= 0.0f)
		{ continue; }
		if(D3DXPlaneDotCoord(&m_planes[i], &D3DXVECTOR3((xCenter + radius), (yCenter - radius), (zCenter + radius))) >= 0.0f)
		{ continue; }
		if(D3DXPlaneDotCoord(&m_planes[i], &D3DXVECTOR3((xCenter - radius), (yCenter + radius), (zCenter + radius))) >= 0.0f)
		{ continue; }
		if(D3DXPlaneDotCoord(&m_planes[i], &D3DXVECTOR3((xCenter + radius), (yCenter + radius), (zCenter + radius))) >= 0.0f)
		{ continue; }
		
		return false;
	} 
	
	return true;
}

// 구의 중심이 시야 절두체의 여섯 평면에 반경 이내만큼 존재하는지 확인한다.
bool FrustumClass::CheckSphere(float xCenter, float yCenter, float zCenter, float radius)
{
	int i;
	
	// Check if the radius of the sphere is inside the view frustum.
	for(i=0; i<6; i++) 
	{ 
		if(D3DXPlaneDotCoord(&m_planes[i], &D3DXVECTOR3(xCenter, yCenter, zCenter)) < -radius)
		{ 
			return false;
		}
	}
	
	return true;
}

bool FrustumClass::CheckRectangle(float xCenter, float yCenter, float zCenter, float xSize, float ySize, float zSize)
{
	int i;
	
	// Check if any of the 6 planes of the rectangle are inside the view frustum.
	for(i=0; i<6; i++)
	{ 
		if(D3DXPlaneDotCoord(&m_planes[i], &D3DXVECTOR3((xCenter - xSize), (yCenter - ySize), (zCenter - zSize))) >= 0.0f)
		{ continue; }
		if(D3DXPlaneDotCoord(&m_planes[i], &D3DXVECTOR3((xCenter + xSize), (yCenter - ySize), (zCenter - zSize))) >= 0.0f)
		{ continue; } 
		if(D3DXPlaneDotCoord(&m_planes[i], &D3DXVECTOR3((xCenter - xSize), (yCenter + ySize), (zCenter - zSize))) >= 0.0f)
		{ continue; } 
		if(D3DXPlaneDotCoord(&m_planes[i], &D3DXVECTOR3((xCenter - xSize), (yCenter - ySize), (zCenter + zSize))) >= 0.0f)
		{ continue; }
		if(D3DXPlaneDotCoord(&m_planes[i], &D3DXVECTOR3((xCenter + xSize), (yCenter + ySize), (zCenter - zSize))) >= 0.0f) 
		{ continue; } 
		if(D3DXPlaneDotCoord(&m_planes[i], &D3DXVECTOR3((xCenter + xSize), (yCenter - ySize), (zCenter + zSize))) >= 0.0f) 
		{ continue; } 
		if(D3DXPlaneDotCoord(&m_planes[i], &D3DXVECTOR3((xCenter - xSize), (yCenter + ySize), (zCenter + zSize))) >= 0.0f)
		{ continue; } 
		if(D3DXPlaneDotCoord(&m_planes[i], &D3DXVECTOR3((xCenter + xSize), (yCenter + ySize), (zCenter + zSize))) >= 0.0f)
		{ continue; }
		
		return false; 
	}
	
	return true;
}





