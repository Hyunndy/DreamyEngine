////////////////////////////////////////////////////////////////////////////////
// Filename: textureclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "textureclass.h"


TextureClass::TextureClass()
{
	m_targaData = 0;
	m_texture = 0;
	m_TGAtexture = 0;
	m_TGAtextureView = 0;

	m_multitexture[0] = 0;
	m_multitexture[1] = 0;
	
	m_tripletexture[0] = 0;
	m_tripletexture[1] = 0;
	m_tripletexture[2] = 0;
}


TextureClass::TextureClass(const TextureClass& other)
{
}


TextureClass::~TextureClass()
{
}

/*
	Direct3D 디바이스와 텍스처의 파일 이름을 가지고 m_texture이라는 셰이더 자원 변수에 텍스처 파일을 로드한다.
	여기서 텍스처자원인 m_texture가 resourceview인것을 기억하자!
*/
bool TextureClass::Initialize(ID3D11Device* device, WCHAR* filename)
{
	HRESULT result;
	
	
	// 이미지 파일로부터 d3d11texture2d 객체 생성 + shaderresourceview에 연결해주는 함수!
	result = D3DX11CreateShaderResourceViewFromFile(device, filename, NULL, NULL,&m_texture,  NULL);
	if (FAILED(result))
	{
		return false;
	}


	return true;
}

bool TextureClass::InitializeMulti(ID3D11Device* device, WCHAR* filename, WCHAR* filename2)
{
	HRESULT result;


	// 이미지 파일로부터 d3d11texture2d 객체 생성 + shaderresourceview에 연결해주는 함수!
	result = D3DX11CreateShaderResourceViewFromFile(device, filename, NULL, NULL, &m_multitexture[0], NULL);
	if (FAILED(result)){	return false;	}

	result = D3DX11CreateShaderResourceViewFromFile(device, filename2, NULL, NULL, &m_multitexture[1], NULL);
	if (FAILED(result)) { return false; }

	return true;
}

bool TextureClass::InitializeTriple(ID3D11Device* device, WCHAR* filename, WCHAR* filename2, WCHAR* filename3)
{
	HRESULT result;


	// 이미지 파일로부터 d3d11texture2d 객체 생성 + shaderresourceview에 연결해주는 함수!
	result = D3DX11CreateShaderResourceViewFromFile(device, filename, NULL, NULL, &m_tripletexture[0], NULL);
	if (FAILED(result)) { return false; }

	result = D3DX11CreateShaderResourceViewFromFile(device, filename2, NULL, NULL, &m_tripletexture[1], NULL);
	if (FAILED(result)) { return false; }

	result = D3DX11CreateShaderResourceViewFromFile(device, filename3, NULL, NULL, &m_tripletexture[2], NULL);
	if (FAILED(result)) { return false; }

	return true;
}

bool TextureClass::InitializeTGA(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
	bool result;
	int height, width;
	D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT hResult;
	unsigned int rowPitch;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;


	// Load the targa image data into memory.
	result = LoadTarga(filename, height, width);
	if (!result)
	{
		return false;
	}

	// Setup the description of the texture.
	textureDesc.Height = height;
	textureDesc.Width = width;
	textureDesc.MipLevels = 0;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	// Create the empty texture.
	hResult = device->CreateTexture2D(&textureDesc, NULL, &m_TGAtexture);
	if (FAILED(hResult))
	{
		return false;
	}

	// Set the row pitch of the targa image data.
	rowPitch = (width * 4) * sizeof(unsigned char);

	// Copy the targa image data into the texture.
	deviceContext->UpdateSubresource(m_TGAtexture, 0, NULL, m_targaData, rowPitch, 0);

	// Setup the shader resource view description.
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	// Create the shader resource view for the texture.
	hResult = device->CreateShaderResourceView(m_TGAtexture, &srvDesc, &m_TGAtextureView);
	if (FAILED(hResult))
	{
		return false;
	}

	// Generate mipmaps for this texture.
	deviceContext->GenerateMips(m_TGAtextureView);

	// Release the targa image data now that the image data has been loaded into the texture.
	delete[] m_targaData;
	m_targaData = 0;

	return true;
}

