#include "TerrainClass.h"

/*---------------------------------------------------------------------------------------------
이름:
용도:
-----------------------------------------------------------------------------------------------*/

TerrainClass::TerrainClass()
{
	m_terrainWidth = 0;
	m_terrainHeight = 0;
	m_vertexCount = 0;
	m_indexCount = 0;
	m_heightScale = 0;

	m_terrainFilename = 0;
	m_colorMapFilename = 0;

	m_vertexBuffer = 0;
	m_indexBuffer = 0;

	m_heightMap = 0;
	m_terrainModel = 0;

	m_ColorTexture = 0;
	m_NormalMapTexture = 0;

	m_TerrainCells = 0;

}

TerrainClass::TerrainClass(const TerrainClass& other)
{
}

TerrainClass::~TerrainClass()
{
}
/*---------------------------------------------------------------------------------------------
이름: Initialize()
용도: 
- 셋업 파일,높이맵 파일 로드 후 정규화, 조명에 대한 공유 법선 계산
-> 컬러맵 로드, 지형 모델 생성, tangent,binormal 벡터 계산
-> 전체 지형에 대한 정점, 인덱스 버퍼를 만드는 대신 지형 모델을 Cell에 로드 한다. (컬링을 위해)
-> 텍스처로딩 -> 모델 데이터 해제
-----------------------------------------------------------------------------------------------*/
bool TerrainClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* SetupFilename, CHAR* ColorTexture, CHAR* NormalMapTexture)
{
	bool result;

	//Terrain filename, height, width, scailing, colormapfilename을 얻는다.
	result = LoadSetupFile(SetupFilename);

	// 지형의 높이맵을 로드
	//result = LoadHeightMap_Bitmap(heightMapFilename);
	result = LoadHeightMap_Raw();
	if (!result) { return false; }

	//높이 스케일에 대한 X,Z좌표를 설정하고 높이 스케일 값에 따라 지형 높이를 조정한다.
	SetTerrainCoordinates();

	// 높이 맵을 로드하고 높이를 줄인 다음 이 함수를 호출하여 조명에 대한 공유 법선을 계산한다.
	// 지형 데이터의 법선을 계산한다.
	result = CalculateNormal();
	if (!result) { return false; }
	
	// Calculate the texture coordinates.
	//CalculateTextureCoordinates();

	//지형의 컬러맵을 로드한다.
	result = LoadColorMap();
	if (!result) { return false; }

	//지형의 3D모델을 작성한다.
	result = BuildTerrainModel();
	if (!result) { return false; }


	// 지형 3d모델이 만들어지면 높이 맵이 필요없어지니까 릴리즈한다.
	ShutdownHeightMap();

	// 지형 모델에 대한 탄젠트 및 binormal을 계산한다.
	CalculateTerrainVectors();

	//result = InitializeBuffer(device);
	//if (!result) { return false; }

	// 지형 데이터로 셀을 만들고 로드한다.
	result = LoadTerrainCells(device);
	if (!result) { return false; }

	//렌더링 버퍼가 로드되었으므로 지형 모델을 해제한다.
	ShutdownTerrainModel();

	//텍스처 로드
	result = LoadTexture(device, deviceContext, ColorTexture, NormalMapTexture);
	if (!result) { return true; }

	return true;
}

bool TerrainClass::Initializeforwater(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* SetupFilename, CHAR* ColorTexture, CHAR* NormalMapTexture)
{
	bool result;


	// Get the terrain filename, dimensions, and so forth from the setup file.
	result = LoadSetupFile(SetupFilename);
	if (!result)
	{
		return false;
	}
	//We now call the new LoadRawHeightMap function to load in RAW height maps.The previous LoadBitmapHeightMap is no longer utilized.

		// Initialize the terrain height map with the data from the raw file.
	result = LoadHeightMap_Raw();
	if (!result)
	{
		return false;
	}

	// Setup the X and Z coordinates for the height map as well as scale the terrain height by the height scale value.
	SetTerrainCoordinates();

	// Calculate the normals for the terrain data.
	result = CalculateNormal();
	if (!result)
	{
		return false;
	}

	// Load in the color map for the terrain.
	result = LoadColorMap();
	if (!result)
	{
		return false;
	}

	// Now build the 3D model of the terrain.
	result = BuildTerrainModel();
	if (!result)
	{
		return false;
	}

	// We can now release the height map since it is no longer needed in memory once the 3D terrain model has been built.
	ShutdownHeightMap();

	// Calculate the tangent and binormal for the terrain model.
	CalculateTerrainVectors();

	// Load the rendering buffers with the terrain data.
	result = InitializeBufferforwater(device);
	if (!result)
	{
		return false;
	}

	// Release the terrain model now that the rendering buffers have been loaded.
	ShutdownTerrainModel();

	//텍스처 로드
	result = LoadTexture(device, deviceContext, ColorTexture, NormalMapTexture);
	if (!result) { return true; }


	return true;
}
bool TerrainClass::Renderforwater(ID3D11DeviceContext* deviceContext)
{
	//렌더링 파이프 라인에 꼭지점, 인덱스 버퍼를 놓아 렌더링을 준비한다.
	RenderBuffersforwater(deviceContext);

	return true;
}

