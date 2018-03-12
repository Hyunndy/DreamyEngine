#include "TerrainClass.h"

/*---------------------------------------------------------------------------------------------
�̸�:
�뵵:
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

	//m_vertexBuffer = 0;
	//m_indexBuffer = 0;

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
�̸�: Initialize()
�뵵: 
- �¾� ����,���̸� ���� �ε� �� ����ȭ, ���� ���� ���� ���� ���
-> �÷��� �ε�, ���� �� ����, tangent,binormal ���� ���
-> ��ü ������ ���� ����, �ε��� ���۸� ����� ��� ���� ���� Cell�� �ε� �Ѵ�. (�ø��� ����)
-> �ؽ�ó�ε� -> �� ������ ����
-----------------------------------------------------------------------------------------------*/
bool TerrainClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* SetupFilename, CHAR* ColorTexture, CHAR* NormalMapTexture)
{
	bool result;

	//Terrain filename, height, width, scailing, colormapfilename�� ��´�.
	result = LoadSetupFile(SetupFilename);

	// ������ ���̸��� �ε�
	//result = LoadHeightMap_Bitmap(heightMapFilename);
	result = LoadHeightMap_Raw();
	if (!result) { return false; }

	//���� �����Ͽ� ���� X,Z��ǥ�� �����ϰ� ���� ������ ���� ���� ���� ���̸� �����Ѵ�.
	SetTerrainCoordinates();

	// ���� ���� �ε��ϰ� ���̸� ���� ���� �� �Լ��� ȣ���Ͽ� ���� ���� ���� ������ ����Ѵ�.
	// ���� �������� ������ ����Ѵ�.
	result = CalculateNormal();
	if (!result) { return false; }
	
	// Calculate the texture coordinates.
	//CalculateTextureCoordinates();

	//������ �÷����� �ε��Ѵ�.
	result = LoadColorMap();
	if (!result) { return false; }

	//������ 3D���� �ۼ��Ѵ�.
	result = BuildTerrainModel();
	if (!result) { return false; }


	// ���� 3d���� ��������� ���� ���� �ʿ�������ϱ� �������Ѵ�.
	ShutdownHeightMap();

	// ���� �𵨿� ���� ź��Ʈ �� binormal�� ����Ѵ�.
	CalculateTerrainVectors();

	//result = InitializeBuffer(device);
	//if (!result) { return false; }

	// ���� �����ͷ� ���� ����� �ε��Ѵ�.
	result = LoadTerrainCells(device);
	if (!result) { return false; }

	//������ ���۰� �ε�Ǿ����Ƿ� ���� ���� �����Ѵ�.
	ShutdownTerrainModel();

	//�ؽ�ó �ε�
	result = LoadTexture(device, deviceContext, ColorTexture, NormalMapTexture);
	if (!result) { return true; }

	return true;
}

//bool TerrainClass::Render(ID3D11DeviceContext* deviceContext)
//{
//	//������ ������ ���ο� ������, �ε��� ���۸� ���� �������� �غ��Ѵ�.
//	RenderBuffers(deviceContext);
//
//	return true;
//}

int TerrainClass::GetIndexCount()
{
	return m_indexCount;
}

