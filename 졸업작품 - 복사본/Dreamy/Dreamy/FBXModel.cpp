#include "FBXModel.h"


FBXModel::FBXModel()
{
	m_Texture = 0;
}


FBXModel::~FBXModel()
{
}

bool FBXModel::Initialize(ID3D11Device* device, char* modelFilename, WCHAR* textureFilename)
{
	FBXLoad(modelFilename);

	InitializeBuffers(device);

	LoadTexture(device, textureFilename);
	
	return true;
}

void FBXModel::Render(ID3D11DeviceContext* deviceContext)
{
	RenderBuffers(deviceContext);
}
bool FBXModel::FBXLoad(char* modelFilename)
{
	//FBXManager ��ü ����
	m_SdkManager = FbxManager::Create(); 
	//Scene��ü ����.
	//����ϰ� ���� FBX���� ���� SCENE�� ����������Ѵ�.
	FbxScene* scene = FbxScene::Create(m_SdkManager, "Scene");	
	//IOSettings ��ü ���� �� ����
	FbxIOSettings *ios = FbxIOSettings::Create(m_SdkManager, IOSROOT);
	m_SdkManager->SetIOSettings(ios);

	//FbxImporter ��ü ����
	m_Importer = FbxImporter::Create(m_SdkManager, "");

	int format = -1;

	if (!m_SdkManager->GetIOPluginRegistry()->DetectReaderFileFormat(modelFilename, format))
		format = m_SdkManager->GetIOPluginRegistry()->FindReaderIDByDescription("FBX binary (*.fbx)");

	bool result = m_Importer->Initialize(modelFilename, -1, m_SdkManager->GetIOSettings());
	if (!result)
	{
		printf("djpqwjdopwqjpd");
		exit(-1);
	}

	//���� �ε��ϱ� ���� �� �����̳�(scene)�� �����
	if (m_Importer->Import(scene))
	{

		//���� ������ ������ ä�� �Ŀ��� �޸� ����� ���̱� ���� �Ҹ��Ų��.
		/* Global Scene Setting.....*/
		// ���� �� �ý���, �ֺ� ����(ambient)�� �ð� ������ �ش� FbxGlobalSettings ��ü�� ���ǵȴ�.

		// ��ǥ���� �����´�.
		FbxAxisSystem sceneAxisSystem = scene->GetGlobalSettings().GetAxisSystem();
		// �� ���� ��ǥ���� �ٲ۴�.
		FbxAxisSystem::MayaZUp.ConvertScene(scene); 
		FbxAxisSystem ourAxisSystem(FbxAxisSystem::eOpenGL);

		//�� ������ �ﰢ��ȭ �� �� �ִ� ��� ��带 �ﰢ��ȭ ��Ų��.
		FbxGeometryConverter geometryConverter(m_SdkManager);
		geometryConverter.Triangulate(scene, true);



		//���� ��Ʈ ��带 �����´�.
		//rootNode->RotationPivot;
		//��������� ��带 Ž���Ѵ�.
		LoadNode(scene->GetRootNode());

		//������ �ٸ� ��ü �����.
		//mesh, light, animation
		//FbxMesh* mesh = FbxMesh::Create(scene, "mesh");
	}
}


bool FBXModel::LoadTexture(ID3D11Device* device, WCHAR* filename)
{
	bool result;

	m_Texture = new TextureClass;
	if (!m_Texture) { return false; }

	result = m_Texture->Initialize(device, filename);
	if (!result) { return false; }

	return true;
}

ID3D11ShaderResourceView* FBXModel::GetTexture()
{
	return m_Texture->GetTexture();
}

//��������� ��带 Ž���Ѵ�.
void FBXModel::LoadNode(FbxNode* node)
{

	//����� �Ӽ��� �����̳�!
	FbxNodeAttribute* nodeAttribute = node->GetNodeAttribute();
	if (nodeAttribute)
	{
		if (nodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			//����� �Ӽ��� �޽����
			//�޽��� ���� �۾��� ���⼭ �����Ѵ�.
			//����� �Ӽ����� �޽�, ����Ʈ, ī�޶� ���� �ִ�.

			// FbxMesh�� ĳ���õ� ��� �Ӽ��� �����͸� �����´�.
			FbxMesh* mesh = node->GetMesh();
			// ���������� ���� ��ġ ����Ʈ�� ä���.
			ParseControlPoints(mesh);

		//	if (s_hasAnimation)
		//		ParseAnimation(node);

			ParseMesh(mesh);
		}	
	}

	const unsigned int childCount = node->GetChildCount();

	// ���
	for (unsigned int i = 0; i < childCount; ++i)
		LoadNode(node->GetChild(i));
}