void TerrainClass::RenderBuffersforwater(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

int TerrainClass::GetIndexCount()
{
	return m_indexCount;
}

/*---------------------------------------------------------------------------------------------
이름: LoadSetupFile
용도: 셋업파일을 로드해 Terrain 이름, color map이름, 높이, 너비, 스케일값을 얻어낸다.
-----------------------------------------------------------------------------------------------*/
bool TerrainClass::LoadSetupFile(char* filename)
{
	int stringLength;
	ifstream fin;
	char input;

	// Initialize the strings that will hold the terrain file name and the color map file name.
	stringLength = 256;

	m_terrainFilename = new char[stringLength];
	if (!m_terrainFilename)
	{
		return false;
	}

	m_colorMapFilename = new char[stringLength];
	if (!m_colorMapFilename)
	{
		return false;
	}

	// Open the setup file.  If it could not open the file then exit.
	fin.open(filename);
	if (fin.fail())
	{
		return false;
	}

	// Read up to the terrain file name.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	// Read in the terrain file name.
	fin >> m_terrainFilename;

	// Read up to the value of terrain height.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	// Read in the terrain height.
	fin >> m_terrainHeight;

	// Read up to the value of terrain width.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	// Read in the terrain width.
	fin >> m_terrainWidth;

	// Read up to the value of terrain height scaling.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	// Read in the terrain height scaling.
	fin >> m_heightScale;

	// Read up to the color map file name.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	// Read in the color map file name.
	fin >> m_colorMapFilename;

	// Close the setup file.
	fin.close();

	return true;


}
/*---------------------------------------------------------------------------------------------
이름: LoadHeightMap
용도: 
- fopen(주소, 모드) : 파일 열기
- fread(저장할 변수, 사이즈, 반복 횟수, 읽을 파일 포인터) : 파일을 읽어서 출력한다.

1) 비트맵형식 높이맵
- 높이 맵을 포함하는 비트 맵 파일을 새 높이 맵 배열로 로드하는 새로운 함수.
- 비트맵에는 빨강, 녹색, 파랑색이 있다. 그러나 이것은 회색 이미지이므로 빨강, 초록, 파랑색을 모두 읽을 수 있다.

2) RAW형식 높이맵
- 16비트 RAW높이맵 파일을 로드한다.
- 16비트 형식이기 때문에 unsigned char 대신 unsigned short를 사용하여 데이터를 읽을 배열을 만든다.
- 배열을 파싱하여 높이 맵 구조에 데이터를 복사할 때 역순으로 tranverse하지 않아도 된다.
-----------------------------------------------------------------------------------------------*/
bool TerrainClass::LoadHeightMap_Bitmap(const char* filename)
{
	FILE *filePtr;
	int error;
	unsigned int count;

	BITMAPFILEHEADER bitmapFileHeader;
	BITMAPINFOHEADER bitmapInfoHeader;
	int imageSize, index;

	unsigned char* bitmapImage;
	unsigned char height;

	// 바이너리 모드로 높이맵 파일을 연다.
	// *바이너리 모드: 텍스트 모드와의 차이는 텍스트 모드는 운영체제에 따라 '\r'을 붙인다는 것, 이진파일을 여는 모드
	if(fopen_s(&filePtr, filename, "rb") != 0)
	{
		return false;
	}

	// 파일 헤더를 읽는다.
	if(fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr) != 1)
	{
		return false;
	}

	// 비트맵 정보 헤더를 읽는다.
	if(fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr) !=1)
	{
		return false;
	}

	// 지형 크기를 저장한다.
	// 지형을 렌더링할 뿐만 아니라 꼭지점과 인덱스 버퍼를 만드는데 이 값을 사용할 수 있다.
	m_terrainWidth = bitmapInfoHeader.biWidth;
	m_terrainHeight = bitmapInfoHeader.biHeight;



	//비트맵 이미지 데이터의 크기를 계산한다.
	imageSize = m_terrainWidth * m_terrainHeight * 3;

	//비트맵 이미지 데이터에 메모리를 할당한다.
	bitmapImage = new unsigned char[imageSize];
	if (!bitmapImage) { return false; }

	//비트맵 데이터의 시작 부분으로 이동한다.
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// 비트맵 이미지 데이터를 읽는다.
	if(fread(bitmapImage, 1, imageSize, filePtr) != imageSize)
	{
		return false;
	}

	// 파일을 닫는다.
	if(fclose(filePtr) !=0)
	{
		return false;
	}


	// 이제 비트맵을 읽어 들여 2차원 높이 맵 배열을 만들고 버퍼를 읽어들인다.
	// for루프 동안 두 개의 루프 변수를 지형의 x(너비)와 z(깊이)로 사용한다.
	// 그리고 나서 비트맵 값을 지형의 Y(높이)로 사용한다.
	// 그레이 스케일 값으로 사용되는 색상 값(빨,녹,파)중 하나만 필요하기 때문에 인덱스를 비트 맵(k)에 3씩 증가시킨다.

	// 높이맵 데이터를 저장할 구조체를 생성한다.
	m_heightMap = new HeightMapType[m_terrainHeight*m_terrainWidth];
	if (!m_heightMap) { return false; }

	// 이미지 데이터 버퍼의 위치를 초기화 한다.
	int k = 0;

	// 이미지 데이터를 높이 맵으로 읽어들인다.
	for (int j = 0; j < m_terrainHeight; j++)
	{
		for (int i = 0; i < m_terrainWidth; i++)
		{
			height = bitmapImage[k];
			
			index = (m_terrainHeight * j) + i;
			
			m_heightMap[index].x = (float)i;
			m_heightMap[index].y = (float)height;
			m_heightMap[index].z = (float)j;

			k += 3;
		}
	}

	delete[] bitmapImage;
	bitmapImage = 0;

	return true;


}

bool TerrainClass::LoadHeightMap_Raw()
{
	int error, index;
	FILE* filePtr;
	unsigned long long imageSize, count;
	unsigned short* rawImage;

	//Setup파일에서 얻어낸 너비, 높이값으로 높이맵 데이터 배열을 생성한다.
	m_heightMap = new HeightMapType[m_terrainWidth * m_terrainHeight];
	if (!m_heightMap) { return false; }

	//이진수로 읽기 위해 16비트 raw 높이맵을 연다.
	error = fopen_s(&filePtr, m_terrainFilename, "rb");
	if (error != 0) { return false; }

	//raw 이미지 데이터의 사이즈를 계산한다.
	imageSize = m_terrainHeight*m_terrainWidth;

	//raw 이미지 데이터에 메모리를 할당한다.
	rawImage = new unsigned short[imageSize];
	if (!rawImage) { return false; }

	//raw 이미지 데이터를 읽는다.
	count = fread(rawImage, sizeof(unsigned short), imageSize, filePtr);
	if (count!= imageSize) { return false; }

	//파일을 닫는다.
	error = fclose(filePtr);
	if (error != 0) { return false; }

	for (int j = 0; j < m_terrainHeight; j++)
	{
		for (int i = 0; i < m_terrainWidth; i++)
		{
			//가로(width)로는 안가고 세로로만 간다.
			index = (m_terrainWidth * j) + i;

			//높이맵 배열에 데이터를 넣는다.
			m_heightMap[index].y = (float)rawImage[index];
		}
	}

	// Release the bitmap image data.
	delete[] rawImage;
	rawImage = 0;

	// Release the terrain filename now that it has been read in.
	delete[] m_terrainFilename;
	m_terrainFilename = 0;

	return true;
}

/*---------------------------------------------------------------------------------------------
이름: SetTerrainCoordinates()
용도: heightmap배열의 x,z값을 넣고 y값을 heightScale로 나눈다.
-----------------------------------------------------------------------------------------------*/
void TerrainClass::SetTerrainCoordinates()
{
	int index;

	for (int j = 0; j < m_terrainHeight; j++)
	{
		for (int i = 0; i < m_terrainWidth; i++)
		{
			index = (m_terrainWidth * j) + i;

			//x,z좌표를 세팅한다.
			//오른손좌표계? 니까 z값 -일수록 뒤로온다?
			m_heightMap[index].x = (float)i;
			m_heightMap[index].z = -(float)j;

			//terrain값을 양의 값으로 계속 옮긴다.
			//(0,-256)을 (256,0)으로.
			m_heightMap[index].z += (float)(m_terrainHeight - 1);

			// height값을 조정한다.
			m_heightMap[index].y /= m_heightScale;
		}
	}

	return;
}

/*---------------------------------------------------------------------------------------------
이름: NormalizeHeightMap()
용도: 지형을 통과하여 각 높이 값을 15로 나눠서 너무 스파이크 해보이지 않게 하는것이다.
-----------------------------------------------------------------------------------------------*/
void TerrainClass::NormalizeHeightMap()
{
	int i, j;


	for (j = 0; j<m_terrainHeight; j++)
	{
		for (i = 0; i<m_terrainWidth; i++)
		{
			m_heightMap[(m_terrainHeight * j) + i].y /= 15.0f;
		}
	}

	return;
}