/*---------------------------------------------------------------------------------------------
�̸�: LoadSetupFile
�뵵: �¾������� �ε��� Terrain �̸�, color map�̸�, ����, �ʺ�, �����ϰ��� ����.
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
�̸�: LoadHeightMap
�뵵: 
- fopen(�ּ�, ���) : ���� ����
- fread(������ ����, ������, �ݺ� Ƚ��, ���� ���� ������) : ������ �о ����Ѵ�.

1) ��Ʈ������ ���̸�
- ���� ���� �����ϴ� ��Ʈ �� ������ �� ���� �� �迭�� �ε��ϴ� ���ο� �Լ�.
- ��Ʈ�ʿ��� ����, ���, �Ķ����� �ִ�. �׷��� �̰��� ȸ�� �̹����̹Ƿ� ����, �ʷ�, �Ķ����� ��� ���� �� �ִ�.

2) RAW���� ���̸�
- 16��Ʈ RAW���̸� ������ �ε��Ѵ�.
- 16��Ʈ �����̱� ������ unsigned char ��� unsigned short�� ����Ͽ� �����͸� ���� �迭�� �����.
- �迭�� �Ľ��Ͽ� ���� �� ������ �����͸� ������ �� �������� tranverse���� �ʾƵ� �ȴ�.
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

	// ���̳ʸ� ���� ���̸� ������ ����.
	// *���̳ʸ� ���: �ؽ�Ʈ ������ ���̴� �ؽ�Ʈ ���� �ü���� ���� '\r'�� ���δٴ� ��, ���������� ���� ���
	if(fopen_s(&filePtr, filename, "rb") != 0)
	{
		return false;
	}

	// ���� ����� �д´�.
	if(fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr) != 1)
	{
		return false;
	}

	// ��Ʈ�� ���� ����� �д´�.
	if(fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr) !=1)
	{
		return false;
	}

	// ���� ũ�⸦ �����Ѵ�.
	// ������ �������� �Ӹ� �ƴ϶� �������� �ε��� ���۸� ����µ� �� ���� ����� �� �ִ�.
	m_terrainWidth = bitmapInfoHeader.biWidth;
	m_terrainHeight = bitmapInfoHeader.biHeight;



	//��Ʈ�� �̹��� �������� ũ�⸦ ����Ѵ�.
	imageSize = m_terrainWidth * m_terrainHeight * 3;

	//��Ʈ�� �̹��� �����Ϳ� �޸𸮸� �Ҵ��Ѵ�.
	bitmapImage = new unsigned char[imageSize];
	if (!bitmapImage) { return false; }

	//��Ʈ�� �������� ���� �κ����� �̵��Ѵ�.
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// ��Ʈ�� �̹��� �����͸� �д´�.
	if(fread(bitmapImage, 1, imageSize, filePtr) != imageSize)
	{
		return false;
	}

	// ������ �ݴ´�.
	if(fclose(filePtr) !=0)
	{
		return false;
	}


	// ���� ��Ʈ���� �о� �鿩 2���� ���� �� �迭�� ����� ���۸� �о���δ�.
	// for���� ���� �� ���� ���� ������ ������ x(�ʺ�)�� z(����)�� ����Ѵ�.
	// �׸��� ���� ��Ʈ�� ���� ������ Y(����)�� ����Ѵ�.
	// �׷��� ������ ������ ���Ǵ� ���� ��(��,��,��)�� �ϳ��� �ʿ��ϱ� ������ �ε����� ��Ʈ ��(k)�� 3�� ������Ų��.

	// ���̸� �����͸� ������ ����ü�� �����Ѵ�.
	m_heightMap = new HeightMapType[m_terrainHeight*m_terrainWidth];
	if (!m_heightMap) { return false; }

	// �̹��� ������ ������ ��ġ�� �ʱ�ȭ �Ѵ�.
	int k = 0;

	// �̹��� �����͸� ���� ������ �о���δ�.
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

	//Setup���Ͽ��� �� �ʺ�, ���̰����� ���̸� ������ �迭�� �����Ѵ�.
	m_heightMap = new HeightMapType[m_terrainWidth * m_terrainHeight];
	if (!m_heightMap) { return false; }

	//�������� �б� ���� 16��Ʈ raw ���̸��� ����.
	error = fopen_s(&filePtr, m_terrainFilename, "rb");
	if (error != 0) { return false; }

	//raw �̹��� �������� ����� ����Ѵ�.
	imageSize = m_terrainHeight*m_terrainWidth;

	//raw �̹��� �����Ϳ� �޸𸮸� �Ҵ��Ѵ�.
	rawImage = new unsigned short[imageSize];
	if (!rawImage) { return false; }

	//raw �̹��� �����͸� �д´�.
	count = fread(rawImage, sizeof(unsigned short), imageSize, filePtr);
	if (count!= imageSize) { return false; }

	//������ �ݴ´�.
	error = fclose(filePtr);
	if (error != 0) { return false; }

	for (int j = 0; j < m_terrainHeight; j++)
	{
		for (int i = 0; i < m_terrainWidth; i++)
		{
			//����(width)�δ� �Ȱ��� ���ηθ� ����.
			index = (m_terrainWidth * j) + i;

			//���̸� �迭�� �����͸� �ִ´�.
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
�̸�: SetTerrainCoordinates()
�뵵: heightmap�迭�� x,z���� �ְ� y���� heightScale�� ������.
-----------------------------------------------------------------------------------------------*/
void TerrainClass::SetTerrainCoordinates()
{
	int index;

	for (int j = 0; j < m_terrainHeight; j++)
	{
		for (int i = 0; i < m_terrainWidth; i++)
		{
			index = (m_terrainWidth * j) + i;

			//x,z��ǥ�� �����Ѵ�.
			//��������ǥ��? �ϱ� z�� -�ϼ��� �ڷο´�?
			m_heightMap[index].x = (float)i;
			m_heightMap[index].z = -(float)j;

			//terrain���� ���� ������ ��� �ű��.
			//(0,-256)�� (256,0)����.
			m_heightMap[index].z += (float)(m_terrainHeight - 1);

			// height���� �����Ѵ�.
			m_heightMap[index].y /= m_heightScale;
		}
	}

	return;
}