bool FBXModel::ParseMesh(FbxMesh* mesh)
{
	if (!mesh->GetNode())
		return false;

	//�޽��� �ﰢ�� ������ �����´�.
	unsigned int triCount = mesh->GetPolygonCount();

	unsigned int vertexCount = 0; // ������ ����

	for (int i = 0; i < triCount; ++i) //�ﰢ�� ����
	{
		vec3 tanget;
		vec3 binormal;
		vec2 uv;

		for (unsigned int j = 0; j < 3; ++j) // �ﰢ���� �� ���� �������� ����
		{
			int controlPointIndex = mesh->GetPolygonVertex(i, j);; // ������ �ε����� �����´�.
			//m_indexCount = controlPointIndex;

			//vec3& position = s_rawPositions[controlPointIndex].pos; //���� ������ ���� ��ġ
			vec3& position = positions[controlPointIndex].pos; //���� ������ ���� ��ġ
			vec3 normal = ParseNormal(mesh, controlPointIndex, vertexCount);
			vec3 tangent = ParseTangent(mesh, controlPointIndex, vertexCount);
			vec3 binormal= ParseBinormal(mesh, controlPointIndex, vertexCount);
			vec2 uv= ParseUV(mesh, controlPointIndex, mesh->GetTextureUVIndex(i, j));


			uv.y = 1.0f - uv.y;
			//position.z = position.z * -1.0f;
			normal.z = normal.z * -1.0f;
			normal.y = normal.y * -1.0f;

			InsertVertex(controlPointIndex, uv, normal, tangent, binormal);

			vertexCount++; // ������ ����++
		}
	}

	return true;
}