/*---------------------------------------------------------------------------------------------
이름: CalculateNormal()
용도:
- 조명 계산을 위한 공유 법선을 계산한다.
- 공유 법선을 계산하려면 지형 격자의 각 꼭지점을 거치고 꼭지점의 일부인 각 면에 대한 법선의 평균을 취해야 한다.
- 그럼 그리드의 각 정점이 주변의 모든것에 평균화되어 빛의 방향이 덜 급격하게 변해 부드러운 표면이 보이게된다.(선형보간인가?)
- 이 기법은 모든 매끄러운 또는 반 매끄러운 다각형 표면에서 잘 작동한다.
- 그러나 큐브와 같이 날카로운 모서리를 가진 표면에서는 잘 작동하지 않는다.

- 각 면의 평균 법선을 구하고, 꼭지점을 하나하나 돌면서 면을 공유하는 꼭지점끼리 평균을 계산해서 공유 법선을 계산한다.
-----------------------------------------------------------------------------------------------*/
bool TerrainClass::CalculateNormal()
{
	int index1, index2, index3, index, count;
	
	float vertex1[3], vertex2[3], vertex3[3], vector1[3], vector2[3], sum[3], length;
	
	VectorType* normals;


	// 정규화 되지 않은 법선 벡터를 저장할 임시 배열을 만든다.
	normals = new VectorType[(m_terrainHeight - 1) * (m_terrainWidth - 1)];
	if (!normals){return false;}

	//메쉬의 모든 면을 살펴보고 법선을 계산한다.
	for (int j = 0; j<(m_terrainHeight - 1); j++)
	{
		for (int i = 0; i<(m_terrainWidth - 1); i++)
		{
			//삼각형 대로 돌아간다
			index1 = ((j + 1) * m_terrainWidth) + i;      
			index2 = ((j + 1) * m_terrainWidth) + (i + 1); 
			index3 = (j * m_terrainWidth) + i;          

			// 표면에서 세 개의 꼭지점을 가져온다.
			vertex1[0] = m_heightMap[index1].x;
			vertex1[1] = m_heightMap[index1].y;
			vertex1[2] = m_heightMap[index1].z;

			vertex2[0] = m_heightMap[index2].x;
			vertex2[1] = m_heightMap[index2].y;
			vertex2[2] = m_heightMap[index2].z;

			vertex3[0] = m_heightMap[index3].x;
			vertex3[1] = m_heightMap[index3].y;
			vertex3[2] = m_heightMap[index3].z;

			// 표면의 두 벡터를 계산한다.
			// 면 1개에 2개의 벡터를 계산한다.
			vector1[0] = vertex1[0] - vertex3[0];
			vector1[1] = vertex1[1] - vertex3[1];
			vector1[2] = vertex1[2] - vertex3[2];
			vector2[0] = vertex3[0] - vertex2[0];
			vector2[1] = vertex3[1] - vertex2[1];
			vector2[2] = vertex3[2] - vertex2[2];

			index = (j * (m_terrainWidth - 1)) + i;

			// 이 두 법선에 대한 정규화 되지 않은 값을 얻기 위해 두 벡터의 외적을 계산해서 그 면의 법선벡터를 생성한다.
			normals[index].nx = (vector1[1] * vector2[2]) - (vector1[2] * vector2[1]);
			normals[index].ny = (vector1[2] * vector2[0]) - (vector1[0] * vector2[2]);
			normals[index].nz = (vector1[0] * vector2[1]) - (vector1[1] * vector2[0]);

			// Calculate the length.
			length = (float)sqrt((normals[index].nx * normals[index].nx) + (normals[index].ny * normals[index].ny) +
				(normals[index].nz * normals[index].nz));

			// Normalize the final value for this face using the length.
			normals[index].nx = (normals[index].nx / length);
			normals[index].ny = (normals[index].ny / length);
			normals[index].nz = (normals[index].nz / length);
		}
	}

	// 이제 모든 정점을 살펴보고 각 면의 평균을 취한다.
	// 정점이 닿아 그 정점에 대한 평균 벡터값을 얻는다.
	for (int j = 0; j<m_terrainHeight; j++)
	{
		for (int i = 0; i<m_terrainWidth; i++)
		{
			// 합계 초기화.
			sum[0] = 0.0f;
			sum[1] = 0.0f;
			sum[2] = 0.0f;

			// 왼쪽 아래 면.
			if (((i - 1) >= 0) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (m_terrainWidth - 1)) + (i - 1);

				sum[0] += normals[index].nx;
				sum[1] += normals[index].ny;
				sum[2] += normals[index].nz;
			}

			// Bottom right face.
			if ((i<(m_terrainWidth - 1)) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (m_terrainWidth - 1)) + i;

				sum[0] += normals[index].nx;
				sum[1] += normals[index].ny;
				sum[2] += normals[index].nz;
			}

			// Upper left face.
			if (((i - 1) >= 0) && (j<(m_terrainHeight - 1)))
			{
				index = (j * (m_terrainWidth - 1)) + (i - 1);

				sum[0] += normals[index].nx;
				sum[1] += normals[index].ny;
				sum[2] += normals[index].nz;
			}
			// Upper right face.
			if ((i < (m_terrainWidth - 1)) && (j < (m_terrainHeight - 1)))
			{
				index = (j * (m_terrainWidth - 1)) + i;

				sum[0] += normals[index].nx;
				sum[1] += normals[index].ny;
				sum[2] += normals[index].nz;
			}


			// 이 법선의 길이를 계산한다.
			// 근데 sqrt()은... 시간이 오래 걸린다고 한다.(제곱근 구하는 함수)
			length = (float)sqrt((sum[0] * sum[0]) + (sum[1] * sum[1]) + (sum[2] * sum[2]));

			// 높이 맵 배열의 정점 위치에 대한 인덱스를 가져온다.
			index = (j * m_terrainWidth) + i;

			// 이 정점의 최종 공유 법선을 표준화하여 높이 맵 배열에 저장한다.
			m_heightMap[index].nx = (sum[0] / length);
			m_heightMap[index].ny = (sum[1] / length);
			m_heightMap[index].nz = (sum[2] / length);
		}
	}

	// Release the temporary normals.
	delete[] normals;
	normals = 0;

	return true;
}

/*---------------------------------------------------------------------------------------------
이름: LoadColorMap() -> 컬러맵은 비트맵.
용도: Setup에서 읽어온 컬러맵의 경로로 들어가서 그 안의 값들을 heightmap에 넣는다.
-----------------------------------------------------------------------------------------------*/
bool TerrainClass::LoadColorMap()
{
	int error, imageSize, i, j, k, index;
	FILE* filePtr;
	unsigned long long count;
	BITMAPFILEHEADER bitmapFileHeader;
	BITMAPINFOHEADER bitmapInfoHeader;
	unsigned char* bitmapImage;


	// Open the color map file in binary.
	error = fopen_s(&filePtr, m_colorMapFilename, "rb");
	if (error != 0)
	{
		return false;
	}

	// Read in the file header.
	count = fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	// Read in the bitmap info header.
	count = fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	// Make sure the color map dimensions are the same as the terrain dimensions for easy 1 to 1 mapping.
	if ((bitmapInfoHeader.biWidth != m_terrainWidth) || (bitmapInfoHeader.biHeight != m_terrainHeight))
	{
		return false;
	}

	// Calculate the size of the bitmap image data.
	// Since this is non-divide by 2 dimensions (eg. 257x257) need to add extra byte to each line.
	imageSize = m_terrainHeight * ((m_terrainWidth * 3) + 1);

	// Allocate memory for the bitmap image data.
	bitmapImage = new unsigned char[imageSize];
	if (!bitmapImage)
	{
		return false;
	}

	// Move to the beginning of the bitmap data.
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// Read in the bitmap image data.
	count = fread(bitmapImage, 1, imageSize, filePtr);
	if (count != imageSize)
	{
		return false;
	}

	// Close the file.
	error = fclose(filePtr);
	if (error != 0)
	{
		return false;
	}

	// Initialize the position in the image data buffer.
	k = 0;

	// Read the image data into the color map portion of the height map structure.
	for (j = 0; j<m_terrainHeight; j++)
	{
		for (i = 0; i<m_terrainWidth; i++)
		{
			// Bitmaps are upside down so load bottom to top into the array.
			index = (m_terrainWidth * (m_terrainHeight - 1 - j)) + i;

			m_heightMap[index].b = (float)bitmapImage[k] / 255.0f;
			m_heightMap[index].g = (float)bitmapImage[k + 1] / 255.0f;
			m_heightMap[index].r = (float)bitmapImage[k + 2] / 255.0f;

			k += 3;
		}

		// Compensate for extra byte at end of each line in non-divide by 2 bitmaps (eg. 257x257).
		k++;
	}

	// Release the bitmap image data.
	delete[] bitmapImage;
	bitmapImage = 0;

	// Release the color map filename now that is has been read in.
	delete[] m_colorMapFilename;
	m_colorMapFilename = 0;

	return true;
}