/*---------------------------------------------------------------------------------------------
�̸�: NormalizeHeightMap()
�뵵: ������ ����Ͽ� �� ���� ���� 15�� ������ �ʹ� ������ũ �غ����� �ʰ� �ϴ°��̴�.
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
�̸�: CalculateNormal()
�뵵:
- ���� ����� ���� ���� ������ ����Ѵ�.
- ���� ������ ����Ϸ��� ���� ������ �� �������� ��ġ�� �������� �Ϻ��� �� �鿡 ���� ������ ����� ���ؾ� �Ѵ�.
- �׷� �׸����� �� ������ �ֺ��� ���Ϳ� ���ȭ�Ǿ� ���� ������ �� �ް��ϰ� ���� �ε巯�� ǥ���� ���̰Եȴ�.(���������ΰ�?)
- �� ����� ��� �Ų����� �Ǵ� �� �Ų����� �ٰ��� ǥ�鿡�� �� �۵��Ѵ�.
- �׷��� ť��� ���� ��ī�ο� �𼭸��� ���� ǥ�鿡���� �� �۵����� �ʴ´�.

- �� ���� ��� ������ ���ϰ�, �������� �ϳ��ϳ� ���鼭 ���� �����ϴ� ���������� ����� ����ؼ� ���� ������ ����Ѵ�.
-----------------------------------------------------------------------------------------------*/
bool TerrainClass::CalculateNormal()
{
	int index1, index2, index3, index, count;
	
	float vertex1[3], vertex2[3], vertex3[3], vector1[3], vector2[3], sum[3], length;
	
	VectorType* normals;


	// ����ȭ ���� ���� ���� ���͸� ������ �ӽ� �迭�� �����.
	normals = new VectorType[(m_terrainHeight - 1) * (m_terrainWidth - 1)];
	if (!normals){return false;}

	//�޽��� ��� ���� ���캸�� ������ ����Ѵ�.
	for (int j = 0; j<(m_terrainHeight - 1); j++)
	{
		for (int i = 0; i<(m_terrainWidth - 1); i++)
		{
			//�ﰢ�� ��� ���ư���
			index1 = ((j + 1) * m_terrainWidth) + i;      
			index2 = ((j + 1) * m_terrainWidth) + (i + 1); 
			index3 = (j * m_terrainWidth) + i;          

			// ǥ�鿡�� �� ���� �������� �����´�.
			vertex1[0] = m_heightMap[index1].x;
			vertex1[1] = m_heightMap[index1].y;
			vertex1[2] = m_heightMap[index1].z;

			vertex2[0] = m_heightMap[index2].x;
			vertex2[1] = m_heightMap[index2].y;
			vertex2[2] = m_heightMap[index2].z;

			vertex3[0] = m_heightMap[index3].x;
			vertex3[1] = m_heightMap[index3].y;
			vertex3[2] = m_heightMap[index3].z;

			// ǥ���� �� ���͸� ����Ѵ�.
			// �� 1���� 2���� ���͸� ����Ѵ�.
			vector1[0] = vertex1[0] - vertex3[0];
			vector1[1] = vertex1[1] - vertex3[1];
			vector1[2] = vertex1[2] - vertex3[2];
			vector2[0] = vertex3[0] - vertex2[0];
			vector2[1] = vertex3[1] - vertex2[1];
			vector2[2] = vertex3[2] - vertex2[2];

			index = (j * (m_terrainWidth - 1)) + i;

			// �� �� ������ ���� ����ȭ ���� ���� ���� ��� ���� �� ������ ������ ����ؼ� �� ���� �������͸� �����Ѵ�.
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

	// ���� ��� ������ ���캸�� �� ���� ����� ���Ѵ�.
	// ������ ��� �� ������ ���� ��� ���Ͱ��� ��´�.
	for (int j = 0; j<m_terrainHeight; j++)
	{
		for (int i = 0; i<m_terrainWidth; i++)
		{
			// �հ� �ʱ�ȭ.
			sum[0] = 0.0f;
			sum[1] = 0.0f;
			sum[2] = 0.0f;

			// ���� �Ʒ� ��.
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


			// �� ������ ���̸� ����Ѵ�.
			// �ٵ� sqrt()��... �ð��� ���� �ɸ��ٰ� �Ѵ�.(������ ���ϴ� �Լ�)
			length = (float)sqrt((sum[0] * sum[0]) + (sum[1] * sum[1]) + (sum[2] * sum[2]));

			// ���� �� �迭�� ���� ��ġ�� ���� �ε����� �����´�.
			index = (j * m_terrainWidth) + i;

			// �� ������ ���� ���� ������ ǥ��ȭ�Ͽ� ���� �� �迭�� �����Ѵ�.
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
�̸�: LoadColorMap() -> �÷����� ��Ʈ��.
�뵵: Setup���� �о�� �÷����� ��η� ���� �� ���� ������ heightmap�� �ִ´�.
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
�̸�: BuildTerrainModel()
�뵵: ���� �Լ����� ����� ���� heightmap�迭�� �ִ� �����͸� ���� ���� ������ �����.
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
�̸�: CalculateTerrainVectors()
�뵵: ������ Terrain���� ��ǥ��� ���� tangent, binormal ���͸� �־��ش�.
-----------------------------------------------------------------------------------------------*/
void TerrainClass::CalculateTerrainVectors()
{
	int faceCount, i, index;
	TempVertexType vertex1, vertex2, vertex3;
	VectorType tangent, binormal;


	// ���� ���� ���� ���� ����Ѵ�.
	faceCount = m_vertexCount / 3;

	// Initialize the index to the model data.
	index = 0;

	//��� ���� ���캸�� tangent, binormal ���͸� ����Ѵ�.
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
�̸�: CalculateTangentBinormal()
�뵵: 
- normal���͸� �̿��� tangent, binormal���͸� ���Ѵ�.
- tangent����: normal vector�� ������ ���� (�ſ� ��������)
- binormal ����: �븻, ź��Ʈ ���͸� ������ ��
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
�̸�: LoadTerrainCells()
�뵵: 
- Terrain Cell ��ü �迭�� �����.
- Cell���� ���� ������ ���ϰ� �迭�� �����Ѵ� 
-> �迭�� ��������� ������ ���鼭 �� ���� ������ Terrain ���� �Ϻη� �ʱ�ȭ �Ѵ�.
-> Terrain�� �����Ϳ� ��ǥ�� ���� ���� ��ġ�� ���� Terrain���� �����͸� ���� ��ġ�� �˰� ���� Cell�� �ε��Ѵ�.
-----------------------------------------------------------------------------------------------*/
bool TerrainClass::LoadTerrainCells(ID3D11Device* device)
{
	int cellHeight, cellWidth, cellRowCount, index;
	bool result;

	// Terrain Cell ������ �������� ������ ���Ѵ�. ������ 33x33
	cellHeight = 33;
	cellWidth = 33;

	// ������ Cell�� ��� �ʿ����� �ʿ��� �� ���� ����Ѵ�.
	cellRowCount = (m_terrainWidth - 1) / (cellWidth - 1);

	// �ʿ��� ���� �� ����.
	m_cellCount = cellRowCount * cellRowCount;

	// Terrain Cell �迭 ����.
	m_TerrainCells = new TerrainCellClass[m_cellCount];
	if (!m_TerrainCells) { return false; }

	// ����, ���θ� �ʿ��� ���� ���� ��ŭ ���鼭 �� �ȿ� �� ������ �� �����͸� �־��ش�.
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
�̸�: GetHeightAtPosition()
�뵵:
- x,z�Է��� �ִ� ���� ���� ���� ���̸� ��ȯ�Ѵ�.
- ī�޶� ������ ����� ���̸� ã�� �� ������ �Լ��� false�� ��ȯ�Ѵ�.
- ��ġ�� ������̵� ã�� �� ���� ��� TerrainCell�� �ݺ��Ѵ�.
- Cell�� ã���� �ش� Cell�� ��� �ﰢ���� �ݺ��ϰ� ���� ���� �ִ� �ﰢ���� ã�´�.
- inputX = ������� x��ġ inputZ = ������� z��ġ
-----------------------------------------------------------------------------------------------*/
bool TerrainClass::GetHeightAtPosition(float inputX, float inputZ, float& height)
{
	int cellId, index;
	float vertex1[3], vertex2[3], vertex3[3];
	bool foundHeight;
	float maxWidth, maxHeight, maxDepth, minWidth, minHeight, minDepth;

	cellId = -1;
	//��� Cell�� ���鼭 ������� x,z���� TerrainCell�� ������ �����ϴ��� �˾Ƴ���.
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

		// �̰��� �츮�� ã���ִ� ���������� Ȯ���մϴ�. 
		foundHeight = CheckHeightOfTriangle(inputX, inputZ, height, vertex1, vertex2, vertex3);
		if (foundHeight)
		{
			return true;
		}
	}


	return false;

}
/*---------------------------------------------------------------------------------------------
�̸�: CheckHeightOfTriangle()
�뵵: 
- ���� �־��� �Է� �ﰢ���� �����ϴ��� �Ǵ��ϴ� �Լ�.
- �ﰢ���� �� ���� ���� �� �������� �� ���� �����.
-> �� ���� ��ġ�� �� ���� ���ο� �ִ��� �׽�Ʈ �Ѵ�.
- ���� �ȿ� ��� ������ ������ �߰ߵǰ� ���̰� ���ȴ�.
-----------------------------------------------------------------------------------------------*/
bool TerrainClass::CheckHeightOfTriangle(float x, float z, float & height, float v0[3], float v1[3], float v2[3])
{
	float startVector[3], directionVector[3], edge1[3], edge2[3], normal[3];
	float Q[3], e1[3], e2[3], e3[3], edgeNormal[3], temp[3];
	float magnitude, D, denominator, numerator, t, determinant;
	
	// ���� ���� ������ ���� ��ġ.
	startVector[0] = x;
	startVector[1] = 0.0f;
	startVector[2] = z;

	// ������ ����� ����.
	directionVector[0] = 0.0f;
	directionVector[1] = -1.0f;
	directionVector[2] = 0.0f;

	// �־��� �� ��(Cell���� �ﰢ��)���� �̿��� �� �𼭸��� ����Ѵ�.
	edge1[0] = v1[0] - v0[0];
	edge1[1] = v1[1] - v0[1];
	edge1[2] = v1[2] - v0[2];

	edge2[0] = v2[0] - v0[0];
	edge2[1] = v2[1] - v0[1];
	edge2[2] = v2[2] - v0[2];

	// �� �𼭸��� �̿��� �ﰢ���� ������ ����Ѵ�.
	normal[0] = (edge1[1] * edge2[2]) - (edge1[2] * edge2[1]);
	normal[1] = (edge1[2] * edge2[0]) - (edge1[0] * edge2[2]);
	normal[2] = (edge1[0] * edge2[1]) - (edge1[1] * edge2[0]);

	magnitude = (float)sqrt((normal[0] * normal[0]) + (normal[1] * normal[1]) + (normal[2] * normal[2]));
	normal[0] = normal[0] / magnitude;
	normal[1] = normal[1] / magnitude;
	normal[2] = normal[2] / magnitude;

	// �������� �������� �Ÿ��� ���Ѵ�.
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
�̸�: LoadTexture()
�뵵: �ؽ�ó�� �����Ѵ�.
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
�̸�: GetTexture()
�뵵: �ؽ�ó�� �����´�. ���⼱ ���� �ؽ�ó.
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
�̸�: RenderCell()
�뵵: 
- Terrain Cell Culling�� �����Ѵ�.
- Terrain Cell�� ũ�⸦ ������ ���� ����ü ��ü�� ����� Cell�� �� �� �ִ��� ���θ� �����Ѵ�.
-----------------------------------------------------------------------------------------------*/
bool TerrainClass::RenderCell(ID3D11DeviceContext* deviceContext, int cellId, FrustumClass* Frustum)
{
	float maxWidth, maxHeight, maxDepth, minWidth, minHeight, minDepth;
	bool result;

	//Terrain Cell�� ũ�⸦ �����´�.
	m_TerrainCells[cellId].GetCellDimensions(maxWidth, maxHeight, maxDepth, minWidth, minHeight, minDepth);

	//Terrain Cell�� ǥ�õǴ��� Ȯ���Ѵ�.
	result = Frustum->CheckRectangle2(maxWidth, maxHeight, maxDepth, minWidth, minHeight, minDepth);
	if (!result) { return false; }

	//���� �������Ѵ�.
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
�̸�: ShutDown()
�뵵: �����Ѵ�.
-----------------------------------------------------------------------------------------------*/
void TerrainClass::Shutdown()
{
	ReleaseTexture();

	ShutdownTerrainCells();
	//ShutdownBuffer();

	ShutdownTerrainModel();

	ShutdownHeightMap();
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

//void TerrainClass::ShutdownBuffer()
//{
//	// Release the index buffer.
//	if (m_indexBuffer)
//	{
//		m_indexBuffer->Release();
//		m_indexBuffer = 0;
//	}
//
//	// Release the vertex buffer.
//	if (m_vertexBuffer)
//	{
//		m_vertexBuffer->Release();
//		m_vertexBuffer = 0;
//	}
//
//	return;
//}


//void TerrainClass::CopyVertexArray(void* vertexList)
//{
//	memcpy(vertexList, m_vertices, sizeof(VertexType) * m_vertexCount);
//	return;
//}
/*---------------------------------------------------------------------------------------------
�̸�: Render
�뵵: �׸��⸦ �غ��ϱ� ���� �׷��Ƚ� ���������ο� �������� �ε��� ���۸� ���´�.
-----------------------------------------------------------------------------------------------*/
//void TerrainClass::Render(ID3D11DeviceContext* deviceContext)
//{
//	RenderBuffers(deviceContext);
//}
/*---------------------------------------------------------------------------------------------
�̸� : InitializeBuffer() ~~~~~~~~~~~~~�𸣰ڴ�~~~~~~~~~~~~~~~~~
�뵵 :
- ���� �׸��� �����͸� �����ϴ� �� ���Ǵ� ���� �� �ε��� ���۸� �����Ѵ�.
- ������ �̷�� ����� 2���� �ﰢ������ �̷������.
(DX11���� ���� �׸����� �� ���� �ʿ��ϰ� ���簢���� �׸����� �� ���� �����ϱ� ���� 8���� �ʿ��ϴ�.
X*Y���ڸ� ����� ū �簢�� ����µ� ���� 8�� �ʿ��ϰ� �װ� �̷�� �簢������ ���е鿡 X2�� �ϸ� �ؿ� ������ �����ȴ�.)

- ���� ���� �� �ﰢ���� �����ϵ��� ����ȴ�.
- 2���� �ﰢ������ �̷���� 1���� ����� �� �ﰢ���� �������� �ð� �������� ����Ǿ� �Ѵ�.

- �ؽ�ó���� ���� �ؽ�ó ��ǥ�� ���ؽ� ���ۿ� �ִ´�.
- CalculateTextureCoord���� ����� ���� ���������� ����Ǿ����Ƿ� �ؽ�ó �����ڸ��� ��������� �ʴ´�. �׸��� �������� �׵��� ���� �ﰢ���� ���� 0.0�� 1.0f�� �� �� �ִ�.
- �̸� ó���ϱ� ���� tu, tv��ǥ�� ��, �Ʒ�, ���� �Ǵ� �����ʿ� �ִ��� ������ Ȯ���� ���� �ش� �ﰢ���� ���� 0.0f�Ǵ� 1.0f���� Ȯ���Ѵ�.
- ���⼭ �ȳ����°� �� ���ڸ��� �׵θ��� ����!!!

- quadtree�� ���鼭 ���̻� ���� ����, �ε��� ����, �ε��� �迭�� �������� �ʴ´�.
- ���� ���� �迭�� �����Ѵ�...............
---------------------------------------------------------------------------------------------*/
//bool TerrainClass::InitializeBuffer(ID3D11Device* device)
//{
//	VertexType* vertices;
//	unsigned long* indices;
//	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
//	D3D11_SUBRESOURCE_DATA vertexData, indexData;
//	HRESULT result;
//	
//	m_vertexCount = (m_terrainWidth - 1)* (m_terrainHeight - 1) * 6;
//
//	m_indexCount = m_vertexCount;
//
//	vertices = new VertexType[m_vertexCount];
//	if (!vertices) { return false; }
//
//	indices = new unsigned long[m_indexCount];
//	if (!indices) { return false; }
//
//	// 3D���� �����ͷ� �������� ä���. 
//	for (int i = 0; i<m_vertexCount; i++)
//	{
//		vertices[i].position = D3DXVECTOR3(m_terrainModel[i].x, m_terrainModel[i].y, m_terrainModel[i].z);
//		vertices[i].texture = D3DXVECTOR2(m_terrainModel[i].tu, m_terrainModel[i].tv);
//		vertices[i].normal = D3DXVECTOR3(m_terrainModel[i].nx, m_terrainModel[i].ny, m_terrainModel[i].nz);
//		vertices[i].tangent = D3DXVECTOR3(m_terrainModel[i].tx, m_terrainModel[i].ty, m_terrainModel[i].tz);
//		vertices[i].binormal = D3DXVECTOR3(m_terrainModel[i].bx, m_terrainModel[i].by, m_terrainModel[i].bz);
//		vertices[i].color = D3DXVECTOR3(m_terrainModel[i].r, m_terrainModel[i].g, m_terrainModel[i].b);
//		indices[i] = i;
//	}
//
//	// Set up the description of the static vertex buffer.
//	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
//	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
//	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//	vertexBufferDesc.CPUAccessFlags = 0;
//	vertexBufferDesc.MiscFlags = 0;
//	vertexBufferDesc.StructureByteStride = 0;
//
//	// Give the subresource structure a pointer to the vertex data.
//	vertexData.pSysMem = vertices;
//	vertexData.SysMemPitch = 0;
//	vertexData.SysMemSlicePitch = 0;
//
//	// Now create the vertex buffer.
//	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
//	if (FAILED(result))
//	{
//		return false;
//	}
//
//	// Set up the description of the static index buffer.
//	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
//	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
//	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
//	indexBufferDesc.CPUAccessFlags = 0;
//	indexBufferDesc.MiscFlags = 0;
//	indexBufferDesc.StructureByteStride = 0;
//
//	// Give the subresource structure a pointer to the index data.
//	indexData.pSysMem = indices;
//	indexData.SysMemPitch = 0;
//	indexData.SysMemSlicePitch = 0;
//
//	// Create the index buffer.
//	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
//	if (FAILED(result))
//	{
//		return false;
//	}
//
//	// Release the arrays now that the buffers have been created and loaded.
//	delete[] vertices;
//	vertices = 0;
//
//	delete[] indices;
//	indices = 0;
//
//	return true;
//}

/*---------------------------------------------------------------------------------------------
�̸�:RenderBuffers()
�뵵:���ؽ�, �ε��� ���ۿ� Terrain�����͵��� ������.
-----------------------------------------------------------------------------------------------*/
//void TerrainClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
//{
//	// ���� ���� ���� �� �������� �����մϴ�.
//	unsigned int stride = sizeof(VertexType);
//	unsigned int offset = 0;
//
//	// ������ �� �� �ֵ��� �Է� ��������� ���� ���۸� Ȱ������ �����մϴ�.
//	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
//
//	// ������ �� �� �ֵ��� �Է� ��������� �ε��� ���۸� Ȱ������ �����մϴ�.
//	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
//
//	// �� ���ؽ� ���ۿ��� �������Ǿ���ϴ� ������Ƽ���� Ÿ���� �����Ѵ�.
//	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//
//	return;
//}
//


/*---------------------------------------------------------------------------------------------
�̸�: CalculateTextureCoordinates()
�뵵:
- ���� �׸����� �ؽ�ó ��ǥ�� �����Ѵ�.
- ���� ���� TEXTURE_REPEAT�� �ؽ�ó�� �׸��忡�� �ݺ� ���εǴ� Ƚ���� �����ϴ� �� ���ȴ�.
- TEXTURE_REPEAT�� ����� Terrain ������ �� ������ ���� ������ �ؽ�ó ��ǥ�� ����� ���� �̸� ���� �� �迭�� �����Ѵ�.

- ���� ���� �ؽ�ó ��ǥ�� ����ϱ� ������ �����ڸ��� �ùٸ��� ó������ ���Ѵ�. (�� ������ �׵θ�)
EX) 2���� �׸��带 �ݺ��ϴ� �ؽ�ó�� �ִ� ��� 0.0F�� 0.5F�� �� �ؽ�ó ��ǥ�� ��´�. �츮�� ������ 0.0F, 0.5F, 1.0F�� ���� �ʿ䰡 �ִ�.
- �� ������ �ذ��ϱ� ���� INITIALIZEbUFFER���� ���� �迭�� ���� �� ���ڸ� ��ǥ�� 0.0f �Ǵ� 1.0f �� ���� �����Ѵ�.
-----------------------------------------------------------------------------------------------*/
//void TerrainClass::CalculateTextureCoordinates()
//{
//	int incrementCount,tuCount, tvCount;
//	float incrementValue, tuCoordinate, tvCoordinate;
//
//
//	// �ؽ�ó ��ǥ�� �󸶳� ���� ���� ��ų�� ����Ѵ�. (���� �Ǵ� Ƚ���� ���� ���� �Ǵ� �ؽ�ó ��ǥ ���� �������״ϱ�!!)
//	// Terrain ������ �� ������ ���� ������ �ؽ�ó ��ǥ�� ����Ѵ�.
//	incrementValue = (float)TEXTURE_REPEAT / (float)m_terrainWidth;
//
//	// �ؽ�ó�� �ݺ� �� Ƚ���� ����մϴ�. 
//	incrementCount = m_terrainWidth / TEXTURE_REPEAT;
//
//	// tu �� tv ��ǥ ���� �ʱ�ȭ�մϴ�. 
//	tuCoordinate = 0.0f;
//	tvCoordinate = 1.0f;
//
//	// tu �� tv ��ǥ �ε����� �ʱ�ȭ�մϴ�. 
//	tuCount = 0;
//	tvCount = 0;
//
//	// ��ü ���� ���� �ݺ��ϰ� �� �������� tu �� tv �ؽ�ó ��ǥ�� ����մϴ�.
//	for (int j = 0; j < m_terrainHeight; j++)
//	{
//		for(int i = 0; i <m_terrainWidth; i++)
//		{
//			//���̸ʿ� �ؽ�ó ��ǥ�� �����Ѵ�. 
//			m_heightMap[(m_terrainHeight * j) + i].tu = tuCoordinate;
//			m_heightMap[(m_terrainHeight * j) + i].tv = tvCoordinate;
//
//			// tu �ؽ�ó ��ǥ�� ���� ����ŭ ������Ű�� �ε����� 1 �� ������Ų��. 
//			// �� ��������ŭ ������Ű�ĸ� TEXTURE_REPEAT��ŭ �� ��ǥ�� �� ����ϴϱ�!
//			tuCoordinate += incrementValue;
//			tuCount++;
//
//			// �ؽ�ó�� ������ ���� �ִ��� Ȯ���ϰ�, �׷��ٸ� ó������ �ٽ� �����ض�.
//			if (tuCount == incrementCount)
//			{
//				tuCoordinate = 0.0f;
//				tuCount = 0;
//			}
//		}
//
//		// tv �ؽ�ó ��ǥ�� ���� ����ŭ ������Ű�� �ε����� 1 �� ������ŵ�ϴ�. 
//		tvCoordinate -= incrementValue;
//		tvCount++;
//
//		// �ؽ�ó�� ��ܿ� �ִ��� Ȯ���ϰ�, �׷��ٸ� �ϴܿ��� �ٽ� �����մϴ�. 
//		if (tvCount == incrementCount)
//		{
//			tvCoordinate = 1.0f;
//			tvCount = 0;
//		}
//	}
//
//	return;
//}



