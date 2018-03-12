#include "FBXModel.h"


FBXModel::FBXModel()
{

}


FBXModel::~FBXModel()
{
}

bool FBXModel::Initialize(ID3D11Device* device)
{
	FBXLoad();
	InitializeBuffers(device);

	return true;
}

void FBXModel::Render(ID3D11DeviceContext* deviceContext)
{
	RenderBuffers(deviceContext);
}
bool FBXModel::FBXLoad()
{
	//FBXManager 객체 생성
	m_SdkManager = FbxManager::Create(); 
	//FBX Scene객체 생성.
	//사용하고 싶은 FBX파일 마다 SCENE을 생성해줘야한다.
	FbxScene* scene = FbxScene::Create(m_SdkManager, "Scene");	
	//IOSettings 객체 생성 및 설정
	FbxIOSettings *ios = FbxIOSettings::Create(m_SdkManager, IOSROOT);
	m_SdkManager->SetIOSettings(ios);

	//FbxImporter 객체 생성
	m_Importer = FbxImporter::Create(m_SdkManager, "");
	bool result = m_Importer->Initialize("../Dreamy/cone.fbx", -1, m_SdkManager->GetIOSettings());
	if (!result)
	{
		printf("djpqwjdopwqjpd");
		exit(-1);
	}

	//씬을 로드하기 위해 씬 컨테이너(scene)를 만든다
	if (m_Importer->Import(scene))
	{

		//파일 내용을 씬으로 채운 후에는 메모리 사용을 줄이기 위해 소멸시킨다.
		/* Global Scene Setting.....*/
		// 씬의 축 시스템, 주변 조명(ambient)및 시간 설정은 해당 FbxGlobalSettings 객체에 정의된다.

		// 좌표축을 가져온다.
		FbxAxisSystem sceneAxisSystem = scene->GetGlobalSettings().GetAxisSystem();
		// 씬 내의 좌표축을 바꾼다.
		FbxAxisSystem::MayaZUp.ConvertScene(scene); 
		FbxAxisSystem ourAxisSystem(FbxAxisSystem::eOpenGL);

		//씬 내에서 삼각형화 할 수 있는 모든 노드를 삼각형화 시킨다.
		FbxGeometryConverter geometryConverter(m_SdkManager);
		geometryConverter.Triangulate(scene, true);

		//씬의 루트 노드를 가져온다.
		FbxNode* rootNode = scene->GetRootNode();
		//rootNode->RotationPivot;
		//재귀적으로 노드를 탐색한다.
		LoadNode(rootNode);





		//씬내의 다른 객체 만들기.
		//mesh, light, animation
		//FbxMesh* mesh = FbxMesh::Create(scene, "mesh");
	}
}

//재귀적으로 노드를 탐색한다.
void FBXModel::LoadNode(FbxNode* node)
{
	//뭔가의 작업을 수행한다.
	VertexType model;

	//노드의 속성이 무엇이냐!
	FbxNodeAttribute* nodeAttribute = node->GetNodeAttribute();
	if (nodeAttribute)
	{
		if (nodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			//노드의 속성이 메쉬라면
			//메쉬에 대한 작업을 여기서 수행한다.
			//노드의 속성에는 메쉬, 라이트, 카메라 등이 있다.

			// FbxMesh로 캐스팅된 노드 속성의 포인터를 가져온다.
			FbxMesh* mesh = node->GetMesh();
			// 제어점으로 부터 위치 리스트를 채운다.
			ParseControlPoints(mesh);

			ParseMesh(mesh);
		}	
	}

	const unsigned int childCount = node->GetChildCount();
	
	for (unsigned int i = 0; i < childCount; i++)
	{
		LoadNode(node->GetChild(i));
	}
}