int FBXModel::GetIndexCount()
{
	return m_indexCount2;
}
vec2 FBXModel::ParseUV(const FbxMesh* mesh, int ControlPointIndex, int Textureindex)
{
	if (mesh->GetElementUVCount() < 1)
		return vec2();

	vec2 result;

	const FbxGeometryElementUV* vertexUV = mesh->GetElementUV(0);

	switch (vertexUV->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
		switch (vertexUV->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			result.x = static_cast<float>(vertexUV->GetDirectArray().GetAt(ControlPointIndex).mData[0]);
			result.y = static_cast<float>(vertexUV->GetDirectArray().GetAt(ControlPointIndex).mData[1]);
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexUV->GetIndexArray().GetAt(ControlPointIndex);
			result.x = static_cast<float>(vertexUV->GetDirectArray().GetAt(index).mData[0]);
			result.y = static_cast<float>(vertexUV->GetDirectArray().GetAt(index).mData[1]);
		}
		break;
		default:
			return vec2();
		}
		break;

	case FbxGeometryElement::eByPolygonVertex:
		switch (vertexUV->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		case FbxGeometryElement::eIndexToDirect:
		{
			result.x = static_cast<float>(vertexUV->GetDirectArray().GetAt(Textureindex).mData[0]);
			result.y = static_cast<float>(vertexUV->GetDirectArray().GetAt(Textureindex).mData[1]);
		}
		break;
		default:
			return vec2();
		}
		break;
	}

	return result;
}
vec3 FBXModel::ParseBinormal(const FbxMesh* mesh, int controlPointIndex, int vertexCounter)
{
	if (mesh->GetElementBinormalCount() < 1)
		return vec3();

	vec3 result; // �븻���͸� ������ ����

	//�޽��� �븻 ���͸� ȹ���Ѵ�.
	const FbxGeometryElementBinormal* vertexBinormal = mesh->GetElementBinormal(0);


	//�븻 ���Ͱ� ��� ���εǾ��ִ��� �˾ƺ���.
	switch (vertexBinormal->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
		//��Ʈ�� ����Ʈ ����
		switch (vertexBinormal->GetReferenceMode())
		{
			case FbxGeometryElement::eDirect:
			{
				result.x = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(controlPointIndex).mData[0]);
				result.y = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(controlPointIndex).mData[1]);
				result.z = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(controlPointIndex).mData[2]);
			}

			break;

			case FbxGeometryElement::eIndexToDirect:
			{
				int index = vertexBinormal->GetIndexArray().GetAt(controlPointIndex); //�ε����� ���´�.
				result.x = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(index).mData[0]);
				result.y = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(index).mData[1]);
				result.z = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(index).mData[2]);
			}

			break;
			default:
				return vec3();
		}

		break;

	case FbxGeometryElement::eByPolygonVertex:
		//�ε��� ����
	
		switch (vertexBinormal->GetReferenceMode())
		{
			case FbxGeometryElement::eDirect:
			{
				result.x = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(vertexCounter).mData[0]);
				result.y = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(vertexCounter).mData[1]);
				result.z = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(vertexCounter).mData[2]);
			}

			break;

			case FbxGeometryElement::eIndexToDirect:
			{
				int index = vertexBinormal->GetIndexArray().GetAt(vertexCounter);
				// �ε����� ���´�.
				result.x = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(index).mData[0]);
				result.y = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(index).mData[1]);
				result.z = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(index).mData[2]);
			}

			break;
			default:
				return vec3();
		}
		break;
	
	}
	return result;
}
vec3 FBXModel::ParseNormal(const FbxMesh* mesh, int controlPointIndex, int vertexCounter)
{
	if (mesh->GetElementNormalCount() < 1)
		return vec3();

	vec3 Normal; // �븻���͸� ������ ����

	//�޽��� �븻 ���͸� ȹ���Ѵ�.
	const FbxGeometryElementNormal* vertexNormal = mesh->GetElementNormal(0);


	//�븻 ���Ͱ� ��� ���εǾ��ִ��� �˾ƺ���.
	switch (vertexNormal->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
		//��Ʈ�� ����Ʈ ����
		switch (vertexNormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			Normal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(controlPointIndex).mData[0]);
			Normal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(controlPointIndex).mData[1]);
			Normal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(controlPointIndex).mData[2]);
		}

		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexNormal->GetIndexArray().GetAt(controlPointIndex); //�ε����� ���´�.
			Normal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[0]);
			Normal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[1]);
			Normal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[2]);
		}

		break;
		default:
			return vec3();
		}

		break;

	case FbxGeometryElement::eByPolygonVertex:
		//�ε��� ����
	{
		switch (vertexNormal->GetReferenceMode())
		{
			case FbxGeometryElement::eDirect:
			{
				Normal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(vertexCounter).mData[0]);
				Normal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(vertexCounter).mData[1]);
				Normal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(vertexCounter).mData[2]);
			}

			break;

			case FbxGeometryElement::eIndexToDirect:
			{
				int index = vertexNormal->GetIndexArray().GetAt(vertexCounter);
				// �ε����� ���´�.
				Normal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[0]);
				Normal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[1]);
				Normal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[2]);
			}

			break;
			default:
				return vec3();
		}
		break;
	}

	}
	return Normal;
}
vec3 FBXModel::ParseTangent(const FbxMesh* mesh, int controlPointIndex, int vertexCounter)
{
	if (mesh->GetElementTangentCount() < 1)
		return vec3();

	vec3 result; // �븻���͸� ������ ����

	//�޽��� �븻 ���͸� ȹ���Ѵ�.
	const FbxGeometryElementTangent* vertexTangent = mesh->GetElementTangent(0);


						//�븻 ���Ͱ� ��� ���εǾ��ִ��� �˾ƺ���.
	switch (vertexTangent->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
		switch (vertexTangent->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			result.x = static_cast<float>(vertexTangent->GetDirectArray().GetAt(controlPointIndex).mData[0]);
			result.y = static_cast<float>(vertexTangent->GetDirectArray().GetAt(controlPointIndex).mData[1]);
			result.z = static_cast<float>(vertexTangent->GetDirectArray().GetAt(controlPointIndex).mData[2]);
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexTangent->GetIndexArray().GetAt(controlPointIndex);
			result.x = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[0]);
			result.y = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[1]);
			result.z = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[2]);
		}
		break;
		default:
			return vec3();
		}
		break;

	case FbxGeometryElement::eByPolygonVertex:
		switch (vertexTangent->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			result.x = static_cast<float>(vertexTangent->GetDirectArray().GetAt(vertexCounter).mData[0]);
			result.y = static_cast<float>(vertexTangent->GetDirectArray().GetAt(vertexCounter).mData[1]);
			result.z = static_cast<float>(vertexTangent->GetDirectArray().GetAt(vertexCounter).mData[2]);
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexTangent->GetIndexArray().GetAt(vertexCounter);
			result.x = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[0]);
			result.y = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[1]);
			result.z = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[2]);
		}
		break;
		default:
			return vec3();
		}
		break;
	}

	return result;
}
//InsertVertex(controlPointindex, texture, normal, tangent, binormal);
void FBXModel::InsertVertex(const unsigned int rawPositionIndex, const vec2& uv, const vec3& normal, const vec3& tangent, const vec3& binormal)
{

	//sun::VertexWithBlending vertex = { s_rawPositions[rawPositionIndex], uv, normal, tangent, binormal };
	sun::VertexWithBlending vertex = { positions[rawPositionIndex], uv, normal, tangent, binormal };
	auto lookup = indexMapping.find(vertex);
	//auto lookup = s_indexMapping.find(vertex);

	if (lookup != indexMapping.end())
	{
		indices.push_back(lookup->second);

	}
	else
	{
		unsigned int index = vertices.size();
		indexMapping[vertex] = index;
		indices.push_back(index);
		vertices.push_back(vertex);


	}
//if (lookup != indexMapping.end())
//{
//	s_indices.push_back(lookup->second);
//}
//else
//{
//	uint index = s_vertices.size();
//	s_indexMapping[vertex] = index;
//	s_indices.push_back(index);
//	s_vertices.push_back(vertex);
//}
 
}



