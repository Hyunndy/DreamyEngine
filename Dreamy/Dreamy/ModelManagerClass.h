#pragma once
#ifndef _MODELMANAGERCLASS_H_ 
#define _MODELMANAGERCLASS_H_

#include <D3DX10math.h>
#include <stdlib.h>
#include <time.h>

/*
	����� ��� �𵨵鿡 ���� ������ ����/�����ϴ� Ŭ����.
	�� ��鳻�� �ٸ� Ÿ�Ե��� �𵨵��� ��� ������ �� �ְ� Ȯ���� �� �ִ�.

	����ٰ� ����Ʈ �����͸� �� ���� ���� ������...?
*/

class ModelManagerClass
{
private:

	// �𵨵��� ��, ��ġ
	struct ModelInfoType
	{
		D3DXVECTOR4 color;
		float positionX, positionY, positionZ;
	};

public:
	ModelManagerClass();
	ModelManagerClass(const ModelManagerClass&);
	~ModelManagerClass();

	
	bool Initialize(int);
	void Shutdown();
	int  GetModelCount(); 
	void GetData(int, float&, float&, float&, D3DXVECTOR4&);

private:
	int m_modelCount;
	ModelInfoType* m_ModelInfoList;

};
#endif 

