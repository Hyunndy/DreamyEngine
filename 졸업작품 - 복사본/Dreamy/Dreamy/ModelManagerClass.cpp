#include "ModelManagerClass.h"



ModelManagerClass::ModelManagerClass()
{
	m_ModelInfoList = 0;
}

ModelManagerClass::ModelManagerClass(const ModelManagerClass& other)
{
}

ModelManagerClass::~ModelManagerClass()
{
}
/*-------------------------------------------------------------------------------------------------------
이름: Initialize()
용도:
- 앞서 정의했던 ModelInfoType으로 제어할 모델들의 숫자를 정한다.
- 임의로 위치를 정해서 모델 배열에 저장한다.
  (랜덤 숫자 생성기의 seed를 현재 시간으로 준다)
---------------------------------------------------------------------------------------------------------*/
bool ModelManagerClass::Initialize(int numModels)
{
	float red, green, blue;

	m_modelCount = numModels;

	m_ModelInfoList = new ModelInfoType[m_modelCount];
	if (!m_ModelInfoList) { return false; }

	// Seed the random generator with the current time.
	srand((unsigned int)time(NULL));

	for (int i = 0; i < m_modelCount; i++)
	{
		// Generate a random color for the model.
		red = (float)rand() / RAND_MAX;
		green = (float)rand() / RAND_MAX;
		blue = (float)rand() / RAND_MAX; 

		m_ModelInfoList[i].color = D3DXVECTOR4(red, green, blue, 1.0f);

		// Generate a random position in front of the viewer for the mode.
		m_ModelInfoList[i].positionX = (((float)rand()-(float)rand())/RAND_MAX) * 10.0f;
		m_ModelInfoList[i].positionY = (((float)rand()-(float)rand())/RAND_MAX) * 10.0f;
		m_ModelInfoList[i].positionZ = ((((float)rand()-(float)rand())/RAND_MAX) * 10.0f) + 5.0f;
	}

	return true;
}

/*-------------------------------------------------------------------------------------------------------
이름: GetData()
용도:
- 주어진 인덱스로 ModelTypeInfo를 이용해 제어할 도형의 위치와 색상을 가져온다.
- 아마 이걸 Initialzie한다음에 GetData를 Render에서 하나봄..?
---------------------------------------------------------------------------------------------------------*/
void ModelManagerClass::GetData(int index, float& positionX, float& positionY, float& positionZ, D3DXVECTOR4& color)
{
	positionX = m_ModelInfoList[index].positionX;
	positionY = m_ModelInfoList[index].positionY;
	positionZ = m_ModelInfoList[index].positionZ;
	
	color = m_ModelInfoList[index].color; 
	
	return;
}

/*-------------------------------------------------------------------------------------------------------
이름: GetModelCount()
용도:
- Render()에서 for문 돌릴 때 필요한...아이
---------------------------------------------------------------------------------------------------------*/
int ModelManagerClass::GetModelCount()
{
	return m_modelCount;
}

void ModelManagerClass::Shutdown()
{
	if(m_ModelInfoList) { delete [] m_ModelInfoList; m_ModelInfoList = 0; }
}