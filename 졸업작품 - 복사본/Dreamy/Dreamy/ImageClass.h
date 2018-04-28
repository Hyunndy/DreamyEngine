#pragma once


/*
	ImageClass
	- ȭ�鿡 �׸��µ� �ʿ��� �� 2D�̹����� ǥ���ϴµ� ���ȴ�.
	- ModelClass�� �����ϴ�!

	DX11���� 2D�������� �ϱ� ���� 3���� ����
	1. ���翵(3D�������X)
	2. ������������(���콺 ��ŷó�� �̹����� �̵��ϴ� ��찡 �ֱ� ������)
	3. Z���� ����(����&���ٽǰ��� ���� ���۸� 1�� �� ������ �Ѵ�)
*/
#include "TextureClass.h"
#include "MatrixClass.h"

class ImageClass : public MatrixClass
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

	bool Initialize( int, int, WCHAR*, int, int);
	void Shutdown();
	bool Render( int, int);
	int GetIndexCount();

	//**�ؽ��Ĵ� ���� �ؽ�����ü�� ���������ο� ������ �ʴ´�.
	//**���̴°� ResourceView���·� ���δ�!
	ID3D11ShaderResourceView* GetTexture();


private:
	bool InitializeBuffers();
	void ShutdownBuffers();
	bool UpdateBuffers( int, int);
	void RenderBuffers();
	bool LoadTexture( WCHAR*);
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

public:
	bool active = false;

};
