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
�̸�: Initialize()
�뵵:
- �ռ� �����ߴ� ModelInfoType���� ������ �𵨵��� ���ڸ� ���Ѵ�.
- ���Ƿ� ��ġ�� ���ؼ� �� �迭�� �����Ѵ�.
  (���� ���� �������� seed�� ���� �ð����� �ش�)
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
�̸�: GetData()
�뵵:
- �־��� �ε����� ModelTypeInfo�� �̿��� ������ ������ ��ġ�� ������ �����´�.
- �Ƹ� �̰� Initialzie�Ѵ����� GetData�� Render���� �ϳ���..?
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
�̸�: GetModelCount()
�뵵:
- Render()���� for�� ���� �� �ʿ���...����
---------------------------------------------------------------------------------------------------------*/
int ModelManagerClass::GetModelCount()
{
	return m_modelCount;
}

void ModelManagerClass::Shutdown()
{
	if(m_ModelInfoList) { delete [] m_ModelInfoList; m_ModelInfoList = 0; }
}