bool FBXModel::ParseMesh(FbxMesh* mesh)
{
	//메쉬의 삼각형 개수를 가져온다.
	unsigned int triCount = mesh->GetPolygonCount();

	unsigned int vertexCount = 0; // 정점의 갯수

	for (unsigned int i = 0; i < triCount; ++i) //삼각형 갯수
	{

		for (unsigned int j = 0; j < 3; ++j) // 삼각형은 세 개의 정점으로 구성
		{
			int controlPointindex = mesh->GetPolygonVertex(i, j); // 제어점 인덱스를 가져온다.
			m_indexCount = controlPointindex;

			vec3& position = positions[controlPointindex].position; //현재 정점에 대한 위치
			vec3 normal = ParseNormal(mesh, controlPointindex, vertexCount);
			vec3 tangent = ParseTangent(mesh, controlPointindex, vertexCount);
			vec3 binormal= ParseBinormal(mesh, controlPointindex, vertexCount);
			vec2 texture= ParseUV(mesh, controlPointindex, mesh->GetTextureUVIndex(i, j));


			texture.y = 1.0f - texture.y;
			normal.z = normal.z * -1.0f;

			InsertVertex(controlPointindex, texture, normal, tangent, binormal);

			vertexCount++; // 정점의 갯수++
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

	//메쉬의 노말 벡터를 획득한다.
	const FbxGeometryElementUV* vertexUV = mesh->GetElementUV(0);
	vec2 result; // 노말벡터를 저장할 벡터

						//노말 벡터가 어떻게 매핑되어있는지 알아본다.
	switch (vertexUV->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
		//컨트롤 포인트 매핑
		switch (vertexUV->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			result.x = static_cast<float>(vertexUV->GetDirectArray().GetAt(ControlPointIndex).mData[0]);
			result.y = static_cast<float>(vertexUV->GetDirectArray().GetAt(ControlPointIndex).mData[1]);
			//result.z = static_cast<float>(vertexUV->GetDirectArray().GetAt(ControlPointIndex).mData[2]);
		}

		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexUV->GetIndexArray().GetAt(ControlPointIndex); //인덱스를 얻어온다.
			result.x = static_cast<float>(vertexUV->GetDirectArray().GetAt(index).mData[0]);
			result.y = static_cast<float>(vertexUV->GetDirectArray().GetAt(index).mData[1]);
			//result.z = static_cast<float>(vertexUV->GetDirectArray().GetAt(index).mData[2]);
		}

		break;
		}

		break;

	case FbxGeometryElement::eByPolygonVertex:
		//인덱스 매핑
	{
		switch (vertexUV->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexUV->GetIndexArray().GetAt(Textureindex);
			// 인덱스를 얻어온다.
			result.x = static_cast<float>(vertexUV->GetDirectArray().GetAt(index).mData[0]);
			result.y = static_cast<float>(vertexUV->GetDirectArray().GetAt(index).mData[1]);
			//result.z = static_cast<float>(vertexUV->GetDirectArray().GetAt(index).mData[2]);
		}

		break;
		default:
			return vec2();
		}
		break;
	}

	}
	return result;
}
vec3 FBXModel::ParseBinormal(const FbxMesh* mesh, int controlPointIndex, int vertexCounter)
{
	if (mesh->GetElementBinormalCount() < 1)
		return vec3();

	//메쉬의 노말 벡터를 획득한다.
	const FbxGeometryElementBinormal* vertexBinormal = mesh->GetElementBinormal(0);
	vec3 result; // 노말벡터를 저장할 벡터

	//노말 벡터가 어떻게 매핑되어있는지 알아본다.
	switch (vertexBinormal->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
		//컨트롤 포인트 매핑
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
				int index = vertexBinormal->GetIndexArray().GetAt(controlPointIndex); //인덱스를 얻어온다.
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
		//인덱스 매핑
	
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
				// 인덱스를 얻어온다.
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

	//메쉬의 노말 벡터를 획득한다.
	const FbxGeometryElementNormal* vertexNormal = mesh->GetElementNormal(0);
	vec3 Normal; // 노말벡터를 저장할 벡터

						//노말 벡터가 어떻게 매핑되어있는지 알아본다.
	switch (vertexNormal->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
		//컨트롤 포인트 매핑
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
			int index = vertexNormal->GetIndexArray().GetAt(controlPointIndex); //인덱스를 얻어온다.
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
		//인덱스 매핑
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
				// 인덱스를 얻어온다.
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

	//메쉬의 노말 벡터를 획득한다.
	const FbxGeometryElementTangent* vertexTangent = mesh->GetElementTangent(0);
	vec3 result; // 노말벡터를 저장할 벡터

						//노말 벡터가 어떻게 매핑되어있는지 알아본다.
	switch (vertexTangent->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
		//컨트롤 포인트 매핑
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
			int index = vertexTangent->GetIndexArray().GetAt(controlPointIndex); //인덱스를 얻어온다.
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
		//인덱스 매핑
	{
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
			// 인덱스를 얻어온다.
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

	}
	return result;
}

void FBXModel::InsertVertex(const unsigned int rawPositionIndex, const vec2& uv, const vec3& normal, const vec3& tangent, const vec3& binormal)
{

	VertexType vertex = { positions[rawPositionIndex], uv, normal, tangent, binormal };
	auto lookup = indexMapping.find(vertex);
	
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
 
}


bool FBXModel::InitializeBuffers(ID3D11Device* device)
{
	VertexType* m_vertices;
	unsigned int* m_indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;
	int m_BufferCount = vertices.size();
	m_indexCount2 = indices.size();

	m_vertices = new VertexType[m_BufferCount];
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
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT; // 버퍼가 쓰이는 방식
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_BufferCount; //생성할 버퍼의 크기
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // 무슨 버퍼인지
	vertexBufferDesc.CPUAccessFlags = 0; // cpu가 버퍼에 접근하는 방식
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// 정점 데이터를 가리키는 보조 리소스 구조체를 작성한다.
	// = 정점 배열을 가져온다.
	vertexData.pSysMem = m_vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// 정점 버퍼 생성
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

	// 필요 없어진 정점 배열, 인덱스 배열을 제거 한다.
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


	// 정점 버퍼의 단위와 오프셋을 설정한다.
	stride = sizeof(VertexType);
	offset = 0;

	// input assmbler(렌더링 파이프라인의 첫번째 단계)에 정점 버퍼를 활성화 하여 그려질 수 있게 한다.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// 입력 조립기(I/A)에 인덱스 버퍼를 활성화 하여 그려질 수 있게 한다.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// 정점 버퍼로 그릴 기본형을 설정한다. 여기선 삼각형. 부채꼴이 될 수도 있고 그렇다.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}
//control point로 부터 정점 리스트를 채운다.
void FBXModel::ParseControlPoints(FbxMesh* mesh)
{
	//메쉬의 정점 갯수를 가져온다.
	m_count = mesh->GetControlPointsCount();
	positions = new Position[m_count];
	
	for (unsigned int i = 0; i < m_count; ++i)
	{
		vec3 position;
		//제어점을 가져오려면 GetControlPointAt(int index)멤버 함수를 이용한다.
		//position.x = static_cast<float>(mesh->GetControlPointAt(i).mData[0]);
		//position.y = static_cast<float>(mesh->GetControlPointAt(i).mData[1]);
		//position.z = static_cast<float>(mesh->GetControlPointAt(i).mData[2]);
		position.x = static_cast<float>(mesh->GetControlPointAt(i).mData[0]);
		position.y = static_cast<float>(mesh->GetControlPointAt(i).mData[1]);
		position.z = static_cast<float>(mesh->GetControlPointAt(i).mData[2]);

		positions[i].position = position;
		
	}
}

void FBXModel::ShutDown()
{

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


