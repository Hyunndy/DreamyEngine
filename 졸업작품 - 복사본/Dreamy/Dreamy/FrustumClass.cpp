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
void FrustumClass::ConstructFrustum(float m_screenDepth, D3DXMATRIX projectionMatrix, D3DXMATRIX viewMatrix)
{
	XMFLOAT4X4 pMatrix, matrix;
	float zMinimum, r, length;
	XMMATRIX finalMatrix;


	// Convert the projection matrix into a 4x4 float type.
	XMStoreFloat4x4(&pMatrix, (XMMATRIX)projectionMatrix);

	// Calculate the minimum Z distance in the frustum.
	zMinimum = -pMatrix._43 / pMatrix._33;
	r = m_screenDepth / (m_screenDepth - zMinimum);

	// Load the updated values back into the projection matrix.
	pMatrix._33 = r;
	pMatrix._43 = -r * zMinimum;
	(XMMATRIX)projectionMatrix = XMLoadFloat4x4(&pMatrix);

	// Create the frustum matrix from the view matrix and updated projection matrix.
	finalMatrix = XMMatrixMultiply((XMMATRIX)viewMatrix, (XMMATRIX)projectionMatrix);

	// Convert the final matrix into a 4x4 float type.
	XMStoreFloat4x4(&matrix, finalMatrix);

	// Calculate near plane of frustum.
	m_planes[0][0] = matrix._14 + matrix._13;
	m_planes[0][1] = matrix._24 + matrix._23;
	m_planes[0][2] = matrix._34 + matrix._33;
	m_planes[0][3] = matrix._44 + matrix._43;

	// Normalize the near plane.
	length = sqrtf((m_planes[0][0] * m_planes[0][0]) + (m_planes[0][1] * m_planes[0][1]) + (m_planes[0][2] * m_planes[0][2]));
	m_planes[0][0] /= length;
	m_planes[0][1] /= length;
	m_planes[0][2] /= length;
	m_planes[0][3] /= length;

	// Calculate far plane of frustum.
	m_planes[1][0] = matrix._14 - matrix._13;
	m_planes[1][1] = matrix._24 - matrix._23;
	m_planes[1][2] = matrix._34 - matrix._33;
	m_planes[1][3] = matrix._44 - matrix._43;

	// Normalize the far plane.
	length = sqrtf((m_planes[1][0] * m_planes[1][0]) + (m_planes[1][1] * m_planes[1][1]) + (m_planes[1][2] * m_planes[1][2]));
	m_planes[1][0] /= length;
	m_planes[1][1] /= length;
	m_planes[1][2] /= length;
	m_planes[1][3] /= length;

	// Calculate left plane of frustum.
	m_planes[2][0] = matrix._14 + matrix._11;
	m_planes[2][1] = matrix._24 + matrix._21;
	m_planes[2][2] = matrix._34 + matrix._31;
	m_planes[2][3] = matrix._44 + matrix._41;

	// Normalize the left plane.
	length = sqrtf((m_planes[2][0] * m_planes[2][0]) + (m_planes[2][1] * m_planes[2][1]) + (m_planes[2][2] * m_planes[2][2]));
	m_planes[2][0] /= length;
	m_planes[2][1] /= length;
	m_planes[2][2] /= length;
	m_planes[2][3] /= length;

	// Calculate right plane of frustum.
	m_planes[3][0] = matrix._14 - matrix._11;
	m_planes[3][1] = matrix._24 - matrix._21;
	m_planes[3][2] = matrix._34 - matrix._31;
	m_planes[3][3] = matrix._44 - matrix._41;

	// Normalize the right plane.
	length = sqrtf((m_planes[3][0] * m_planes[3][0]) + (m_planes[3][1] * m_planes[3][1]) + (m_planes[3][2] * m_planes[3][2]));
	m_planes[3][0] /= length;
	m_planes[3][1] /= length;
	m_planes[3][2] /= length;
	m_planes[3][3] /= length;

	// Calculate top plane of frustum.
	m_planes[4][0] = matrix._14 - matrix._12;
	m_planes[4][1] = matrix._24 - matrix._22;
	m_planes[4][2] = matrix._34 - matrix._32;
	m_planes[4][3] = matrix._44 - matrix._42;

	// Normalize the top plane.
	length = sqrtf((m_planes[4][0] * m_planes[4][0]) + (m_planes[4][1] * m_planes[4][1]) + (m_planes[4][2] * m_planes[4][2]));
	m_planes[4][0] /= length;
	m_planes[4][1] /= length;
	m_planes[4][2] /= length;
	m_planes[4][3] /= length;

	// Calculate bottom plane of frustum.
	m_planes[5][0] = matrix._14 + matrix._12;
	m_planes[5][1] = matrix._24 + matrix._22;
	m_planes[5][2] = matrix._34 + matrix._32;
	m_planes[5][3] = matrix._44 + matrix._42;

	// Normalize the bottom plane.
	length = sqrtf((m_planes[5][0] * m_planes[5][0]) + (m_planes[5][1] * m_planes[5][1]) + (m_planes[5][2] * m_planes[5][2]));
	m_planes[5][0] /= length;
	m_planes[5][1] /= length;
	m_planes[5][2] /= length;
	m_planes[5][3] /= length;

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
	for (i = 0; i<6; i++)
	{
		if (D3DXPlaneDotCoord(&m_planes[i], &D3DXVECTOR3((xCenter - radius), (yCenter - radius), (zCenter - radius))) >= 0.0f)
		{
			continue;
		}

		if (D3DXPlaneDotCoord(&m_planes[i], &D3DXVECTOR3((xCenter + radius), (yCenter - radius), (zCenter - radius))) >= 0.0f)
		{
			continue;
		}

		if (D3DXPlaneDotCoord(&m_planes[i], &D3DXVECTOR3((xCenter - radius), (yCenter + radius), (zCenter - radius))) >= 0.0f)
		{
			continue;
		}

		if (D3DXPlaneDotCoord(&m_planes[i], &D3DXVECTOR3((xCenter + radius), (yCenter + radius), (zCenter - radius))) >= 0.0f)
		{
			continue;
		}

		if (D3DXPlaneDotCoord(&m_planes[i], &D3DXVECTOR3((xCenter - radius), (yCenter - radius), (zCenter + radius))) >= 0.0f)
		{
			continue;
		}

		if (D3DXPlaneDotCoord(&m_planes[i], &D3DXVECTOR3((xCenter + radius), (yCenter - radius), (zCenter + radius))) >= 0.0f)
		{
			continue;
		}

		if (D3DXPlaneDotCoord(&m_planes[i], &D3DXVECTOR3((xCenter - radius), (yCenter + radius), (zCenter + radius))) >= 0.0f)
		{
			continue;
		}

		if (D3DXPlaneDotCoord(&m_planes[i], &D3DXVECTOR3((xCenter + radius), (yCenter + radius), (zCenter + radius))) >= 0.0f)
		{
			continue;
		}

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

//CheckRectangle()과 같으나 이건 센터 정점, width가 아닌 최대 최소 면적을 사용한다.
//여섯개의 절두체 평면과 사각형의 여섯면의 내적을 수행한다.
//사각형의 어떤 부분이 절두체에 있다고 판단되면 true륿 ㅏㄴ환한다.

bool FrustumClass::CheckRectangle2(float maxWidth, float maxHeight, float maxDepth, float minWidth, float minHeight, float minDepth)
{
	int i;
	float dotProduct;


	// Check if any of the 6 planes of the rectangle are inside the view frustum.
	for (i = 0; i<6; i++)
	{
		dotProduct = ((m_planes[i][0] * minWidth) + (m_planes[i][1] * minHeight) + (m_planes[i][2] * minDepth) + (m_planes[i][3] * 1.0f));
		if (dotProduct >= 0.0f)
		{
			continue;
		}

		dotProduct = ((m_planes[i][0] * maxWidth) + (m_planes[i][1] * minHeight) + (m_planes[i][2] * minDepth) + (m_planes[i][3] * 1.0f));
		if (dotProduct >= 0.0f)
		{
			continue;
		}

		dotProduct = ((m_planes[i][0] * minWidth) + (m_planes[i][1] * maxHeight) + (m_planes[i][2] * minDepth) + (m_planes[i][3] * 1.0f));
		if (dotProduct >= 0.0f)
		{
			continue;
		}

		dotProduct = ((m_planes[i][0] * maxWidth) + (m_planes[i][1] * maxHeight) + (m_planes[i][2] * minDepth) + (m_planes[i][3] * 1.0f));
		if (dotProduct >= 0.0f)
		{
			continue;
		}

		dotProduct = ((m_planes[i][0] * minWidth) + (m_planes[i][1] * minHeight) + (m_planes[i][2] * maxDepth) + (m_planes[i][3] * 1.0f));
		if (dotProduct >= 0.0f)
		{
			continue;
		}

		dotProduct = ((m_planes[i][0] * maxWidth) + (m_planes[i][1] * minHeight) + (m_planes[i][2] * maxDepth) + (m_planes[i][3] * 1.0f));
		if (dotProduct >= 0.0f)
		{
			continue;
		}

		dotProduct = ((m_planes[i][0] * minWidth) + (m_planes[i][1] * maxHeight) + (m_planes[i][2] * maxDepth) + (m_planes[i][3] * 1.0f));
		if (dotProduct >= 0.0f)
		{
			continue;
		}

		dotProduct = ((m_planes[i][0] * maxWidth) + (m_planes[i][1] * maxHeight) + (m_planes[i][2] * maxDepth) + (m_planes[i][3] * 1.0f));
		if (dotProduct >= 0.0f)
		{
			continue;
		}

		return false;
	}

	return true;
}