/*---------------------------------------------------------------------------------------------
이름: BuildTerrainModel()
용도: 위의 함수들이 만들어 놓은 heightmap배열에 있는 데이터를 토대로 실제 지형을 만든다.
-----------------------------------------------------------------------------------------------*/
bool TerrainClass::BuildTerrainModel()
{
	int i, j, index, index1, index2, index3, index4;


	// Calculate the number of vertices in the 3D terrain model.
	m_vertexCount = (m_terrainHeight - 1) * (m_terrainWidth - 1) * 6;

	// Create the 3D terrain model array.
	m_terrainModel = new ModelType[m_vertexCount];
	if (!m_terrainModel)
	{
		return false;
	}

	// Initialize the index into the height map array.
	index = 0;

	// Load the 3D terrain model with the height map terrain data.
	// We will be creating 2 triangles for each of the four points in a quad.
	for (j = 0; j<(m_terrainHeight - 1); j++)
	{
		for (i = 0; i<(m_terrainWidth - 1); i++)
		{
			// Get the indexes to the four points of the quad.
			index1 = (m_terrainWidth * j) + i;          // Upper left.
			index2 = (m_terrainWidth * j) + (i + 1);      // Upper right.
			index3 = (m_terrainWidth * (j + 1)) + i;      // Bottom left.
			index4 = (m_terrainWidth * (j + 1)) + (i + 1);  // Bottom right.

															// Now create two triangles for that quad.
															// Triangle 1 - Upper left.
			m_terrainModel[index].x = m_heightMap[index1].x;
			m_terrainModel[index].y = m_heightMap[index1].y;
			m_terrainModel[index].z = m_heightMap[index1].z;
			m_terrainModel[index].tu = 0.0f;
			m_terrainModel[index].tv = 0.0f;
			m_terrainModel[index].nx = m_heightMap[index1].nx;
			m_terrainModel[index].ny = m_heightMap[index1].ny;
			m_terrainModel[index].nz = m_heightMap[index1].nz;
			m_terrainModel[index].r = m_heightMap[index1].r;
			m_terrainModel[index].g = m_heightMap[index1].g;
			m_terrainModel[index].b = m_heightMap[index1].b;
			index++;

			// Triangle 1 - Upper right.
			m_terrainModel[index].x = m_heightMap[index2].x;
			m_terrainModel[index].y = m_heightMap[index2].y;
			m_terrainModel[index].z = m_heightMap[index2].z;
			m_terrainModel[index].tu = 1.0f;
			m_terrainModel[index].tv = 0.0f;
			m_terrainModel[index].nx = m_heightMap[index2].nx;
			m_terrainModel[index].ny = m_heightMap[index2].ny;
			m_terrainModel[index].nz = m_heightMap[index2].nz;
			m_terrainModel[index].r = m_heightMap[index2].r;
			m_terrainModel[index].g = m_heightMap[index2].g;
			m_terrainModel[index].b = m_heightMap[index2].b;
			index++;

			// Triangle 1 - Bottom left.
			m_terrainModel[index].x = m_heightMap[index3].x;
			m_terrainModel[index].y = m_heightMap[index3].y;
			m_terrainModel[index].z = m_heightMap[index3].z;
			m_terrainModel[index].tu = 0.0f;
			m_terrainModel[index].tv = 1.0f;
			m_terrainModel[index].nx = m_heightMap[index3].nx;
			m_terrainModel[index].ny = m_heightMap[index3].ny;
			m_terrainModel[index].nz = m_heightMap[index3].nz;
			m_terrainModel[index].r = m_heightMap[index3].r;
			m_terrainModel[index].g = m_heightMap[index3].g;
			m_terrainModel[index].b = m_heightMap[index3].b;
			index++;

			// Triangle 2 - Bottom left.
			m_terrainModel[index].x = m_heightMap[index3].x;
			m_terrainModel[index].y = m_heightMap[index3].y;
			m_terrainModel[index].z = m_heightMap[index3].z;
			m_terrainModel[index].tu = 0.0f;
			m_terrainModel[index].tv = 1.0f;
			m_terrainModel[index].nx = m_heightMap[index3].nx;
			m_terrainModel[index].ny = m_heightMap[index3].ny;
			m_terrainModel[index].nz = m_heightMap[index3].nz;
			m_terrainModel[index].r = m_heightMap[index3].r;
			m_terrainModel[index].g = m_heightMap[index3].g;
			m_terrainModel[index].b = m_heightMap[index3].b;
			index++;

			// Triangle 2 - Upper right.
			m_terrainModel[index].x = m_heightMap[index2].x;
			m_terrainModel[index].y = m_heightMap[index2].y;
			m_terrainModel[index].z = m_heightMap[index2].z;
			m_terrainModel[index].tu = 1.0f;
			m_terrainModel[index].tv = 0.0f;
			m_terrainModel[index].nx = m_heightMap[index2].nx;
			m_terrainModel[index].ny = m_heightMap[index2].ny;
			m_terrainModel[index].nz = m_heightMap[index2].nz;
			m_terrainModel[index].r = m_heightMap[index2].r;
			m_terrainModel[index].g = m_heightMap[index2].g;
			m_terrainModel[index].b = m_heightMap[index2].b;
			index++;

			// Triangle 2 - Bottom right.
			m_terrainModel[index].x = m_heightMap[index4].x;
			m_terrainModel[index].y = m_heightMap[index4].y;
			m_terrainModel[index].z = m_heightMap[index4].z;
			m_terrainModel[index].tu = 1.0f;
			m_terrainModel[index].tv = 1.0f;
			m_terrainModel[index].nx = m_heightMap[index4].nx;
			m_terrainModel[index].ny = m_heightMap[index4].ny;
			m_terrainModel[index].nz = m_heightMap[index4].nz;
			m_terrainModel[index].r = m_heightMap[index4].r;
			m_terrainModel[index].g = m_heightMap[index4].g;
			m_terrainModel[index].b = m_heightMap[index4].b;
			index++;
		}
	}

	return true;
}

