//#include "FBXAnimation.h"
//
//static vector<sun::Ani_VertexWithBlending>                  s_vertices;;     // 정점
//static vector<uint>                                     s_indices;      // 인덱스
//static unordered_map<sun::Ani_VertexWithBlending, uint>    s_indexMapping;  // 정점+인덱스 맵핑
//
//static sun::Ani_Position* s_rawPositions;                    // 정점 위치, 애니메이션
//static uint s_rawPositionCount;
//
//static FbxAMatrix s_rootMatrix;                            // STR
//
//static sun::Skeleton s_skeleton;                         // 뼈(std::vector<Joint> joints;)
//
//static bool s_hasAnimation;
//
//static FbxTime s_AnimationStart, s_AnimationEnd;           // 애니메이션 시작과 종료 시간
//static size_t s_AnimationLength = 1;                  // 애니메이션 길이(종료 - 시작)
//static FbxAnimStack* s_animStack;
//
//
//FBXAnimation::FBXAnimation()
//{
//	m_Texture = 0;
//}
//
//
//FBXAnimation::~FBXAnimation()
//{
//}
//
//bool FBXAnimation::Initialize(ID3D11Device* device, char* modelFilename, WCHAR* textureFilename)
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
//void FBXAnimation::Render(ID3D11DeviceContext* deviceContext)
//{
//	RenderBuffers(deviceContext);
//}
//bool FBXAnimation::FBXLoad(char* modelFilename)
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
//		s_animStack = scene->GetSrcObject<FbxAnimStack>();
//
//		if (s_animStack)
//		{
//			FbxString animStackName = s_animStack->GetName();
//			FbxTakeInfo* takeInfo = scene->GetTakeInfo(animStackName);
//
//			s_AnimationStart = takeInfo->mLocalTimeSpan.GetStart();
//			s_AnimationEnd = takeInfo->mLocalTimeSpan.GetStop();
//
//			s_AnimationLength = s_AnimationEnd.GetFrameCount(FbxTime::eFrames24) - s_AnimationStart.GetFrameCount(FbxTime::eFrames24) + 1;
//		}
//
//		LoadJoint(scene->GetRootNode(), -1, -1, -1);
//
//		s_hasAnimation = s_skeleton.size() > 0 ? true : false;
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
//void FBXAnimation::LoadJoint(FbxNode* node, int depth, int index, int parentIndex)
//{
//
//	FbxNodeAttribute* nodeAttribute = node->GetNodeAttribute();
//
//	if (nodeAttribute && nodeAttribute->GetAttributeType() == FbxNodeAttribute::eSkeleton)
//	{
//		sun::Joint joint;
//		joint.parentIndex = parentIndex;
//		joint.name = node->GetName();
//
//		s_skeleton.push_back(joint);
//	}
//
//	const uint childCount = node->GetChildCount();
//
//	// 재귀
//	for (uint i = 0; i < childCount; ++i)
//		LoadJoint(node->GetChild(i), depth + 1, s_skeleton.size(), index);
//}
//
//bool FBXAnimation::LoadTexture(ID3D11Device* device, WCHAR* filename)
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
//ID3D11ShaderResourceView* FBXAnimation::GetTexture()
//{
//	return m_Texture->GetTexture();
//}
//
////재귀적으로 노드를 탐색한다.
//void FBXAnimation::LoadNode(FbxNode* node)
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
//				if (s_hasAnimation)
//					ParseAnimation(node);
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
//bool FBXAnimation::ParseMesh(FbxMesh* mesh)
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
//		    //현재 정점에 대한 위치
//			vec3& position = s_rawPositions[controlPointIndex].pos; //현재 정점에 대한 위치
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
//int FBXAnimation::GetIndexCount()
//{
//	return m_indexCount2;
//}
//vec2 FBXAnimation::ParseUV(const FbxMesh* mesh, int ControlPointIndex, int Textureindex)
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
//vec3 FBXAnimation::ParseBinormal(const FbxMesh* mesh, int controlPointIndex, int vertexCounter)
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
//vec3 FBXAnimation::ParseNormal(const FbxMesh* mesh, int controlPointIndex, int vertexCounter)
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
//vec3 FBXAnimation::ParseTangent(const FbxMesh* mesh, int controlPointIndex, int vertexCounter)
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
//void FBXAnimation::InsertVertex(const unsigned int rawPositionIndex, const vec2& uv, const vec3& normal, const vec3& tangent, const vec3& binormal)
//{
//
//	sun::Ani_VertexWithBlending vertex = { s_rawPositions[rawPositionIndex], uv, normal, tangent, binormal };
//	auto lookup = s_indexMapping.find(vertex);
//	//auto lookup = s_indexMapping.find(vertex);
//
//	if (lookup != s_indexMapping.end())
//	{
//		s_indices.push_back(lookup->second);
//
//	}
//	else
//	{
//		unsigned int index = s_vertices.size();
//		s_indexMapping[vertex] = index;
//		s_indices.push_back(index);
//		s_vertices.push_back(vertex);
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
//void FBXAnimation::ParseAnimation(FbxNode* node)
//{
//	FbxGeometry* geo = node->GetGeometry();
//
//	s_rootMatrix.SetIdentity();
//
//	const FbxVector4 T = node->GetGeometricTranslation(FbxNode::eSourcePivot);
//	const FbxVector4 R = node->GetGeometricRotation(FbxNode::eSourcePivot);
//	const FbxVector4 S = node->GetGeometricScaling(FbxNode::eSourcePivot);
//
//	s_rootMatrix = FbxAMatrix(T, R, S);
//
//	uint deformerCount = geo->GetDeformerCount();
//
//	// 보통 1개
//	for (int deformerIndex = 0; deformerIndex < deformerCount; ++deformerIndex)
//	{
//		FbxSkin* skin = reinterpret_cast<FbxSkin*>(geo->GetDeformer(deformerIndex, FbxDeformer::eSkin));
//
//		if (!skin) continue;
//
//		uint clusterCount = skin->GetClusterCount();
//
//		// 클러스터 안의 link가 joint 역할
//
//		for (uint clusterIndex = 0; clusterIndex < clusterCount; ++clusterIndex)
//		{
//			FbxCluster* cluster = skin->GetCluster(clusterIndex);
//
//			FbxAMatrix transformMatrix;
//			FbxAMatrix transformLinkMatrix;
//			FbxAMatrix globalBindposeInverseMatrix;
//
//			cluster->GetTransformMatrix(transformMatrix);
//			cluster->GetTransformLinkMatrix(transformLinkMatrix);
//
//
//			globalBindposeInverseMatrix = transformLinkMatrix.Inverse() * transformMatrix * s_rootMatrix;
//			unsigned int jointIndex;
//
//			String jointName = cluster->GetLink()->GetName();
//
//			for (uint i = 0; i < s_skeleton.size(); ++i)
//				if (s_skeleton[i].name == jointName)
//					jointIndex = i;
//
//			s_skeleton[jointIndex].globalBindPositionInverse = globalBindposeInverseMatrix;
//			s_skeleton[jointIndex].node = cluster->GetLink();
//
//			uint IndicesCount = cluster->GetControlPointIndicesCount();
//
//			for (uint i = 0; i < IndicesCount; ++i)
//			{
//				sun::BlendingIndexWeightPair blendingIndexWeightPair;
//
//				blendingIndexWeightPair.blendingIndex = jointIndex;
//				blendingIndexWeightPair.blendingWeight = (float)(cluster->GetControlPointWeights()[i]);
//				//if(s_rawPositions)
//				s_rawPositions[cluster->GetControlPointIndices()[i]].blendingInfo.push_back(blendingIndexWeightPair);
//			}
//
//			// ㄴ조인트 설정 및 각 포지션별 조인트 인덱스 설정 완료
//
//			// 애니메이션 시작
//			sun::KeyFrame** anim = &s_skeleton[jointIndex].animation;
//
//			for (FbxLongLong i = s_AnimationStart.GetFrameCount(FbxTime::eFrames24); i <= s_AnimationEnd.GetFrameCount(FbxTime::eFrames24); ++i)
//			{
//				FbxTime time;
//				time.SetFrame(i, FbxTime::eFrames24);
//				*anim = new sun::KeyFrame();
//				(*anim)->frameNum = i;
//
//				const FbxVector4 Tr = cluster->GetLink()->GetGeometricTranslation(FbxNode::eSourcePivot);
//				const FbxVector4 Rr = cluster->GetLink()->GetGeometricRotation(FbxNode::eSourcePivot);
//				const FbxVector4 Sr = cluster->GetLink()->GetGeometricScaling(FbxNode::eSourcePivot);
//
//				//s_rootMatrix = ;
//
//				FbxAMatrix currentTransformOffset = node->EvaluateGlobalTransform(time) *FbxAMatrix(Tr, Rr, Sr);
//				FbxAMatrix globalTransform = currentTransformOffset.Inverse() * cluster->GetLink()->EvaluateGlobalTransform(time);
//
//				(*anim)->globalTransform = globalTransform;
//
//				anim = &((*anim)->next);
//			}
//		}
//	}
//
//	// 4개이하 가중치 0처리
//	sun::BlendingIndexWeightPair blendingIndexWeightPair;
//	
//	blendingIndexWeightPair.blendingIndex = 0;
//	blendingIndexWeightPair.blendingWeight = 0;
//
//		for (uint rawPositionIndex = 0; rawPositionIndex < s_rawPositionCount; ++rawPositionIndex)
//		{
//			for (uint i = s_rawPositions[rawPositionIndex].blendingInfo.size(); i <= 4; ++i)
//			{
//				s_rawPositions[rawPositionIndex].blendingInfo.push_back(blendingIndexWeightPair);
//			}
//		}
//
//}
////bool FBXAnimation::UpdateBuffers(ID3D11DeviceContext* deviceContext)
//{
//	HRESULT result;
//	D3D11_MAPPED_SUBRESOURCE mappedResource;
//	sun::Ani_VertexWithBlending* verticesPtr;
//	
//	// Initialize vertex array to zeros at first.
//	memset(m_vertices, 0, (sizeof(sun::Ani_VertexWithBlending) * m_BufferCount));
//	
//	for (int i = 0; i < m_BufferCount; ++i)	   
//	{										   
//		m_vertices[i] = s_vertices[i];		   
//	}
//	
//	// Lock the vertex buffer.
//	result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
//	if (FAILED(result))
//	{
//		return false;
//	}
//	
//	// Get a pointer to the data in the vertex buffer.
//	verticesPtr = (sun::Ani_VertexWithBlending*)mappedResource.pData;
//	
//	// Copy the data into the vertex buffer.
//	memcpy(verticesPtr, (void*)m_vertices, (sizeof(sun::Ani_VertexWithBlending) * m_BufferCount));
//	
//	// Unlock the vertex buffer.
//	deviceContext->Unmap(m_vertexBuffer, 0);
//	
//	return true;
//}
//void FBXAnimation::WriteBuffers()
//{
//	w_vertexBuffer.resize(s_AnimationLength);
//
//	for (uint i = 0; i < s_AnimationLength; ++i)
//	{
//		for (uint j = 0; j < s_vertices.size(); ++j)
//		{
//			vec3 position = s_vertices[j].position.pos;
//
//			float weight1 = s_vertices[j].position.blendingInfo[0].blendingWeight;
//			float weight2 = s_vertices[j].position.blendingInfo[1].blendingWeight;
//			float weight3 = s_vertices[j].position.blendingInfo[2].blendingWeight;
//			float weight4 = s_vertices[j].position.blendingInfo[3].blendingWeight;
//
//			uint index1 = s_vertices[j].position.blendingInfo[0].blendingIndex;
//			uint index2 = s_vertices[j].position.blendingInfo[1].blendingIndex;
//			uint index3 = s_vertices[j].position.blendingInfo[2].blendingIndex;
//			uint index4 = s_vertices[j].position.blendingInfo[3].blendingIndex;
//
//			sun::KeyFrame* anim1 = s_skeleton[index1].animation;
//			sun::KeyFrame* anim2 = s_skeleton[index2].animation;
//			sun::KeyFrame* anim3 = s_skeleton[index3].animation;
//			sun::KeyFrame* anim4 = s_skeleton[index4].animation;
//
//
//			for (int time = 0; time < i; ++time)
//			{
//				anim1 = anim1->next;
//				anim2 = anim2->next;
//				anim3 = anim3->next;
//				anim4 = anim4->next;
//			}
//
//
//
//			FbxAMatrix a1 = anim1->globalTransform *  s_skeleton[index1].globalBindPositionInverse;
//			FbxAMatrix a2 = anim2->globalTransform *  s_skeleton[index2].globalBindPositionInverse;
//			FbxAMatrix a3 = anim3->globalTransform *  s_skeleton[index3].globalBindPositionInverse;
//			FbxAMatrix a4 = anim4->globalTransform *  s_skeleton[index4].globalBindPositionInverse;
//
//			FbxVector4 pos = { position.x, position.y, position.z, 1 };
//			FbxVector4 tmp = { position.x, position.y, position.z, 1 };
//
//
//			//transform.rows[0].x * x + transform.rows[0].y * y + transform.rows[0].z * z + transform.rows[0].w,
//			//transform.rows[1].x * x + transform.rows[1].y * y + transform.rows[1].z * z + transform.rows[1].w,
//			//transform.rows[2].x * x + transform.rows[2].y * y + transform.rows[2].z * z + transform.rows[2].w
//			a1 = a1.Transpose();
//			a2 = a2.Transpose();
//			a3 = a3.Transpose();
//			a4 = a4.Transpose();
//
//			float a11 = a1.Get(0, 0) * tmp[0] + a1.Get(0, 1) * tmp[1] + a1.Get(0, 2) * tmp[2] + a1.Get(0, 3);// *tmp[3];
//			float b11 = a1.Get(1, 0) * tmp[0] + a1.Get(1, 1) * tmp[1] + a1.Get(1, 2) * tmp[2] + a1.Get(1, 3);// * tmp[3];
//			float c11 = a1.Get(2, 0) * tmp[0] + a1.Get(2, 1) * tmp[1] + a1.Get(2, 2) * tmp[2] + a1.Get(2, 3);// * tmp[3];
//
//
//			float a22 = a2.Get(0, 0) * tmp[0] + a2.Get(0, 1) * tmp[1] + a2.Get(0, 2) * tmp[2] + a2.Get(0, 3);// * tmp[3];
//			float b22 = a2.Get(1, 0) * tmp[0] + a2.Get(1, 1) * tmp[1] + a2.Get(1, 2) * tmp[2] + a2.Get(1, 3);// * tmp[3];
//			float c22 = a2.Get(2, 0) * tmp[0] + a2.Get(2, 1) * tmp[1] + a2.Get(2, 2) * tmp[2] + a2.Get(2, 3);// * tmp[3];
//
//			float a33 = a3.Get(0, 0) * tmp[0] + a3.Get(0, 1) * tmp[1] + a3.Get(0, 2) * tmp[2] + a3.Get(0, 3);// * tmp[3];
//			float b33 = a3.Get(1, 0) * tmp[0] + a3.Get(1, 1) * tmp[1] + a3.Get(1, 2) * tmp[2] + a3.Get(1, 3);// * tmp[3];
//			float c33 = a3.Get(2, 0) * tmp[0] + a3.Get(2, 1) * tmp[1] + a3.Get(2, 2) * tmp[2] + a3.Get(2, 3);// * tmp[3];
//
//			float a44 = a4.Get(0, 0) * tmp[0] + a4.Get(0, 1) * tmp[1] + a4.Get(0, 2) * tmp[2] + a4.Get(0, 3);// * tmp[3];
//			float b44 = a4.Get(1, 0) * tmp[0] + a4.Get(1, 1) * tmp[1] + a4.Get(1, 2) * tmp[2] + a4.Get(1, 3);// * tmp[3];
//			float c44 = a4.Get(2, 0) * tmp[0] + a4.Get(2, 1) * tmp[1] + a4.Get(2, 2) * tmp[2] + a4.Get(2, 3);// * tmp[3];
//
//
//			if (weight1 > 0.0f)
//				pos = FbxVector4(a11, b11, c11, 1) * weight1;
//			if (weight2 > 0.0f)
//				pos = FbxVector4(a22, b22, c22, 1) * weight2;
//			if (weight3 > 0.0f)
//				pos = FbxVector4(a33, b33, c33, 1) * weight3;
//			if (weight4 > 0.0f)
//				pos = FbxVector4(a44, b44, c44, 1) * weight4;
//
//
//			vec3 result = { (float)pos[0], (float)pos[1], (float)pos[2] };
//
//			s_vertices[j].position.pos = result;
//		
//		}
//	}
//
//}
//
//bool FBXAnimation::InitializeBuffers(ID3D11Device* device)
//{
//
//	sun::Ani_VertexWithBlending* m_vertices; // 얘를 헤더파일로 보낸다.
//	unsigned int* m_indices;
//	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
//	D3D11_SUBRESOURCE_DATA vertexData, indexData;
//	HRESULT result;
//	
//	int m_BufferCount = s_vertices.size(); // 얘도 헤더파일로 따로 빼 m_bufferCount
//	m_indexCount2 = s_indices.size();
//
//	m_vertices = new sun::Ani_VertexWithBlending[m_BufferCount];
//	if (!m_vertices) { return false; }
//
//	m_indices = new unsigned int[m_indexCount2];
//	if (!m_indices) { return false; }
//
//
//	for (int i = 0; i < m_BufferCount; ++i)	   // 얘 updatebUffer로
//	{										   //
//		m_vertices[i] = s_vertices[i];		   //
//	}
//
//	for (int i = 0; i < m_indexCount2; ++i)
//	{
//		m_indices[i] = s_indices[i];
//	}
//	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT; // 버퍼가 쓰이는 방식
//	vertexBufferDesc.ByteWidth = sizeof(sun::Ani_VertexWithBlending) * m_BufferCount; //생성할 버퍼의 크기
//	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // 무슨 버퍼인지
//	vertexBufferDesc.CPUAccessFlags = 0; // cpu가 버퍼에 접근하는 방식
//	vertexBufferDesc.MiscFlags = 0;
//	vertexBufferDesc.StructureByteStride = 0;
//
//	// Set up the description of the dynamic vertex buffer.
//	//vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
//	//vertexBufferDesc.ByteWidth = sizeof(sun::Ani_VertexWithBlending) * m_BufferCount;//vertexBufferSize;
//	//vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//	//vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
//	//vertexBufferDesc.MiscFlags = 0;
//	//vertexBufferDesc.StructureByteStride = 0;
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
//	delete[] m_vertices;	//
//	m_vertices = 0;			//
//
//	delete[] m_indices;
//	m_indices = 0;
//
//	return true;
//}
//
//void FBXAnimation::RenderBuffers(ID3D11DeviceContext* deviceContext)
//{
//	unsigned int stride;
//	unsigned int offset;
//
//
//	// 정점 버퍼의 단위와 오프셋을 설정한다.
//	stride = sizeof(sun::Ani_VertexWithBlending);
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
//void FBXAnimation::ParseControlPoints(FbxMesh* mesh)
//{
//	//메쉬의 정점 갯수를 가져온다.
//	s_rawPositionCount = mesh->GetControlPointsCount();
//	s_rawPositions = new sun::Ani_Position[s_rawPositionCount];
//
//	for (unsigned int i = 0; i < m_count; ++i)
//	{
//		vec3 position;
//		position.x = static_cast<float>(mesh->GetControlPointAt(i).mData[0]);
//		position.y = static_cast<float>(mesh->GetControlPointAt(i).mData[1]);
//		position.z = static_cast<float>(mesh->GetControlPointAt(i).mData[2]);
//
//		s_rawPositions[i].pos = position;
//
//	}
//}
//
//void FBXAnimation::PlayAnimation()
//{
//	m_frame++;
//
//	if (m_frame > s_AnimationLength - 1) m_frame = 0;
//
//	SetFrame(m_frame);
//}
//
//void FBXAnimation::SetFrame(uint frame)
//{
//	if (frame > s_AnimationLength) frame = 0;
//	
//
//}
//
//void FBXAnimation::ShutDown()
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
//void FBXAnimation::ShutdownBuffers()
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
//
//
//