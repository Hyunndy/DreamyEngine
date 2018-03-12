#pragma once
#ifndef _RTTTEXTURECLASS_H_
#define _RTTTEXTURECLASS_H_

#include "stdafx.h"

/*
	RTT텍스처를 그린다.

*/

class RTTTextureClass
{
private:

	// 3D객체에 Z버퍼를 끄고 그려지는것이기 때문에 UV좌표를 뺀 좌표를 그려줘야 한다.
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
	};
public:
	RTTTextureClass();
	RTTTextureClass(const RTTTextureClass&);
	~RTTTextureClass();

	bool Initialize(ID3D11Device*, int, int, int, int);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, int);
	int GetIndexCount();

	//**텍스쳐는 절대 텍스쳐자체로 파이프라인에 묶이지 않는다.
	//**묶이는건 ResourceView형태로 묶인다!
	ID3D11ShaderResourceView* GetTexture();

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	bool UpdateBuffers(ID3D11DeviceContext*, int, int);
	void RenderBuffers(ID3D11DeviceContext*);


private:

	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;


	/*
	3D모델과는 달리 2D이미지는 화면 크기, 이미지 크기, 이전에 그려졌던 위치를 기억해야 한다.

	이전에 그려졌던 위치를 알아야 하는 이유는
	만약 이전 프레임과 비교해서 위치가 변하지 않았다면 동적 정점 버퍼를 바꾸지 않아도 되기 때문에
	성능의 향상을 꾀할 수 있기 때문이다!
	*/
	int m_screenWidth, m_screenHeight;
	int m_bitmapWidth, m_bitmapHeight;
	int m_previousPosX, m_previousPosY;



};
#endif