/*---------------------------------------------------------------------------------------------
이름: CalculateTerrainVectors()
용도: 생성한 Terrain모델의 좌표들로 계산된 tangent, binormal 벡터를 넣어준다.
-----------------------------------------------------------------------------------------------*/
void TerrainClass::CalculateTerrainVectors()
{
	int faceCount, i, index;
	TempVertexType vertex1, vertex2, vertex3;
	VectorType tangent, binormal;


	// 지형 모델의 면의 수를 계산한다.
	faceCount = m_vertexCount / 3;

	// Initialize the index to the model data.
	index = 0;

	//모든 면을 살펴보고 tangent, binormal 벡터를 계산한다.
	for (i = 0; i<faceCount; i++)
	{
		// Get the three vertices for this face from the terrain model.
		vertex1.x = m_terrainModel[index].x;
		vertex1.y = m_terrainModel[index].y;
		vertex1.z = m_terrainModel[index].z;
		vertex1.tu = m_terrainModel[index].tu;
		vertex1.tv = m_terrainModel[index].tv;
		vertex1.nx = m_terrainModel[index].nx;
		vertex1.ny = m_terrainModel[index].ny;
		vertex1.nz = m_terrainModel[index].nz;
		index++;

		vertex2.x = m_terrainModel[index].x;
		vertex2.y = m_terrainModel[index].y;
		vertex2.z = m_terrainModel[index].z;
		vertex2.tu = m_terrainModel[index].tu;
		vertex2.tv = m_terrainModel[index].tv;
		vertex2.nx = m_terrainModel[index].nx;
		vertex2.ny = m_terrainModel[index].ny;
		vertex2.nz = m_terrainModel[index].nz;
		index++;

		vertex3.x = m_terrainModel[index].x;
		vertex3.y = m_terrainModel[index].y;
		vertex3.z = m_terrainModel[index].z;
		vertex3.tu = m_terrainModel[index].tu;
		vertex3.tv = m_terrainModel[index].tv;
		vertex3.nx = m_terrainModel[index].nx;
		vertex3.ny = m_terrainModel[index].ny;
		vertex3.nz = m_terrainModel[index].nz;
		index++;

		// Calculate the tangent and binormal of that face.
		CalculateTangentBinormal(vertex1, vertex2, vertex3, tangent, binormal);

		// Store the tangent and binormal for this face back in the model structure.
		m_terrainModel[index - 1].tx = tangent.nx;
		m_terrainModel[index - 1].ty = tangent.ny;
		m_terrainModel[index - 1].tz = tangent.nz;
		m_terrainModel[index - 1].bx = binormal.nx;
		m_terrainModel[index - 1].by = binormal.ny;
		m_terrainModel[index - 1].bz = binormal.nz;

		m_terrainModel[index - 2].tx = tangent.nx;
		m_terrainModel[index - 2].ty = tangent.ny;
		m_terrainModel[index - 2].tz = tangent.nz;
		m_terrainModel[index - 2].bx = binormal.nx;
		m_terrainModel[index - 2].by = binormal.ny;
		m_terrainModel[index - 2].bz = binormal.nz;

		m_terrainModel[index - 3].tx = tangent.nx;
		m_terrainModel[index - 3].ty = tangent.ny;
		m_terrainModel[index - 3].tz = tangent.nz;
		m_terrainModel[index - 3].bx = binormal.nx;
		m_terrainModel[index - 3].by = binormal.ny;
		m_terrainModel[index - 3].bz = binormal.nz;
	}

	return;
}

/*---------------------------------------------------------------------------------------------
이름: CalculateTangentBinormal()
용도: 
- normal벡터를 이용해 tangent, binormal벡터를 구한다.
- tangent벡터: normal vector과 수직인 벡터 (매우 여러개임)
- binormal 벡터: 노말, 탄젠트 벡터를 외적한 값
-----------------------------------------------------------------------------------------------*/
void TerrainClass::CalculateTangentBinormal(TempVertexType vertex1, TempVertexType vertex2, TempVertexType vertex3, VectorType& tangent, VectorType& binormal)
{
	float vector1[3], vector2[3];
	float tuVector[2], tvVector[2];
	float den;
	float length;


	// Calculate the two vectors for this face.
	vector1[0] = vertex2.x - vertex1.x;
	vector1[1] = vertex2.y - vertex1.y;
	vector1[2] = vertex2.z - vertex1.z;

	vector2[0] = vertex3.x - vertex1.x;
	vector2[1] = vertex3.y - vertex1.y;
	vector2[2] = vertex3.z - vertex1.z;

	// Calculate the tu and tv texture space vectors.
	tuVector[0] = vertex2.tu - vertex1.tu;
	tvVector[0] = vertex2.tv - vertex1.tv;

	tuVector[1] = vertex3.tu - vertex1.tu;
	tvVector[1] = vertex3.tv - vertex1.tv;

	// Calculate the denominator of the tangent/binormal equation.
	den = 1.0f / (tuVector[0] * tvVector[1] - tuVector[1] * tvVector[0]);

	// Calculate the cross products and multiply by the coefficient to get the tangent and binormal.
	tangent.nx = (tvVector[1] * vector1[0] - tvVector[0] * vector2[0]) * den;
	tangent.ny = (tvVector[1] * vector1[1] - tvVector[0] * vector2[1]) * den;
	tangent.nz = (tvVector[1] * vector1[2] - tvVector[0] * vector2[2]) * den;

	binormal.nx = (tuVector[0] * vector2[0] - tuVector[1] * vector1[0]) * den;
	binormal.ny = (tuVector[0] * vector2[1] - tuVector[1] * vector1[1]) * den;
	binormal.nz = (tuVector[0] * vector2[2] - tuVector[1] * vector1[2]) * den;

	// Calculate the length of the tangent.
	length = (float)sqrt((tangent.nx * tangent.nx) + (tangent.ny * tangent.ny) + (tangent.nz * tangent.nz));

	// Normalize the tangent and then store it.
	tangent.nx = tangent.nx / length;
	tangent.ny = tangent.ny / length;
	tangent.nz = tangent.nz / length;

	// Calculate the length of the binormal.
	length = (float)sqrt((binormal.nx * binormal.nx) + (binormal.ny * binormal.ny) + (binormal.nz * binormal.nz));

	// Normalize the binormal and then store it.
	binormal.nx = binormal.nx / length;
	binormal.ny = binormal.ny / length;
	binormal.nz = binormal.nz / length;

	return;
}

/*---------------------------------------------------------------------------------------------
이름: LoadTerrainCells()
용도: 
- Terrain Cell 객체 배열을 만든다.
- Cell안의 정점 갯수를 정하고 배열을 생성한다 
-> 배열이 만들어지면 루프를 돌면서 각 셀을 내부의 Terrain 모델의 일부로 초기화 한다.
-> Terrain의 포인터와 지표를 셀의 현재 위치에 보내 Terrain에서 데이터를 읽을 위치를 알고 현재 Cell을 로드한다.
-----------------------------------------------------------------------------------------------*/
bool TerrainClass::LoadTerrainCells(ID3D11Device* device)
{
	int cellHeight, cellWidth, cellRowCount, index;
	bool result;

	// Terrain Cell 내부의 정점들의 갯수를 정한다. 지금은 33x33
	cellHeight = 33;
	cellWidth = 33;

	// 지형에 Cell이 몇개가 필요한지 필요한 셀 수를 계산한다.
	cellRowCount = (m_terrainWidth - 1) / (cellWidth - 1);

	// 필요한 최종 셀 갯수.
	m_cellCount = cellRowCount * cellRowCount;

	// Terrain Cell 배열 생성.
	m_TerrainCells = new TerrainCellClass[m_cellCount];
	if (!m_TerrainCells) { return false; }

	// 가로, 세로를 필요한 셀의 갯수 만큼 돌면서 셀 안에 셀 내부의 모델 데이터를 넣어준다.
	for (int j = 0; j < cellRowCount; j++)
	{
		for (int i = 0; i < cellRowCount; i++)
		{
			index = (cellRowCount * j) + i;

			result = m_TerrainCells[index].Initialize(device, m_terrainModel, i, j, cellHeight, cellWidth, m_terrainWidth);
			if (!result) { return false; }
		}
	}

	return true;
}

