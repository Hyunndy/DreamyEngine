//#include "FBXModel.h"
//
//
//FBXModel::FBXModel()
//{
//	m_Texture = 0;
//}
//
//
//FBXModel::~FBXModel()
//{
//}
//
//bool FBXModel::Initialize(ID3D11Device* device, char* modelFilename, WCHAR* textureFilename)
//{
//	FBXLoad(modelFilename);
//
//	InitializeBuffers(device);
//
//	LoadTexture(device, textureFilename);
//
//	return true;
//}
//
//void FBXModel::Render(ID3D11DeviceContext* deviceContext)
//{
//	RenderBuffers(deviceContext);
//}
//bool FBXModel::FBXLoad(char* modelFilename)
//{
//	//FBXManager 객체 생성
//	m_SdkManager = FbxManager::Create();
//	//Scene객체 생성.
//	//사용하고 싶은 FBX파일 마다 SCENE을 생성해줘야한다.
//	FbxScene* scene = FbxScene::Create(m_SdkManager, "Scene");
//	//IOSettings 객체 생성 및 설정
//	FbxIOSettings *ios = FbxIOSettings::Create(m_SdkManager, IOSROOT);
//	m_SdkManager->SetIOSettings(ios);
//
//	//FbxImporter 객체 생성
//	m_Importer = FbxImporter::Create(m_SdkManager, "");
//
//	int format = -1;
//
//	if (!m_SdkManager->GetIOPluginRegistry()->DetectReaderFileFormat(modelFilename, format))
//		format = m_SdkManager->GetIOPluginRegistry()->FindReaderIDByDescription("FBX binary (*.fbx)");
//
//	bool result = m_Importer->Initialize(modelFilename, -1, m_SdkManager->GetIOSettings());
//	if (!result)
//	{
//		printf("djpqwjdopwqjpd");
//		exit(-1);
//	}
//
//	//씬을 로드하기 위해 씬 컨테이너(scene)를 만든다
//	if (m_Importer->Import(scene))
//	{
//
//		//파일 내용을 씬으로 채운 후에는 메모리 사용을 줄이기 위해 소멸시킨다.
//		/* Global Scene Setting.....*/
//		// 씬의 축 시스템, 주변 조명(ambient)및 시간 설정은 해당 FbxGlobalSettings 객체에 정의된다.
//
//		// 좌표축을 가져온다.
//		FbxAxisSystem sceneAxisSystem = scene->GetGlobalSettings().GetAxisSystem();
//		// 씬 내의 좌표축을 바꾼다.
//		FbxAxisSystem::MayaZUp.ConvertScene(scene);
//		FbxAxisSystem ourAxisSystem(FbxAxisSystem::eOpenGL);
//
//		//씬 내에서 삼각형화 할 수 있는 모든 노드를 삼각형화 시킨다.
//		FbxGeometryConverter geometryConverter(m_SdkManager);
//		geometryConverter.Triangulate(scene, true);
//
//		//s_animStack = scene->GetSrcObject<FbxAnimStack>();
//		//
//		//if (s_animStack)
//		//{
//		//	FbxString animStackName = s_animStack->GetName();
//		//	FbxTakeInfo* takeInfo = scene->GetTakeInfo(animStackName);
//		//
//		//	s_AnimationStart = takeInfo->mLocalTimeSpan.GetStart();
//		//	s_AnimationEnd = takeInfo->mLocalTimeSpan.GetStop();
//		//
//		//	s_AnimationLength = s_AnimationEnd.GetFrameCount(FbxTime::eFrames24) - s_AnimationStart.GetFrameCount(FbxTime::eFrames24) + 1;
//		//}
//		//
//		//LoadJoint(scene->GetRootNode(), -1, -1, -1);
//		//
//		//s_hasAnimation = s_skeleton.size() > 0 ? true : false;
//
//		//씬의 루트 노드를 가져온다.
//		//rootNode->RotationPivot;
//		//재귀적으로 노드를 탐색한다.
//		LoadNode(scene->GetRootNode());
//
//		//씬내의 다른 객체 만들기.
//		//mesh, light, animation
//		//FbxMesh* mesh = FbxMesh::Create(scene, "mesh");
//	}
//}
////void FBXModel::LoadJoint(FbxNode* node, int depth, int index, int parentIndex)
////{
////
////	FbxNodeAttribute* nodeAttribute = node->GetNodeAttribute();
////
////	if (nodeAttribute && nodeAttribute->GetAttributeType() == FbxNodeAttribute::eSkeleton)
////	{
////		sun::Joint joint;
////		joint.parentIndex = parentIndex;
////		joint.name = node->GetName();
////
////		s_skeleton.push_back(joint);
////	}
////
////	const uint childCount = node->GetChildCount();
////
////	// 재귀
////	for (uint i = 0; i < childCount; ++i)
////		LoadJoint(node->GetChild(i), depth + 1, s_skeleton.size(), index);
////}
//
//bool FBXModel::LoadTexture(ID3D11Device* device, WCHAR* filename)
//{
//	bool result;
//
//	m_Texture = new TextureClass;
//	if (!m_Texture) { return false; }
//
//	result = m_Texture->Initialize(device, filename);
//	if (!result) { return false; }
//
//	return true;
//}
//
//ID3D11ShaderResourceView* FBXModel::GetTexture()
//{
//	return m_Texture->GetTexture();
//}
//
////재귀적으로 노드를 탐색한다.
//void FBXModel::LoadNode(FbxNode* node)
//{
//
//	//노드의 속성이 무엇이냐!
//	FbxNodeAttribute* nodeAttribute = node->GetNodeAttribute();
//	if (nodeAttribute)
//	{
//		if (nodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
//		{
//			//노드의 속성이 메쉬라면
//			//메쉬에 대한 작업을 여기서 수행한다.
//			//노드의 속성에는 메쉬, 라이트, 카메라 등이 있다.
//
//			// FbxMesh로 캐스팅된 노드 속성의 포인터를 가져온다.
//			FbxMesh* mesh = node->GetMesh();
//			// 제어점으로 부터 위치 리스트를 채운다.
//			ParseControlPoints(mesh);
//
//			//	if (s_hasAnimation)
//			//		ParseAnimation(node);
//
//			ParseMesh(mesh);
//		}
//	}
//
//	const unsigned int childCount = node->GetChildCount();
//
//	// 재귀
//	for (unsigned int i = 0; i < childCount; ++i)
//		LoadNode(node->GetChild(i));
//}
//
//bool FBXModel::ParseMesh(FbxMesh* mesh)
//{
//	if (!mesh->GetNode())
//		return false;
//
//	//메쉬의 삼각형 개수를 가져온다.
//	unsigned int triCount = mesh->GetPolygonCount();
//
//	unsigned int vertexCount = 0; // 정점의 갯수
//
//	for (int i = 0; i < triCount; ++i) //삼각형 갯수
//	{
//		vec3 tanget;
//		vec3 binormal;
//		vec2 uv;
//
//		for (unsigned int j = 0; j < 3; ++j) // 삼각형은 세 개의 정점으로 구성
//		{
//			int controlPointIndex = mesh->GetPolygonVertex(i, j);; // 제어점 인덱스를 가져온다.
//																   //m_indexCount = controlPointIndex;
//
//																   //vec3& position = s_rawPositions[controlPointIndex].pos; //현재 정점에 대한 위치
//			vec3& position = positions[controlPointIndex].position; //현재 정점에 대한 위치
//			vec3 normal = ParseNormal(mesh, controlPointIndex, vertexCount);
//			vec3 tangent = ParseTangent(mesh, controlPointIndex, vertexCount);
//			vec3 binormal = ParseBinormal(mesh, controlPointIndex, vertexCount);
//			vec2 uv = ParseUV(mesh, controlPointIndex, mesh->GetTextureUVIndex(i, j));
//
//
//			uv.y = 1.0f - uv.y;
//			//position.z = position.z * -1.0f;
//			normal.z = normal.z * -1.0f;
//			normal.y = normal.y * -1.0f;
//
//			InsertVertex(controlPointIndex, uv, normal, tangent, binormal);
//
//			vertexCount++; // 정점의 갯수++
//		}
//	}
//
//	return true;
//}
//
//int FBXModel::GetIndexCount()
//{
//	return m_indexCount2;
//}
//vec2 FBXModel::ParseUV(const FbxMesh* mesh, int ControlPointIndex, int Textureindex)
//{
//	if (mesh->GetElementUVCount() < 1)
//		return vec2();
//
//	vec2 result;
//
//	const FbxGeometryElementUV* vertexUV = mesh->GetElementUV(0);
//
//	switch (vertexUV->GetMappingMode())
//	{
//	case FbxGeometryElement::eByControlPoint:
//		switch (vertexUV->GetReferenceMode())
//		{
//		case FbxGeometryElement::eDirect:
//		{
//			result.x = static_cast<float>(vertexUV->GetDirectArray().GetAt(ControlPointIndex).mData[0]);
//			result.y = static_cast<float>(vertexUV->GetDirectArray().GetAt(ControlPointIndex).mData[1]);
//		}
//		break;
//
//		case FbxGeometryElement::eIndexToDirect:
//		{
//			int index = vertexUV->GetIndexArray().GetAt(ControlPointIndex);
//			result.x = static_cast<float>(vertexUV->GetDirectArray().GetAt(index).mData[0]);
//			result.y = static_cast<float>(vertexUV->GetDirectArray().GetAt(index).mData[1]);
//		}
//		break;
//		default:
//			return vec2();
//		}
//		break;
//
//	case FbxGeometryElement::eByPolygonVertex:
//		switch (vertexUV->GetReferenceMode())
//		{
//		case FbxGeometryElement::eDirect:
//		case FbxGeometryElement::eIndexToDirect:
//		{
//			result.x = static_cast<float>(vertexUV->GetDirectArray().GetAt(Textureindex).mData[0]);
//			result.y = static_cast<float>(vertexUV->GetDirectArray().GetAt(Textureindex).mData[1]);
//		}
//		break;
//		default:
//			return vec2();
//		}
//		break;
//	}
//
//	return result;
//}
//vec3 FBXModel::ParseBinormal(const FbxMesh* mesh, int controlPointIndex, int vertexCounter)
//{
//	if (mesh->GetElementBinormalCount() < 1)
//		return vec3();
//
//	vec3 result; // 노말벡터를 저장할 벡터
//
//				 //메쉬의 노말 벡터를 획득한다.
//	const FbxGeometryElementBinormal* vertexBinormal = mesh->GetElementBinormal(0);
//
//
//	//노말 벡터가 어떻게 매핑되어있는지 알아본다.
//	switch (vertexBinormal->GetMappingMode())
//	{
//	case FbxGeometryElement::eByControlPoint:
//		//컨트롤 포인트 매핑
//		switch (vertexBinormal->GetReferenceMode())
//		{
//		case FbxGeometryElement::eDirect:
//		{
//			result.x = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(controlPointIndex).mData[0]);
//			result.y = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(controlPointIndex).mData[1]);
//			result.z = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(controlPointIndex).mData[2]);
//		}
//
//		break;
//
//		case FbxGeometryElement::eIndexToDirect:
//		{
//			int index = vertexBinormal->GetIndexArray().GetAt(controlPointIndex); //인덱스를 얻어온다.
//			result.x = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(index).mData[0]);
//			result.y = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(index).mData[1]);
//			result.z = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(index).mData[2]);
//		}
//
//		break;
//		default:
//			return vec3();
//		}
//
//		break;
//
//	case FbxGeometryElement::eByPolygonVertex:
//		//인덱스 매핑
//
//		switch (vertexBinormal->GetReferenceMode())
//		{
//		case FbxGeometryElement::eDirect:
//		{
//			result.x = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(vertexCounter).mData[0]);
//			result.y = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(vertexCounter).mData[1]);
//			result.z = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(vertexCounter).mData[2]);
//		}
//
//		break;
//
//		case FbxGeometryElement::eIndexToDirect:
//		{
//			int index = vertexBinormal->GetIndexArray().GetAt(vertexCounter);
//			// 인덱스를 얻어온다.
//			result.x = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(index).mData[0]);
//			result.y = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(index).mData[1]);
//			result.z = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(index).mData[2]);
//		}
//
//		break;
//		default:
//			return vec3();
//		}
//		break;
//
//	}
//	return result;
//}
//vec3 FBXModel::ParseNormal(const FbxMesh* mesh, int controlPointIndex, int vertexCounter)
//{
//	if (mesh->GetElementNormalCount() < 1)
//		return vec3();
//
//	vec3 Normal; // 노말벡터를 저장할 벡터
//
//				 //메쉬의 노말 벡터를 획득한다.
//	const FbxGeometryElementNormal* vertexNormal = mesh->GetElementNormal(0);
//
//
//	//노말 벡터가 어떻게 매핑되어있는지 알아본다.
//	switch (vertexNormal->GetMappingMode())
//	{
//	case FbxGeometryElement::eByControlPoint:
//		//컨트롤 포인트 매핑
//		switch (vertexNormal->GetReferenceMode())
//		{
//		case FbxGeometryElement::eDirect:
//		{
//			Normal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(controlPointIndex).mData[0]);
//			Normal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(controlPointIndex).mData[1]);
//			Normal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(controlPointIndex).mData[2]);
//		}
//
//		break;
//
//		case FbxGeometryElement::eIndexToDirect:
//		{
//			int index = vertexNormal->GetIndexArray().GetAt(controlPointIndex); //인덱스를 얻어온다.
//			Normal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[0]);
//			Normal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[1]);
//			Normal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[2]);
//		}
//
//		break;
//		default:
//			return vec3();
//		}
//
//		break;
//
//	case FbxGeometryElement::eByPolygonVertex:
//		//인덱스 매핑
//	{
//		switch (vertexNormal->GetReferenceMode())
//		{
//		case FbxGeometryElement::eDirect:
//		{
//			Normal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(vertexCounter).mData[0]);
//			Normal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(vertexCounter).mData[1]);
//			Normal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(vertexCounter).mData[2]);
//		}
//
//		break;
//
//		case FbxGeometryElement::eIndexToDirect:
//		{
//			int index = vertexNormal->GetIndexArray().GetAt(vertexCounter);
//			// 인덱스를 얻어온다.
//			Normal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[0]);
//			Normal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[1]);
//			Normal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[2]);
//		}
//
//		break;
//		default:
//			return vec3();
//		}
//		break;
//	}
//
//	}
//	return Normal;
//}
//vec3 FBXModel::ParseTangent(const FbxMesh* mesh, int controlPointIndex, int vertexCounter)
//{
//	if (mesh->GetElementTangentCount() < 1)
//		return vec3();
//
//	vec3 result; // 노말벡터를 저장할 벡터
//
//				 //메쉬의 노말 벡터를 획득한다.
//	const FbxGeometryElementTangent* vertexTangent = mesh->GetElementTangent(0);
//
//
//	//노말 벡터가 어떻게 매핑되어있는지 알아본다.
//	switch (vertexTangent->GetMappingMode())
//	{
//	case FbxGeometryElement::eByControlPoint:
//		switch (vertexTangent->GetReferenceMode())
//		{
//		case FbxGeometryElement::eDirect:
//		{
//			result.x = static_cast<float>(vertexTangent->GetDirectArray().GetAt(controlPointIndex).mData[0]);
//			result.y = static_cast<float>(vertexTangent->GetDirectArray().GetAt(controlPointIndex).mData[1]);
//			result.z = static_cast<float>(vertexTangent->GetDirectArray().GetAt(controlPointIndex).mData[2]);
//		}
//		break;
//
//		case FbxGeometryElement::eIndexToDirect:
//		{
//			int index = vertexTangent->GetIndexArray().GetAt(controlPointIndex);
//			result.x = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[0]);
//			result.y = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[1]);
//			result.z = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[2]);
//		}
//		break;
//		default:
//			return vec3();
//		}
//		break;
//
//	case FbxGeometryElement::eByPolygonVertex:
//		switch (vertexTangent->GetReferenceMode())
//		{
//		case FbxGeometryElement::eDirect:
//		{
//			result.x = static_cast<float>(vertexTangent->GetDirectArray().GetAt(vertexCounter).mData[0]);
//			result.y = static_cast<float>(vertexTangent->GetDirectArray().GetAt(vertexCounter).mData[1]);
//			result.z = static_cast<float>(vertexTangent->GetDirectArray().GetAt(vertexCounter).mData[2]);
//		}
//		break;
//
//		case FbxGeometryElement::eIndexToDirect:
//		{
//			int index = vertexTangent->GetIndexArray().GetAt(vertexCounter);
//			result.x = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[0]);
//			result.y = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[1]);
//			result.z = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[2]);
//		}
//		break;
//		default:
//			return vec3();
//		}
//		break;
//	}
//
//	return result;
//}
////InsertVertex(controlPointindex, texture, normal, tangent, binormal);
//void FBXModel::InsertVertex(const unsigned int rawPositionIndex, const vec2& uv, const vec3& normal, const vec3& tangent, const vec3& binormal)
//{
//
//	//sun::VertexWithBlending vertex = { s_rawPositions[rawPositionIndex], uv, normal, tangent, binormal };
//	VertexType vertex = { positions[rawPositionIndex], uv, normal, tangent, binormal };
//	auto lookup = indexMapping.find(vertex);
//	//auto lookup = s_indexMapping.find(vertex);
//
//	if (lookup != indexMapping.end())
//	{
//		indices.push_back(lookup->second);
//
//	}
//	else
//	{
//		unsigned int index = vertices.size();
//		indexMapping[vertex] = index;
//		indices.push_back(index);
//		vertices.push_back(vertex);
//
//
//	}
//	//if (lookup != indexMapping.end())
//	//{
//	//	s_indices.push_back(lookup->second);
//	//}
//	//else
//	//{
//	//	uint index = s_vertices.size();
//	//	s_indexMapping[vertex] = index;
//	//	s_indices.push_back(index);
//	//	s_vertices.push_back(vertex);
//	//}
//
//}
//
////void FBXModel::ParseAnimation(FbxNode* node)
////{
////	FbxGeometry* geo = node->GetGeometry();
////
////	s_rootMatrix.SetIdentity();
////
////	const FbxVector4 T = node->GetGeometricTranslation(FbxNode::eSourcePivot);
////	const FbxVector4 R = node->GetGeometricRotation(FbxNode::eSourcePivot);
////	const FbxVector4 S = node->GetGeometricScaling(FbxNode::eSourcePivot);
////
////	s_rootMatrix = FbxAMatrix(T, R, S);
////
////	uint deformerCount = geo->GetDeformerCount();
////
////	// 보통 1개
////	for (int deformerIndex = 0; deformerIndex < deformerCount; ++deformerIndex)
////	{
////		FbxSkin* skin = reinterpret_cast<FbxSkin*>(geo->GetDeformer(deformerIndex, FbxDeformer::eSkin));
////
////		if (!skin) continue;
////
////		uint clusterCount = skin->GetClusterCount();
////
////		// 클러스터 안의 link가 joint 역할
////
////		for (uint clusterIndex = 0; clusterIndex < clusterCount; ++clusterIndex)
////		{
////			FbxCluster* cluster = skin->GetCluster(clusterIndex);
////
////			FbxAMatrix transformMatrix;
////			FbxAMatrix transformLinkMatrix;
////			FbxAMatrix globalBindposeInverseMatrix;
////
////			cluster->GetTransformMatrix(transformMatrix);
////			cluster->GetTransformLinkMatrix(transformLinkMatrix);
////
////
////			globalBindposeInverseMatrix = transformLinkMatrix.Inverse() * transformMatrix * s_rootMatrix;
////			unsigned int jointIndex;
////
////			String jointName = cluster->GetLink()->GetName();
////
////			for (uint i = 0; i < s_skeleton.size(); ++i)
////				if (s_skeleton[i].name == jointName)
////					jointIndex = i;
////
////			s_skeleton[jointIndex].globalBindPositionInverse = globalBindposeInverseMatrix;
////			s_skeleton[jointIndex].node = cluster->GetLink();
////
////			uint IndicesCount = cluster->GetControlPointIndicesCount();
////
////			for (uint i = 0; i < IndicesCount; ++i)
////			{
////				sun::BlendingIndexWeightPair blendingIndexWeightPair;
////
////				blendingIndexWeightPair.blendingIndex = jointIndex;
////				blendingIndexWeightPair.blendingWeight = (float)(cluster->GetControlPointWeights()[i]);
////				s_rawPositions[cluster->GetControlPointIndices()[i]].blendingInfo.push_back(blendingIndexWeightPair);
////			}
////
////			// ㄴ조인트 설정 및 각 포지션별 조인트 인덱스 설정 완료
////
////			// 애니메이션 시작
////			sun::KeyFrame** anim = &s_skeleton[jointIndex].animation;
////
////			for (FbxLongLong i = s_AnimationStart.GetFrameCount(FbxTime::eFrames24); i <= s_AnimationEnd.GetFrameCount(FbxTime::eFrames24); ++i)
////			{
////				FbxTime time;
////				time.SetFrame(i, FbxTime::eFrames24);
////				*anim = new sun::KeyFrame();
////				(*anim)->frameNum = i;
////
////				const FbxVector4 Tr = cluster->GetLink()->GetGeometricTranslation(FbxNode::eSourcePivot);
////				const FbxVector4 Rr = cluster->GetLink()->GetGeometricRotation(FbxNode::eSourcePivot);
////				const FbxVector4 Sr = cluster->GetLink()->GetGeometricScaling(FbxNode::eSourcePivot);
////
////				//s_rootMatrix = ;
////
////				FbxAMatrix currentTransformOffset = node->EvaluateGlobalTransform(time) *FbxAMatrix(Tr, Rr, Sr);
////				FbxAMatrix globalTransform = currentTransformOffset.Inverse() * cluster->GetLink()->EvaluateGlobalTransform(time);
////
////				(*anim)->globalTransform = globalTransform;
////
////				anim = &((*anim)->next);
////			}
////		}
////	}
////
////	// 4개이하 가중치 0처리
////	sun::BlendingIndexWeightPair blendingIndexWeightPair;
////
////	blendingIndexWeightPair.blendingIndex = 0;
////	blendingIndexWeightPair.blendingWeight = 0;
////
////	for (uint rawPositionIndex = 0; rawPositionIndex < s_rawPositionCount; ++rawPositionIndex)
////	{
////		for (uint i = s_rawPositions[rawPositionIndex].blendingInfo.size(); i <= 4; ++i)
////		{
////			s_rawPositions[rawPositionIndex].blendingInfo.push_back(blendingIndexWeightPair);
////		}
////	}
////
////}
//
//
//bool FBXModel::InitializeBuffers(ID3D11Device* device)
//{
//	//sun::VertexWithBlending* m_vertices;
//	VertexType* m_vertices;
//	unsigned int* m_indices;
//	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
//	D3D11_SUBRESOURCE_DATA vertexData, indexData;
//	HRESULT result;
//	int i;
//	int m_BufferCount = vertices.size();
//	m_indexCount2 = indices.size();
//
//	m_vertices = new VertexType[m_BufferCount];
//	if (!m_vertices) { return false; }
//
//	m_indices = new unsigned int[m_indexCount2];
//	if (!m_indices) { return false; }
//
//
//	for (int i = 0; i < m_BufferCount; ++i)
//	{
//		m_vertices[i] = vertices[i];
//
//
//	}
//
//	for (int i = 0; i < m_indexCount2; ++i)
//	{
//		m_indices[i] = indices[i];
//	}
//	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT; // 버퍼가 쓰이는 방식
//	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_BufferCount; //생성할 버퍼의 크기
//	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // 무슨 버퍼인지
//	vertexBufferDesc.CPUAccessFlags = 0; // cpu가 버퍼에 접근하는 방식
//	vertexBufferDesc.MiscFlags = 0;
//	vertexBufferDesc.StructureByteStride = 0;
//
//	// 정점 데이터를 가리키는 보조 리소스 구조체를 작성한다.
//	// = 정점 배열을 가져온다.
//	vertexData.pSysMem = m_vertices;
//	vertexData.SysMemPitch = 0;
//	vertexData.SysMemSlicePitch = 0;
//
//	// 정점 버퍼 생성
//	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
//	if (FAILED(result))
//	{
//		return false;
//	}
//
//	// Set up the description of the static index buffer.
//	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
//	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount2;
//	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
//	indexBufferDesc.CPUAccessFlags = 0;
//	indexBufferDesc.MiscFlags = 0;
//	indexBufferDesc.StructureByteStride = 0;
//
//	// Give the subresource structure a pointer to the index data.
//	indexData.pSysMem = m_indices;
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
//	// 필요 없어진 정점 배열, 인덱스 배열을 제거 한다.
//	delete[] m_vertices;
//	m_vertices = 0;
//
//	delete[] m_indices;
//	m_indices = 0;
//
//	return true;
//}
//
//void FBXModel::RenderBuffers(ID3D11DeviceContext* deviceContext)
//{
//	unsigned int stride;
//	unsigned int offset;
//
//
//	// 정점 버퍼의 단위와 오프셋을 설정한다.
//	stride = sizeof(VertexType);
//	offset = 0;
//
//	// input assmbler(렌더링 파이프라인의 첫번째 단계)에 정점 버퍼를 활성화 하여 그려질 수 있게 한다.
//	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
//
//	// 입력 조립기(I/A)에 인덱스 버퍼를 활성화 하여 그려질 수 있게 한다.
//	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
//
//	// 정점 버퍼로 그릴 기본형을 설정한다. 여기선 삼각형. 부채꼴이 될 수도 있고 그렇다.
//	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//
//	return;
//}
////control point로 부터 정점 리스트를 채운다.
//void FBXModel::ParseControlPoints(FbxMesh* mesh)
//{
//	//메쉬의 정점 갯수를 가져온다.
//	m_count = mesh->GetControlPointsCount();
//	positions = new Position[m_count];
//
//	for (unsigned int i = 0; i < m_count; ++i)
//	{
//		vec3 position;
//		//제어점을 가져오려면 GetControlPointAt(int index)멤버 함수를 이용한다.
//		//position.x = static_cast<float>(mesh->GetControlPointAt(i).mData[0]);
//		//position.y = static_cast<float>(mesh->GetControlPointAt(i).mData[1]);
//		//position.z = static_cast<float>(mesh->GetControlPointAt(i).mData[2]);
//		position.x = static_cast<float>(mesh->GetControlPointAt(i).mData[0]);
//		position.y = static_cast<float>(mesh->GetControlPointAt(i).mData[1]);
//		position.z = static_cast<float>(mesh->GetControlPointAt(i).mData[2]);
//
//		positions[i].position = position;
//
//	}
//}
//
//void FBXModel::ShutDown()
//{
//	if (m_Texture)
//	{
//		m_Texture->Shutdown();
//		m_Texture = 0;
//	}
//
//	if (m_Importer)
//	{
//		m_Importer->Destroy();
//		m_Importer = nullptr;
//	}
//
//	if (m_Scene)
//	{
//		m_Scene->Destroy();
//		m_Scene = nullptr;
//	}
//
//	if (m_SdkManager)
//	{
//		m_SdkManager->Destroy();
//		m_SdkManager = nullptr;
//	}
//	ShutdownBuffers();
//}
//
//void FBXModel::ShutdownBuffers()
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
