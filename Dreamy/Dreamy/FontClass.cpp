#include "FontClass.h"


FontClass::FontClass()
{
	m_Font = 0;
	m_Texture = 0;
}

FontClass::FontClass(const FontClass& other)
{
}

FontClass::~FontClass()
{
}

/*--------------------------------------------------------------------------------------------------------
�̸� : Initialize()
�뵵 :
- �۲� �����Ϳ� �۲� �ؽ�ó�� �ҷ��´�.
----------------------------------------------------------------------------------------------------------*/
bool FontClass::Initialize(ID3D11Device* device, char* fontFilename, WCHAR* textureFilename)
{
	
	bool result;

	// ��Ʈ �����Ͱ� ����ִ� �ؽ�Ʈ ������ �ε��Ѵ�.
	result = LoadFontPositionTextFile(fontFilename);
	if(!result) { return false; }
	

	// �۲� �ؽ�ó�� �ҷ��´�.
	result = LoadTexture(device, textureFilename);
	if(!result) { return false; } 
	

	return true;

}


/*--------------------------------------------------------------------------------------------------------
�̸� : LoadFontPositionTextFile()
�뵵 :
- �ؽ�ó������ ���� ��ġ ������ ��� �ִ� fontdata.txt������ �ҷ��´�.
- ������ ���� �� ������ �о� m_Font �迭�� �����Ѵ�.
- m_Font: �ؽ�ó�� �ο��� �ε���
----------------------------------------------------------------------------------------------------------*/
bool FontClass::LoadFontPositionTextFile(char* filename)
{
	ifstream fin;
	int i;
	char temp;

	// FontType����ü�� �̿��� �ؽ�ó���Ͽ� �ִ� 95�� ���ڿ� �ε����� �ο��Ѵ�.
	m_Font = new FontType[95];
	
	if(!m_Font) { return false; }


	fin.open(filename);
	if(fin.fail()) { return false; }


	for(i=0; i<95; i++)
	{ 
		fin.get(temp); 

		while(temp != ' ')
		{ 
			fin.get(temp);
		}
		//�� �� ������ ���� ��
		fin.get(temp);

		while(temp != ' ')
		{ 
			fin.get(temp);
		} 

		// �ؽ�ó�� U��ǥ�� LEFT, RIGHT, ������ �ȼ� �ʺ� �о���δ�.
		fin >> m_Font[i].left;
		fin >> m_Font[i].right;
		fin >> m_Font[i].size;
	} 

	// Close the file. 
	fin.close();
	
	
	return true;
}


/*--------------------------------------------------------------------------------------------------------
�̸� : LoadTexture()
�뵵 :
- �۲� �ؽ�ó ������ �ε��Ѵ�.
- �ؽ�ó ������ �о� �ؽ�ó ���̴��� �����Ѵ�.

- �� �ؽ�ó���� ���ڸ� �̾� ������ �簢���� ���� ȭ�鿡 �׸���.
----------------------------------------------------------------------------------------------------------*/
bool FontClass::LoadTexture(ID3D11Device* device, WCHAR* filename)
{
	bool result;


	// Create the texture object.
	m_Texture = new TextureClass;
	if (!m_Texture) { return false;}

	// Initialize the texture object.
	result = m_Texture->Initialize(device, filename);
	if (!result) { return false; }

	return true;
}


/*--------------------------------------------------------------------------------------------------------
�̸� : ReleaseFontData(), ReleaseTexture()
�뵵 :
- �о���� �ε��� �����͸� �����Ѵ�.
----------------------------------------------------------------------------------------------------------*/
void FontClass::ReleaseFontData()
{ 
	if(m_Font) 
	{ 
		delete [] m_Font;
		m_Font = 0;
	}	
	return;
}

void FontClass::ReleaseTexture()
{
	// Release the texture object.
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}

	return;
}

/*--------------------------------------------------------------------------------------------------------
�̸� : ShutDown()
�뵵 :
- �۲� �����Ϳ� �۲� �ؽ�ó�� �����Ѵ�.
----------------------------------------------------------------------------------------------------------*/
void FontClass::ShutDown()
{
	ReleaseTexture();

	ReleaseFontData();

	return;
}

