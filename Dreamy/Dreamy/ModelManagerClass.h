#pragma once
#ifndef _MODELMANAGERCLASS_H_ 
#define _MODELMANAGERCLASS_H_

#include <D3DX10math.h>
#include <stdlib.h>
#include <time.h>

/*
	장면의 모든 모델들에 대한 정보를 유지/관리하는 클래스.
	한 장면내의 다른 타입들의 모델들을 모두 관리할 수 있게 확장할 수 있다.

	여기다가 스마트 포인터를 쓸 수도 있지 않을까...?
*/

class ModelManagerClass
{
private:

	// 모델들의 색, 위치
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