void TextureClass::Shutdown()
{
	// Release the texture view resource.
	if (m_TGAtextureView)
	{
		m_TGAtextureView->Release();
		m_TGAtextureView = 0;
	}
	// Release the texture.
	if (m_TGAtexture)
	{
		m_TGAtexture->Release();
		m_TGAtexture = 0;
	}
	// Release the texture.
	if (m_texture)
	{
		m_texture->Release();
		m_texture = 0;
	}

	// Release the targa data.
	if (m_targaData)
	{
		delete[] m_targaData;
		m_targaData = 0;
	}
	// Release the texture resource.
	//if (m_texture) {m_texture->Release();	m_texture = 0;}

	if (m_multitexture[0]) { m_multitexture[0]->Release();	m_multitexture[0] = 0; }
	if (m_multitexture[1]) { m_multitexture[1]->Release();	m_multitexture[1] = 0; }

	if (m_tripletexture[0]) { m_tripletexture[0]->Release(); m_tripletexture[0] = 0; }
	if (m_tripletexture[1]) { m_tripletexture[1]->Release(); m_tripletexture[1] = 0; }
	if (m_tripletexture[2]) { m_tripletexture[2]->Release(); m_tripletexture[2] = 0; }


	return;
}

bool TextureClass::LoadTarga(char* filename, int& height, int& width)
{
	int error, bpp, imageSize, index, i, j, k;
	FILE* filePtr;
	unsigned int count;
	TargaHeader targaFileHeader;
	unsigned char* targaImage;


	// Open the targa file for reading in binary.
	error = fopen_s(&filePtr, filename, "rb");
	if (error != 0)
	{
		return false;
	}

	// Read in the file header.
	count = (unsigned int)fread(&targaFileHeader, sizeof(TargaHeader), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	// Get the important information from the header.
	height = (int)targaFileHeader.height;
	width = (int)targaFileHeader.width;
	bpp = (int)targaFileHeader.bpp;

	// Check that it is 32 bit and not 24 bit.
	if (bpp != 32)
	{
		return false;
	}

	// Calculate the size of the 32 bit image data.
	imageSize = width * height * 4;

	// Allocate memory for the targa image data.
	targaImage = new unsigned char[imageSize];
	if (!targaImage)
	{
		return false;
	}

	// Read in the targa image data.
	count = (unsigned int)fread(targaImage, 1, imageSize, filePtr);
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

	// Allocate memory for the targa destination data.
	m_targaData = new unsigned char[imageSize];
	if (!m_targaData)
	{
		return false;
	}

	// Initialize the index into the targa destination data array.
	index = 0;

	// Initialize the index into the targa image data.
	k = (width * height * 4) - (width * 4);

	// Now copy the targa image data into the targa destination array in the correct order since the targa format is stored upside down.
	for (j = 0; j<height; j++)
	{
		for (i = 0; i<width; i++)
		{
			m_targaData[index + 0] = targaImage[k + 2];  // Red.
			m_targaData[index + 1] = targaImage[k + 1];  // Green.
			m_targaData[index + 2] = targaImage[k + 0];  // Blue
			m_targaData[index + 3] = targaImage[k + 3];  // Alpha

														 // Increment the indexes into the targa data.
			k += 4;
			index += 4;
		}

		// Set the targa image data index back to the preceding row at the beginning of the column since its reading it in upside down.
		k -= (width * 8);
	}

	// Release the targa image data now that it was copied into the destination array.
	delete[] targaImage;
	targaImage = 0;

	return true;
}

// 다른 객체가 이 텍스처 셰이더 자원에 접근할 필요가 있을 때 사용된다.
// 이 함수를 통해 텍스처를 렌더링 할 수 있다.
ID3D11ShaderResourceView* TextureClass::GetTexture()
{
	return m_texture;
}

ID3D11ShaderResourceView* TextureClass::GetTripleTexture1()
{
	return m_tripletexture[0];
}

ID3D11ShaderResourceView* TextureClass::GetTripleTexture2()
{
	return m_tripletexture[1];
}

ID3D11ShaderResourceView* TextureClass::GetTripleTexture3()
{
	return m_tripletexture[2];
}

ID3D11ShaderResourceView** TextureClass::GetMultiTextureArray()
{
	return m_multitexture;
}

ID3D11ShaderResourceView** TextureClass::GetTripleTextureArray()
{
	return m_tripletexture;
}

ID3D11ShaderResourceView* TextureClass::GetTGATexture()
{
	return m_TGAtextureView;
}