/*---------------------------------------------------------------------------------------------
이름: GetHeightAtPosition()
용도:
- x,z입력이 있는 지형 위에 현재 높이를 반환한다.
- 카메라가 지형을 벗어나서 높이를 찾을 수 없으면 함수는 false를 반환한다.
- 위치가 어느것이든 찾을 때 까지 모든 TerrainCell을 반복한다.
- Cell을 찾으면 해당 Cell의 모든 삼각형을 반복하고 현재 위에 있는 삼각형을 찾는다.
- inputX = 사용자의 x위치 inputZ = 사용자의 z위치
-----------------------------------------------------------------------------------------------*/
bool TerrainClass::GetHeightAtPosition(float inputX, float inputZ, float& height)
{
	int cellId, index;
	float vertex1[3], vertex2[3], vertex3[3];
	bool foundHeight;
	float maxWidth, maxHeight, maxDepth, minWidth, minHeight, minDepth;

	cellId = -1;
	//모든 Cell을 돌면서 사용자의 x,z값이 TerrainCell의 면적에 포함하는지 알아낸다.
	for (int i = 0; i < m_cellCount; i++)
	{
		m_TerrainCells[i].GetCellDimensions(maxWidth, maxHeight, maxDepth, minWidth, minHeight, minDepth);

		if ((inputX <maxWidth) && (inputX> minWidth) && (inputZ <maxDepth) && (inputZ>minDepth))
		{
			cellId = i;
			i = m_cellCount;
		}
	}
	if (cellId == -1)
	{
		return false;
	}

	for (int i = 0; i <(m_TerrainCells[cellId].GetVertexCount() / 3); i++)
	{
		index = i * 3;

		vertex1[0] = m_TerrainCells[cellId].m_vertexList[index].nx;
		vertex1[1] = m_TerrainCells[cellId].m_vertexList[index].ny;
		vertex1[2] = m_TerrainCells[cellId].m_vertexList[index].nz;
		index++;

		vertex2[0] = m_TerrainCells[cellId].m_vertexList[index].nx;
		vertex2[1] = m_TerrainCells[cellId].m_vertexList[index].ny;
		vertex2[2] = m_TerrainCells[cellId].m_vertexList[index].nz;
		index++;

		vertex3[0] = m_TerrainCells[cellId].m_vertexList[index].nx;
		vertex3[1] = m_TerrainCells[cellId].m_vertexList[index].ny;
		vertex3[2] = m_TerrainCells[cellId].m_vertexList[index].nz;

		// 이것이 우리가 찾고있는 폴리곤인지 확인합니다. 
		foundHeight = CheckHeightOfTriangle(inputX, inputZ, height, vertex1, vertex2, vertex3);
		if (foundHeight)
		{
			return true;
		}
	}


	return false;

}
/*---------------------------------------------------------------------------------------------
이름: CheckHeightOfTriangle()
용도: 
- 선이 주어진 입력 삼각형과 교차하는지 판단하는 함수.
- 삼각형의 세 점을 취해 그 세점에서 세 줄을 만든다.
-> 그 다음 위치가 세 줄의 내부에 있는지 테스트 한다.
- 세줄 안에 모두 있으면 교차가 발견되고 높이가 계산된다.
-----------------------------------------------------------------------------------------------*/
bool TerrainClass::CheckHeightOfTriangle(float x, float z, float & height, float v0[3], float v1[3], float v2[3])
{
	float startVector[3], directionVector[3], edge1[3], edge2[3], normal[3];
	float Q[3], e1[3], e2[3], e3[3], edgeNormal[3], temp[3];
	float magnitude, D, denominator, numerator, t, determinant;
	
	// 전송 중인 광선의 시작 위치.
	startVector[0] = x;
	startVector[1] = 0.0f;
	startVector[2] = z;

	// 광선이 쏘아질 방향.
	directionVector[0] = 0.0f;
	directionVector[1] = -1.0f;
	directionVector[2] = 0.0f;

	// 주어진 세 점(Cell안의 삼각형)들을 이용해 두 모서리를 계산한다.
	edge1[0] = v1[0] - v0[0];
	edge1[1] = v1[1] - v0[1];
	edge1[2] = v1[2] - v0[2];

	edge2[0] = v2[0] - v0[0];
	edge2[1] = v2[1] - v0[1];
	edge2[2] = v2[2] - v0[2];

	// 두 모서리를 이용해 삼각형의 법선을 계산한다.
	normal[0] = (edge1[1] * edge2[2]) - (edge1[2] * edge2[1]);
	normal[1] = (edge1[2] * edge2[0]) - (edge1[0] * edge2[2]);
	normal[2] = (edge1[0] * edge2[1]) - (edge1[1] * edge2[0]);

	magnitude = (float)sqrt((normal[0] * normal[0]) + (normal[1] * normal[1]) + (normal[2] * normal[2]));
	normal[0] = normal[0] / magnitude;
	normal[1] = normal[1] / magnitude;
	normal[2] = normal[2] / magnitude;

	// 원점에서 평면까지의 거리를 구한다.
	D = ((-normal[0] * v0[0]) + (-normal[1] * v0[1]) + (-normal[2] * v0[2]));

	// Get the denominator of the equation.
	denominator = ((normal[0] * directionVector[0]) + (normal[1] * directionVector[1]) + (normal[2] * directionVector[2]));

	// Make sure the result doesn't get too close to zero to prevent divide by zero.
	if (fabs(denominator) < 0.0001f)
	{
		return false;
	}

	// Get the numerator of the equation.
	numerator = -1.0f * (((normal[0] * startVector[0]) + (normal[1] * startVector[1]) + (normal[2] * startVector[2])) + D);

	// Calculate where we intersect the triangle.
	t = numerator / denominator;

	// Find the intersection vector.
	Q[0] = startVector[0] + (directionVector[0] * t);
	Q[1] = startVector[1] + (directionVector[1] * t);
	Q[2] = startVector[2] + (directionVector[2] * t);

	// Find the three edges of the triangle.
	e1[0] = v1[0] - v0[0];
	e1[1] = v1[1] - v0[1];
	e1[2] = v1[2] - v0[2];

	e2[0] = v2[0] - v1[0];
	e2[1] = v2[1] - v1[1];
	e2[2] = v2[2] - v1[2];

	e3[0] = v0[0] - v2[0];
	e3[1] = v0[1] - v2[1];
	e3[2] = v0[2] - v2[2];

	// Calculate the normal for the first edge.
	edgeNormal[0] = (e1[1] * normal[2]) - (e1[2] * normal[1]);
	edgeNormal[1] = (e1[2] * normal[0]) - (e1[0] * normal[2]);
	edgeNormal[2] = (e1[0] * normal[1]) - (e1[1] * normal[0]);

	// Calculate the determinant to see if it is on the inside, outside, or directly on the edge.
	temp[0] = Q[0] - v0[0];
	temp[1] = Q[1] - v0[1];
	temp[2] = Q[2] - v0[2];

	determinant = ((edgeNormal[0] * temp[0]) + (edgeNormal[1] * temp[1]) + (edgeNormal[2] * temp[2]));

	// Check if it is outside.
	if (determinant > 0.001f)
	{
		return false;
	}

	// Calculate the normal for the second edge.
	edgeNormal[0] = (e2[1] * normal[2]) - (e2[2] * normal[1]);
	edgeNormal[1] = (e2[2] * normal[0]) - (e2[0] * normal[2]);
	edgeNormal[2] = (e2[0] * normal[1]) - (e2[1] * normal[0]);

	// Calculate the determinant to see if it is on the inside, outside, or directly on the edge.
	temp[0] = Q[0] - v1[0];
	temp[1] = Q[1] - v1[1];
	temp[2] = Q[2] - v1[2];

	determinant = ((edgeNormal[0] * temp[0]) + (edgeNormal[1] * temp[1]) + (edgeNormal[2] * temp[2]));

	// Check if it is outside.
	if (determinant > 0.001f)
	{
		return false;
	}

	// Calculate the normal for the third edge.
	edgeNormal[0] = (e3[1] * normal[2]) - (e3[2] * normal[1]);
	edgeNormal[1] = (e3[2] * normal[0]) - (e3[0] * normal[2]);
	edgeNormal[2] = (e3[0] * normal[1]) - (e3[1] * normal[0]);

	// Calculate the determinant to see if it is on the inside, outside, or directly on the edge.
	temp[0] = Q[0] - v2[0];
	temp[1] = Q[1] - v2[1];
	temp[2] = Q[2] - v2[2];

	determinant = ((edgeNormal[0] * temp[0]) + (edgeNormal[1] * temp[1]) + (edgeNormal[2] * temp[2]));

	// Check if it is outside.
	if (determinant > 0.001f)
	{
		return false;
	}

	// Now we have our height.
	height = Q[1];

	return true;

}

