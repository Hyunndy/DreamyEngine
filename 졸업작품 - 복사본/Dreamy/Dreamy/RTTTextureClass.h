////////////////////////////////////////////////////////////////////////////////
// Filename: RTTTextureClass.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dx10math.h>


////////////////////////////////////////////////////////////////////////////////
// Class name: RTTTextureClass
////////////////////////////////////////////////////////////////////////////////
class RTTTextureClass
{
public:
	RTTTextureClass();
	RTTTextureClass(const RTTTextureClass&);
	~RTTTextureClass();

	bool Initialize(ID3D11Device*, int, int, float, float);
	void Shutdown();

	void SetRenderTarget(ID3D11DeviceContext*);
	void ClearRenderTarget(ID3D11DeviceContext*, float, float, float, float);
	ID3D11ShaderResourceView* GetShaderResourceView();
	void GetProjectionMatrix(D3DXMATRIX&);
	void GetOrthoMatrix(D3DXMATRIX&);

private:
	ID3D11Texture2D* m_renderTargetTexture;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11ShaderResourceView* m_shaderResourceView;
	ID3D11Texture2D* m_depthStencilBuffer;
	ID3D11DepthStencilView* m_depthStencilView;
	D3D11_VIEWPORT m_viewport;
	D3DXMATRIX m_projectionMatrix;
	D3DXMATRIX m_orthoMatrix;

};



//#pragma once
//#ifndef _RTTTEXTURECLASS_H_
//#define _RTTTEXTURECLASS_H_
//
//#include "stdafx.h"
//
///*
//	RTT�ؽ�ó�� �׸���.
//
//*/
//
//class RTTTextureClass
//{
//private:
//
//	// 3D��ü�� Z���۸� ���� �׷����°��̱� ������ UV��ǥ�� �� ��ǥ�� �׷���� �Ѵ�.
//	struct VertexType
//	{
//		D3DXVECTOR3 position;
//		D3DXVECTOR2 texture;
//	};
//public:
//	RTTTextureClass();
//	RTTTextureClass(const RTTTextureClass&);
//	~RTTTextureClass();
//
//	bool Initialize(ID3D11Device*, int, int, int, int);
//	void Shutdown();
//	bool Render(ID3D11DeviceContext*, int, int);
//	int GetIndexCount();
//
//	//**�ؽ��Ĵ� ���� �ؽ�����ü�� ���������ο� ������ �ʴ´�.
//	//**���̴°� ResourceView���·� ���δ�!
//	ID3D11ShaderResourceView* GetShaderResourceView();
//
//private:
//	bool InitializeBuffers(ID3D11Device*);
//	void ShutdownBuffers();
//	bool UpdateBuffers(ID3D11DeviceContext*, int, int);
//	void RenderBuffers(ID3D11DeviceContext*);
//
//
//private:
//
//	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
//	int m_vertexCount, m_indexCount;
//
//
//	/*
//	3D�𵨰��� �޸� 2D�̹����� ȭ�� ũ��, �̹��� ũ��, ������ �׷����� ��ġ�� ����ؾ� �Ѵ�.
//
//	������ �׷����� ��ġ�� �˾ƾ� �ϴ� ������
//	���� ���� �����Ӱ� ���ؼ� ��ġ�� ������ �ʾҴٸ� ���� ���� ���۸� �ٲ��� �ʾƵ� �Ǳ� ������
//	������ ����� ���� �� �ֱ� �����̴�!
//	*/
//	int m_screenWidth, m_screenHeight;
//	int m_bitmapWidth, m_bitmapHeight;
//	int m_previousPosX, m_previousPosY;
//
//
//
//};
//#endif