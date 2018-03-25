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
이름 : Initialize()
용도 :
- 글꼴 데이터와 글꼴 텍스처를 불러온다.
----------------------------------------------------------------------------------------------------------*/
bool FontClass::Initialize(ID3D11Device* device, char* fontFilename, WCHAR* textureFilename)
{
	
	bool result;

	// 폰트 데이터가 들어있는 텍스트 파일을 로드한다.
	result = LoadFontPositionTextFile(fontFilename);
	if(!result) { return false; }
	

	// 글꼴 텍스처를 불러온다.
	result = LoadTexture(device, textureFilename);
	if(!result) { return false; } 
	

	return true;

}


/*--------------------------------------------------------------------------------------------------------
이름 : LoadFontPositionTextFile()
용도 :
- 텍스처에서의 글자 위치 정보를 담고 있는 fontdata.txt파일을 불러온다.
- 파일을 열고 각 라인을 읽어 m_Font 배열에 저장한다.
- m_Font: 텍스처에 부여할 인덱스
----------------------------------------------------------------------------------------------------------*/
bool FontClass::LoadFontPositionTextFile(char* filename)
{
	ifstream fin;
	int i;
	char temp;

	// FontType구조체를 이용해 텍스처파일에 있는 95개 글자에 인덱스를 부여한다.
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
		//한 줄 끝나면 다음 줄
		fin.get(temp);

		while(temp != ' ')
		{ 
			fin.get(temp);
		} 

		// 텍스처의 U좌표인 LEFT, RIGHT, 문자의 픽셀 너비를 읽어들인다.
		fin >> m_Font[i].left;
		fin >> m_Font[i].right;
		fin >> m_Font[i].size;



	} 

	// Close the file. 
	fin.close();
	
	
	return true;
}


/*--------------------------------------------------------------------------------------------------------
이름 : LoadTexture()
용도 :
- 글꼴 텍스처 파일을 로드한다.
- 텍스처 파일을 읽어 텍스처 셰이더에 전달한다.

- 이 텍스처에서 글자를 뽑아 각각의 사각형에 입혀 화면에 그린다.
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
이름 : ReleaseFontData(), ReleaseTexture()
용도 :
- 읽어들인 인덱스 데이터를 해제한다.
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
이름 : ShutDown()
용도 :
- 글꼴 데이터와 글꼴 텍스처를 해제한다.
----------------------------------------------------------------------------------------------------------*/
void FontClass::ShutDown()
{
	ReleaseTexture();

	ReleaseFontData();

	return;
}

/*--------------------------------------------------------------------------------------------------------
이름 : GetTexture()
용도 :
- 글꼴이 실제로 그려질 수 있도록 글꼴 텍스처의 인터페이스를 반환한다.
----------------------------------------------------------------------------------------------------------*/
ID3D11ShaderResourceView* FontClass::GetTexture()
{
	return m_Texture->GetTexture();
}

/*--------------------------------------------------------------------------------------------------------
이름 : BuildVertexArray()
용도 :
- TextClass에 의해 호출되어 인자로 받은 문장으로 정점 배열을 생성한다.
- TextClass는 이 함수로 얻은 정점 배열로 매 프레임마다 새로운 정점 버퍼를 생성할 수 있다.

- vertics: 정점 배열을 가리키는 포인터, TextClass에게 이 함수에서 내용을 채운 배열을 전달한다.
- sentence: 정점 배열을 만드는데 필요한 문장(문자열)
- drawx, drawy : 문장이 그려질 화면상의 좌표

- 1. 문자당 두 개의 삼각형을 만든다. 
- 2. 두 삼각형에 실제로 그려질 글자에 해당하는 텍스처를 m_Font 배열의 U좌표와 픽셀 너비를 이용하여 매핑 시킨다.
- 3. 글자에 해당하는 도형이 생성되면 X좌표를 갱신하여 글자가 그려질 위치를 잡는다.
----------------------------------------------------------------------------------------------------------*/
void FontClass::BuildVertexArray(void* vertices, char* sentence, float drawX, float drawY)
{
	VertexType* vertexPtr;
	int numLetters, index, i, letter;


	// Coerce the input vertices into a VertexType structure.
	vertexPtr = (VertexType*)vertices;

	// 문장의 글자 갯수를 numLetters에 할당한다.
	numLetters = (int)strlen(sentence);

	// Initialize the index to the vertex array.
	index = 0;

	// 반복문을 돌면서 정점/인덱스 배열을 만든다.
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





