/*---------------------------------------------------------------------------------------------
이름: LoadTexture()
용도: 텍스처를 생성한다.
-----------------------------------------------------------------------------------------------*/
bool TerrainClass::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, CHAR* ColorTexture, CHAR* NormalMapTexture)
{
	bool result;

	m_ColorTexture = new TextureClass;
	if (!m_ColorTexture) { return false; }

	result = m_ColorTexture->InitializeTGA(device, deviceContext, ColorTexture);
	if (!result) { return false; }

	m_NormalMapTexture = new TextureClass;
	if (!m_NormalMapTexture) { return false; }

	result = m_NormalMapTexture->InitializeTGA(device, deviceContext, NormalMapTexture);
	if (!m_NormalMapTexture) { return false; }


	return true;
}


/*---------------------------------------------------------------------------------------------
이름: GetTexture()
용도: 텍스처를 가져온다. 여기선 단일 텍스처.
-----------------------------------------------------------------------------------------------*/
ID3D11ShaderResourceView* TerrainClass::GetColorTexture()
{
	return m_ColorTexture->GetTGATexture();
}

ID3D11ShaderResourceView* TerrainClass::GetNormalMapTexture()
{
	return m_NormalMapTexture->GetTGATexture();
}

int TerrainClass::GetVertexCount()
{
	return m_vertexCount;
}
/*---------------------------------------------------------------------------------------------
이름: RenderCell()
용도: 
- Terrain Cell Culling을 수행한다.
- Terrain Cell의 크기를 가져온 다음 절두체 객체를 사용해 Cell을 볼 수 있는지 여부를 결정한다.
-----------------------------------------------------------------------------------------------*/
bool TerrainClass::RenderCell(ID3D11DeviceContext* deviceContext, int cellId, FrustumClass* Frustum)
{
	float maxWidth, maxHeight, maxDepth, minWidth, minHeight, minDepth;
	bool result;

	//Terrain Cell의 크기를 가져온다.
	m_TerrainCells[cellId].GetCellDimensions(maxWidth, maxHeight, maxDepth, minWidth, minHeight, minDepth);

	//Terrain Cell이 표시되는지 확인한다.
	result = Frustum->CheckRectangle2(maxWidth, maxHeight, maxDepth, minWidth, minHeight, minDepth);
	if (!result) { return false; }

	//셀을 렌더링한다.
	m_TerrainCells[cellId].Render(deviceContext);
	
	
	return true;

}


void TerrainClass::RenderCellLines(ID3D11DeviceContext* deviceContext, int cellId)
{
	m_TerrainCells[cellId].RenderLineBuffers(deviceContext);
	return;
}


int TerrainClass::GetCellIndexCount(int cellId)
{
	return m_TerrainCells[cellId].GetIndexCount();
}


int TerrainClass::GetCellLinesIndexCount(int cellId)
{
	return m_TerrainCells[cellId].GetLineBuffersIndexCount();
}


int TerrainClass::GetCellCount()
{
	return m_cellCount;
}


/*---------------------------------------------------------------------------------------------
이름: ShutDown()
용도: 해제한다.
-----------------------------------------------------------------------------------------------*/
void TerrainClass::Shutdown()
{
	ReleaseTexture();

	ShutdownTerrainCells();
	//ShutdownBuffer();

	ShutdownTerrainModel();

	ShutdownHeightMap();
}

void TerrainClass::Shutdownforwater()
{
	ReleaseTexture();
	// Release the rendering buffers.
	ShutdownBufferforwater();

	// Release the terrain model.
	ShutdownTerrainModel();

	// Release the height map.
	ShutdownHeightMap();

	return;
}
void TerrainClass::ShutdownTerrainModel()
{
	// Release the terrain model data.
	if (m_terrainModel)
	{
		delete[] m_terrainModel;
		m_terrainModel = 0;
	}

	return;
}
void TerrainClass::ShutdownTerrainCells()
{
	if (m_TerrainCells)
	{
		for (int i = 0; i < m_cellCount; i++)
		{
			m_TerrainCells[i].Shutdown();
		}
		delete[] m_TerrainCells;
		m_TerrainCells = 0;
	}
	return;
}
void TerrainClass::ReleaseTexture()
{


	// Release the texture object.
	if (m_NormalMapTexture)
	{
		m_NormalMapTexture->Shutdown();
		delete m_NormalMapTexture;
		m_NormalMapTexture = 0;
	}
	// Release the texture object.
	if (m_ColorTexture)
	{
		m_ColorTexture->Shutdown();
		delete m_ColorTexture;
		m_ColorTexture = 0;
	}
	return;
}
void TerrainClass::ShutdownHeightMap()
{
	if (m_heightMap)
	{
		delete[] m_heightMap;
		m_heightMap = 0;
	}

	return;
}

void TerrainClass::ShutdownBufferforwater()
{
	// Release the index buffer.
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}


