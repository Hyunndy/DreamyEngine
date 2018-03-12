#pragma once
#ifndef _IMAGECLASS_H_
#define _IMAGECLASS_H_

#include "stdafx.h"
#include "TextureClass.h"
/*
	ImageClass
	- ȭ�鿡 �׸��µ� �ʿ��� �� 2D�̹����� ǥ���ϴµ� ���ȴ�.
	- ModelClass�� �����ϴ�!

	DX11���� 2D�������� �ϱ� ���� 3���� ����
	1. ���翵(3D�������X)
	2. ������������(���콺 ��ŷó�� �̹����� �̵��ϴ� ��찡 �ֱ� ������)
	3. Z���� ����(����&���ٽǰ��� ���� ���۸� 1�� �� ������ �Ѵ�)
*/

class ImageClass
{
private:

	// 3D��ü�� Z���۸� ���� �׷����°��̱� ������ UV��ǥ�� �� ��ǥ�� �׷���� �Ѵ�.
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

	//**�ؽ��Ĵ� ���� �ؽ�����ü�� ���������ο� ������ �ʴ´�.
	//**���̴°� ResourceView���·� ���δ�!
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
		3D�𵨰��� �޸� 2D�̹����� ȭ�� ũ��, �̹��� ũ��, ������ �׷����� ��ġ�� ����ؾ� �Ѵ�.
		
		������ �׷����� ��ġ�� �˾ƾ� �ϴ� ������
		���� ���� �����Ӱ� ���ؼ� ��ġ�� ������ �ʾҴٸ� ���� ���� ���۸� �ٲ��� �ʾƵ� �Ǳ� ������
		������ ����� ���� �� �ֱ� �����̴�!
		*/
		int m_screenWidth, m_screenHeight;
		int m_bitmapWidth, m_bitmapHeight;
		int m_previousPosX, m_previousPosY;



};
#endif