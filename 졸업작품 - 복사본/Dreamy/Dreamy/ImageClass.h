#pragma once
#ifndef _IMAGECLASS_H_
#define _IMAGECLASS_H_

#include "stdafx.h"
#include "TextureClass.h"
/*
	ImageClass
	- 화면에 그리는데 필요한 각 2D이미지를 표현하는데 사용된다.
	- ModelClass랑 유사하다!

	DX11에서 2D렌더링을 하기 위한 3가지 컨셉
	1. 정사영(3D투영행렬X)
	2. 동적정점버퍼(마우스 피킹처럼 이미지가 이동하는 경우가 있기 때문에)
	3. Z버퍼 끄기(깊이&스텐실값이 같은 버퍼를 1개 더 만들어야 한다)
*/

class ImageClass
{
private:

	// 3D객체에 Z버퍼를 끄고 그려지는것이기 때문에 UV좌표를 뺀 좌표를 그려줘야 한다.
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
	};
public:
	ImageClass();
	ImageClass(const ImageClass&);
	~ImageClass();

	bool Initialize(ID3D11Device*,  int, int, WCHAR*, int, int);
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
		bool LoadTexture(ID3D11Device*, WCHAR*);
		void ReleaseTexture();

	private:

		ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
		int m_vertexCount, m_indexCount;
		TextureClass* m_Texture;

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