//void TerrainClass::CopyVertexArray(void* vertexList)
//{
//	memcpy(vertexList, m_vertices, sizeof(VertexType) * m_vertexCount);
//	return;
//}
/*---------------------------------------------------------------------------------------------
이름: Render
용도: 그리기를 준비하기 위해 그래픽스 파이프라인에 꼭지점과 인덱스 버퍼를 놓는다.
-----------------------------------------------------------------------------------------------*/
//void TerrainClass::Render(ID3D11DeviceContext* deviceContext)
//{
//	RenderBuffers(deviceContext);
//}
/*---------------------------------------------------------------------------------------------
이름 : InitializeBuffer() ~~~~~~~~~~~~~모르겠다~~~~~~~~~~~~~~~~~
용도 :
- 지형 그리드 데이터를 보유하는 데 사용되는 정점 및 인덱스 버퍼를 생성한다.
- 지형을 이루는 쿼드는 2개의 삼각형으로 이루어진다.
(DX11에서 선을 그리려면 두 점이 필요하고 정사각형을 그리려면 네 선을 형성하기 위해 8점이 필요하다.
X*Y격자를 만들면 큰 사각형 만드는데 점이 8개 필요하고 그걸 이루는 사각형들의 선분들에 X2씩 하면 밑에 공식이 충족된다.)

- 법선 벡터 및 삼각형을 수용하도록 변경된다.
- 2개의 삼각형으로 이루어진 1개의 쿼드는 각 삼각형의 꼭지점이 시계 방향으로 저장되야 한다.

- 텍스처링을 위해 텍스처 좌표를 버텍스 버퍼에 넣는다.
- CalculateTextureCoord에서 계산은 개별 정점에서만 수행되었으므로 텍스처 가장자리가 만들어지지 않는다. 그리고 꼭지점은 그들이 속한 삼각형에 따라 0.0와 1.0f가 될 수 있다.
- 이를 처리하기 위해 tu, tv좌표가 위, 아래, 왼쪽 또는 오른쪽에 있는지 빠르게 확인한 다음 해당 삼각형에 대해 0.0f또는 1.0f인지 확인한다.
- 여기서 안나오는건 각 격자마다 테두리가 진다!!!

- quadtree를 쓰면서 더이상 정점 버퍼, 인덱스 버퍼, 인덱스 배열을 생성하지 않는다.
- 오직 정점 배열만 생성한다...............
---------------------------------------------------------------------------------------------*/
bool TerrainClass::InitializeBufferforwater(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	
	m_vertexCount = (m_terrainWidth - 1)* (m_terrainHeight - 1) * 6;

	m_indexCount = m_vertexCount;

	vertices = new VertexType[m_vertexCount];
	if (!vertices) { return false; }

	indices = new unsigned long[m_indexCount];
	if (!indices) { return false; }

	// 3D모델의 데이터로 정점들을 채운다. 
	for (int i = 0; i<m_vertexCount; i++)
	{
		vertices[i].position = D3DXVECTOR3(m_terrainModel[i].x, m_terrainModel[i].y, m_terrainModel[i].z);
		vertices[i].texture = D3DXVECTOR2(m_terrainModel[i].tu, m_terrainModel[i].tv);
		vertices[i].normal = D3DXVECTOR3(m_terrainModel[i].nx, m_terrainModel[i].ny, m_terrainModel[i].nz);
		vertices[i].tangent = D3DXVECTOR3(m_terrainModel[i].tx, m_terrainModel[i].ty, m_terrainModel[i].tz);
		vertices[i].binormal = D3DXVECTOR3(m_terrainModel[i].bx, m_terrainModel[i].by, m_terrainModel[i].bz);
		vertices[i].color = D3DXVECTOR3(m_terrainModel[i].r, m_terrainModel[i].g, m_terrainModel[i].b);
		indices[i] = i;
	}

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Release the arrays now that the buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}

/*---------------------------------------------------------------------------------------------
이름:RenderBuffers()
용도:버텍스, 인덱스 버퍼에 Terrain데이터들을 보낸다.
-----------------------------------------------------------------------------------------------*/
//void TerrainClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
//{
//	// 정점 버퍼 보폭 및 오프셋을 설정합니다.
//	unsigned int stride = sizeof(VertexType);
//	unsigned int offset = 0;
//
//	// 렌더링 할 수 있도록 입력 어셈블러에서 정점 버퍼를 활성으로 설정합니다.
//	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
//
//	// 렌더링 할 수 있도록 입력 어셈블러에서 인덱스 버퍼를 활성으로 설정합니다.
//	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
//
//	// 이 버텍스 버퍼에서 렌더링되어야하는 프리미티브의 타입을 설정한다.
//	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//
//	return;
//}
//


/*---------------------------------------------------------------------------------------------
이름: CalculateTextureCoordinates()
용도:
- 지형 그리드의 텍스처 좌표를 생성한다.
- 전역 변수 TEXTURE_REPEAT는 텍스처가 그리드에서 반복 매핑되는 횟수를 결정하는 데 사용된다.
- TEXTURE_REPEAT을 사용해 Terrain 격자의 각 정점에 대한 점진적 텍스처 좌표를 계산한 다음 이를 높이 맵 배열에 저장한다.

- 정점 마다 텍스처 좌표를 사용하기 때문에 가장자리를 올바르게 처리하지 못한다. (각 쿼드의 테두리)
EX) 2개의 그리드를 반복하는 텍스처가 있는 경우 0.0F와 0.5F의 두 텍스처 좌표만 얻는다. 우리는 실제로 0.0F, 0.5F, 1.0F을 가질 필요가 있다.
- 이 문제를 해결하기 위해 INITIALIZEbUFFER에서 정점 배열을 만들 때 가자리 좌표가 0.0f 또는 1.0f 일 때를 결정한다.
-----------------------------------------------------------------------------------------------*/
//void TerrainClass::CalculateTextureCoordinates()
//{
//	int incrementCount,tuCount, tvCount;
//	float incrementValue, tuCoordinate, tvCoordinate;
//
//
//	// 텍스처 좌표를 얼마나 많이 증가 시킬지 계산한다. (매핑 되는 횟수에 따라 매핑 되는 텍스처 좌표 량이 많아질테니까!!)
//	// Terrain 격자의 각 정점에 대한 점진적 텍스처 좌표를 계산한다.
//	incrementValue = (float)TEXTURE_REPEAT / (float)m_terrainWidth;
//
//	// 텍스처를 반복 할 횟수를 계산합니다. 
//	incrementCount = m_terrainWidth / TEXTURE_REPEAT;
//
//	// tu 및 tv 좌표 값을 초기화합니다. 
//	tuCoordinate = 0.0f;
//	tvCoordinate = 1.0f;
//
//	// tu 및 tv 좌표 인덱스를 초기화합니다. 
//	tuCount = 0;
//	tvCount = 0;
//
//	// 전체 높이 맵을 반복하고 각 꼭지점의 tu 및 tv 텍스처 좌표를 계산합니다.
//	for (int j = 0; j < m_terrainHeight; j++)
//	{
//		for(int i = 0; i <m_terrainWidth; i++)
//		{
//			//높이맵에 텍스처 좌표를 저장한다. 
//			m_heightMap[(m_terrainHeight * j) + i].tu = tuCoordinate;
//			m_heightMap[(m_terrainHeight * j) + i].tv = tvCoordinate;
//
//			// tu 텍스처 좌표를 증가 값만큼 증가시키고 인덱스를 1 씩 증가시킨다. 
//			// 왜 증가값만큼 증가시키냐면 TEXTURE_REPEAT만큼 이 좌표를 더 써야하니까!
//			tuCoordinate += incrementValue;
//			tuCount++;
//
//			// 텍스처의 오른쪽 끝에 있는지 확인하고, 그렇다면 처음부터 다시 시작해라.
//			if (tuCount == incrementCount)
//			{
//				tuCoordinate = 0.0f;
//				tuCount = 0;
//			}
//		}
//
//		// tv 텍스처 좌표를 증가 값만큼 증가시키고 인덱스를 1 씩 증가시킵니다. 
//		tvCoordinate -= incrementValue;
//		tvCount++;
//
//		// 텍스처의 상단에 있는지 확인하고, 그렇다면 하단에서 다시 시작합니다. 
//		if (tvCount == incrementCount)
//		{
//			tvCoordinate = 1.0f;
//			tvCount = 0;
//		}
//	}
//
//	return;
//}