/*--------------------------------------------------------------------------------------------------------
�̸� : GetTexture()
�뵵 :
- �۲��� ������ �׷��� �� �ֵ��� �۲� �ؽ�ó�� �������̽��� ��ȯ�Ѵ�.
----------------------------------------------------------------------------------------------------------*/
ID3D11ShaderResourceView* FontClass::GetTexture()
{
	return m_Texture->GetTexture();
}

/*--------------------------------------------------------------------------------------------------------
�̸� : BuildVertexArray()
�뵵 :
- TextClass�� ���� ȣ��Ǿ� ���ڷ� ���� �������� ���� �迭�� �����Ѵ�.
- TextClass�� �� �Լ��� ���� ���� �迭�� �� �����Ӹ��� ���ο� ���� ���۸� ������ �� �ִ�.

- vertics: ���� �迭�� ����Ű�� ������, TextClass���� �� �Լ����� ������ ä�� �迭�� �����Ѵ�.
- sentence: ���� �迭�� ����µ� �ʿ��� ����(���ڿ�)
- drawx, drawy : ������ �׷��� ȭ����� ��ǥ

- 1. ���ڴ� �� ���� �ﰢ���� �����. 
- 2. �� �ﰢ���� ������ �׷��� ���ڿ� �ش��ϴ� �ؽ�ó�� m_Font �迭�� U��ǥ�� �ȼ� �ʺ� �̿��Ͽ� ���� ��Ų��.
- 3. ���ڿ� �ش��ϴ� ������ �����Ǹ� X��ǥ�� �����Ͽ� ���ڰ� �׷��� ��ġ�� ��´�.
----------------------------------------------------------------------------------------------------------*/
void FontClass::BuildVertexArray(void* vertices, char* sentence, float drawX, float drawY)
{
	VertexType* vertexPtr;
	int numLetters, index, i, letter;


	// Coerce the input vertices into a VertexType structure.
	vertexPtr = (VertexType*)vertices;

	// ������ ���� ������ numLetters�� �Ҵ��Ѵ�.
	numLetters = (int)strlen(sentence);

	// Initialize the index to the vertex array.
	index = 0;

	// �ݺ����� ���鼭 ����/�ε��� �迭�� �����.
	for (i = 0; i<numLetters; i++)
	{
		letter = ((int)sentence[i]) - 32;

		// If the letter is a space then just move over three pixels.
		if (letter == 0)
		{
			drawX = drawX + 3.0f;
		}
		else
		{
			// First triangle in quad.
			vertexPtr[index].position = D3DXVECTOR3(drawX, drawY, 0.0f);  // Top left.
			vertexPtr[index].texture = D3DXVECTOR2(m_Font[letter].left, 0.0f);
			index++;

			vertexPtr[index].position = D3DXVECTOR3((drawX + m_Font[letter].size), (drawY - 16), 0.0f);  // Bottom right.
			vertexPtr[index].texture = D3DXVECTOR2(m_Font[letter].right, 1.0f);
			index++;

			vertexPtr[index].position = D3DXVECTOR3(drawX, (drawY - 16), 0.0f);  // Bottom left.
			vertexPtr[index].texture = D3DXVECTOR2(m_Font[letter].left, 1.0f);
			index++;

			// Second triangle in quad.
			vertexPtr[index].position = D3DXVECTOR3(drawX, drawY, 0.0f);  // Top left.
			vertexPtr[index].texture = D3DXVECTOR2(m_Font[letter].left, 0.0f);
			index++;

			vertexPtr[index].position = D3DXVECTOR3(drawX + m_Font[letter].size, drawY, 0.0f);  // Top right.
			vertexPtr[index].texture = D3DXVECTOR2(m_Font[letter].right, 0.0f);
			index++;

			vertexPtr[index].position = D3DXVECTOR3((drawX + m_Font[letter].size), (drawY - 16), 0.0f);  // Bottom right.
			vertexPtr[index].texture = D3DXVECTOR2(m_Font[letter].right, 1.0f);
			index++;

			// Update the x location for drawing by the size of the letter and one pixel.
			drawX = drawX + m_Font[letter].size + 1.0f;
		}
	}

	return;
}





