bool FBXModel::InitializeBuffers(ID3D11Device* device)
{
	//sun::VertexWithBlending* m_vertices;
	sun::VertexWithBlending* m_vertices;
	unsigned int* m_indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;
	int m_BufferCount = vertices.size();
	m_indexCount2 = indices.size();

	m_vertices = new sun::VertexWithBlending[m_BufferCount];
	if (!m_vertices) { return false; }

	m_indices = new unsigned int[m_indexCount2];
	if (!m_indices) { return false; }


	for (int i = 0; i < m_BufferCount; ++i)
	{
		m_vertices[i] = vertices[i];
		

	}

	for (int i = 0; i < m_indexCount2; ++i)
	{
		m_indices[i] = indices[i];
	}
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT; // ���۰� ���̴� ���
	vertexBufferDesc.ByteWidth = sizeof(sun::VertexWithBlending) * m_BufferCount; //������ ������ ũ��
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // ���� ��������
	vertexBufferDesc.CPUAccessFlags = 0; // cpu�� ���ۿ� �����ϴ� ���
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// ���� �����͸� ����Ű�� ���� ���ҽ� ����ü�� �ۼ��Ѵ�.
	// = ���� �迭�� �����´�.
	vertexData.pSysMem = m_vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// ���� ���� ����
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount2;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = m_indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// �ʿ� ������ ���� �迭, �ε��� �迭�� ���� �Ѵ�.
	delete[] m_vertices;
	m_vertices = 0;

	delete[] m_indices;
	m_indices = 0;

	return true;
}

void FBXModel::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;


	// ���� ������ ������ �������� �����Ѵ�.
	stride = sizeof(sun::VertexWithBlending);
	offset = 0;

	// input assmbler(������ ������������ ù��° �ܰ�)�� ���� ���۸� Ȱ��ȭ �Ͽ� �׷��� �� �ְ� �Ѵ�.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// �Է� ������(I/A)�� �ε��� ���۸� Ȱ��ȭ �Ͽ� �׷��� �� �ְ� �Ѵ�.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// ���� ���۷� �׸� �⺻���� �����Ѵ�. ���⼱ �ﰢ��. ��ä���� �� ���� �ְ� �׷���.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}
//control point�� ���� ���� ����Ʈ�� ä���.
void FBXModel::ParseControlPoints(FbxMesh* mesh)
{
	//�޽��� ���� ������ �����´�.
	m_count = mesh->GetControlPointsCount();
	positions = new sun::Position[m_count];

	for (unsigned int i = 0; i < m_count; ++i)
	{
		vec3 position;
		//�������� ���������� GetControlPointAt(int index)��� �Լ��� �̿��Ѵ�.
		//position.x = static_cast<float>(mesh->GetControlPointAt(i).mData[0]);
		//position.y = static_cast<float>(mesh->GetControlPointAt(i).mData[1]);
		//position.z = static_cast<float>(mesh->GetControlPointAt(i).mData[2]);
		position.x = static_cast<float>(mesh->GetControlPointAt(i).mData[0]);
		position.y = static_cast<float>(mesh->GetControlPointAt(i).mData[1]);
		position.z = static_cast<float>(mesh->GetControlPointAt(i).mData[2]);

		positions[i].pos = position;

	}
}



void FBXModel::ShutDown()
{
	if (m_Texture)
	{
		m_Texture->Shutdown();
		m_Texture = 0;
	}

	if (m_Importer)
	{
		m_Importer->Destroy();
		m_Importer = nullptr;
	}

	if (m_Scene)
	{
		m_Scene->Destroy();
		m_Scene = nullptr;
	}

	if (m_SdkManager)
	{
		m_SdkManager->Destroy();
		m_SdkManager = nullptr;
	}
	ShutdownBuffers();
}

void FBXModel::ShutdownBuffers()
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


