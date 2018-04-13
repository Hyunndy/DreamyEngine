#include "../stdafx.h"
#include "Model.h"
#include "ModelScene.h"
#include "ModelMaterial.h"
#include "ModelBuffer.h"
#include "ModelUtility.h"
#include "ModelAnimation.h"
#include "ModelAnimationController.h"
#include "ModelAnimationKeyFrames.h"
#include "ModelSkeleton.h"
#include "ModelSkeletonBone.h"
#include "ModelBoneWeights.h"

wstring ModelScene::TexturePath;

ModelScene::ModelScene()
{
	// paladin_prop_j_nordstrom.fbx 파일은 매쉬와 스켈레톤을 가지고 있음
	// 나머지 fbx 파일들(예 : sword and shield idle.fbx)은 애니메이션 정보만 가지고 있음

	// SDK Manager 메모리 할당
	manager = FbxManager::Create();
	//assert(manager != NULL);

	// FbxScene 메모리 할당
	scene = FbxScene::Create(manager, "");
	//assert(scene != NULL);

	modelBuffer = new ModelBuffer();

	/*****************************************************************
	NOTE : FbxGeometryConverter 클래스
		FbxMesh, FbxNurbs, FbxPatch와 같은 기하학 노드 속성을
		변환하는 기능을 제공하며, 주로 삼각측량(Triangulation) 및
		NURBS와 Patch surface 간의 변환이라는 두가지 주요 범위에
		중점을 두고 있는 클래스.
	*****************************************************************/
	converter = new FbxGeometryConverter(manager);
	{
		//LoadScene(file, isMaterial, isSkeleton, isMesh, isAnimation);
	}

	skeleton = NULL;
	//animationController = NULL;
	animationController = new ModelAnimationController();
}

ModelScene::~ModelScene()
{
	SAFE_DELETE(converter);

	for each(ModelMaterial* temp in materials)
		SAFE_DELETE(temp);

	for each(Model* temp in models)
		SAFE_DELETE(temp);

	SAFE_DELETE(modelBuffer);


	scene->Destroy();
	manager->Destroy();
}

void ModelScene::Update()
{
	bool isAnimated = false;
	if (animationController != NULL)
	{
		isAnimated = animationController->GetAnimationCount() > 0;
		
		if (isAnimated == true)
		{
			animationController->Update();

			if (skeleton != NULL)
			{
				skeleton->UpdateAnimation(animationController);
			}

			for each(Model* model in models)
				model->UpdateAnimation(animationController);
		}
	}

	if (skeleton != NULL)
	{
		D3DXMATRIX* bonesArray = skeleton->GetSkinTransforms();
		if (bonesArray != NULL)
		{
			UINT boneCount = skeleton->GetBoneCount();
			boneCount = boneCount < ModelBoneWeights::MaxBonesPerMax ? boneCount : ModelBoneWeights::MaxBonesPerMax;

			modelBuffer->SetBoneArray(bonesArray, boneCount);
		}
	}

	modelBuffer->SetSkinning(isAnimated);
	modelBuffer->Update();

	for each(Model* model in models)
		model->Update(isAnimated);
}

void ModelScene::Render()
{
	modelBuffer->SetVSBuffer(2);

	for each(Model* model in models)
		model->Render();
}

// 
void ModelScene::LoadScene(wstring file, bool isMaterial, bool isSkeleton, bool isMesh, bool isAnimation)
{
	string tempFile = String::WStringToString(file);
	animationFileName = file;

	//SDK의 버전을 얻어온다.
	int sdkMajor, sdkMinor, sdkRevision;
	FbxManager::GetFileFormatVersion(sdkMajor, sdkMinor, sdkRevision);

	// 입출력 세팅
	/*****************************************************************
	NOTE : FbxIOSettings 클래스
		Importer나 Exporter를 만들기 이전에 생성해야하며,
		Fbx File에 있는 많은 옵션들 중 어떤 것들을 사용할지를
		설정하는 역할을 하는 클래스.
		옵션은 ReadXMLFile(), WriteXmlFile(), WriteXmlPropToFile()
		함수를 통해 XML 파일을 불러들여와 저장할 수 있다.
	*****************************************************************/
	ios = FbxIOSettings::Create(manager, IOSROOT);
	// 안에 텍스쳐가 내장되었을 수도 있다는 것을 세팅함
	ios->SetBoolProp(IMP_FBX_TEXTURE, true);

	manager->SetIOSettings(ios);

	//임포터 생성
	/*****************************************************************
	NOTE : FbxImporter 클래스
		FbxFile을 SDK 객체로 가져오는 클래스
	
	NOTE : Initialize 함수
		파일의경로명과 FbxIoSettings 값을 가져와 초기화 작업을 수행,
		초기화 됬을 경우 true를 반환하고 아닐 경우 false를 반환.

	NOTE : Import 함수
		현재 열려있는 파일을 씬으로 가져오는 작업을 수행.
		작업이 수행될 경우 true를 반환하고 아닐 경우 false를 반환.
	*****************************************************************/
	importer = FbxImporter::Create(manager, "");
	bool status = importer->Initialize(tempFile.c_str(), -1, ios);
	//assert(status == true);

	status = importer->Import(scene);
	//assert(status == true);

	ProcessScene(isMaterial, isSkeleton, isMesh, isAnimation);

	importer->Destroy();
}

void ModelScene::SetCurrentAnimation(wstring filePath)
{
	animationController->SetCurrentAnimation(filePath);
	animationController->Play();
}

void ModelScene::ProcessScene(bool isMaterial, bool isSkeleton, bool isMesh, bool isAnimation)
{

	if (isMaterial == true) ProcessMaterial();
	if (isSkeleton == true) ProcessNode(scene->GetRootNode(), FbxNodeAttribute::eSkeleton);
	if (isMesh == true) ProcessNode(scene->GetRootNode(), FbxNodeAttribute::eMesh);
	if (isAnimation == true) ProcessAnimations(animationFileName);
}

void ModelScene::ProcessMaterial()
{
	for (int i = 0; i < scene->GetMaterialCount(); i++)
	{
		/***********************************************************
		NOTE : FbxSurfaceMaterial 클래스
			메터리얼 설정을 포함하고 있는 클래스.
		***********************************************************/
		FbxSurfaceMaterial* temp = scene->GetMaterial(i);
		ModelMaterial* material = new ModelMaterial(i, temp);

		materials.push_back(material);
		fbxMaterials.push_back(temp);
	}
}

void ModelScene::ProcessNode(FbxNode * node, FbxNodeAttribute::EType attribute)
{
	/***************************************************************
	NOTE : FbxNode 클래스
		씬 그래프(Scene Graph)의 요소를 뜻하는 클래스.
		씬 그래프는 FbxNode 객체의 트리를 말한다.
		트리 관리 서비스(Tree Management Service)는	이 클래스 내에
		포함되어 있다.
		트리 관리 외에도 이 클래스는 씬에서 객체의 위치를 설명하는데
		필요한 모든 속성을 정의한다.
		이 정보에는 기본 Translation, Rotation 및 Scaling 속성과
		단단함(stiffness)과 꺽기(dampening)와 같은
		피벗(중심점), 리미트(한계점) 및 IK관절(IK joints) 속성을 위한
		고급 옵션을 제공하고 있다.
	***************************************************************/

	/***************************************************************
	NOTE : FbxNodeAttribute 클래스
		모든 유형의 노드 속성에 대한 기본 클래스.
	***************************************************************/

	FbxNodeAttribute* nodeAttribute = node->GetNodeAttribute();
	if (nodeAttribute != NULL)
	{
		// 현재 node의 AttributeType과 설정된 attribute과 같을 때
		if (nodeAttribute->GetAttributeType() == attribute)
		{
			switch (nodeAttribute->GetAttributeType())
			{
				case FbxNodeAttribute::eSkeleton:
				{
					//string test = node->GetName();
					ProcessSkeleton(node);
				}	
				break;

				case FbxNodeAttribute::eMesh:
				{
					//string test =  node->GetName();
					ProcessMesh(node);
				}	
				break;

				case FbxNodeAttribute::eMarker:
				case FbxNodeAttribute::eNurbs:
				case FbxNodeAttribute::ePatch:
				case FbxNodeAttribute::eCamera:
				case FbxNodeAttribute::eLight:
				case FbxNodeAttribute::eNull:
					break;
			}//switch
		}//if
	}//if(nodeAttribute)
	// 여기까지 자신의 Node에 대한 작업을 수행한다.

	// 자식이 존재할 경우 재귀함수를 호출한다.
	for (int i = 0; i < node->GetChildCount(); i++)
		ProcessNode(node->GetChild(i), attribute);
}

void ModelScene::ProcessMesh(FbxNode * node)
{
	FbxNodeAttribute* attribute = node->GetNodeAttribute();
	converter->Triangulate(attribute, true, true);

	/***************************************************************
	NOTE : FbxMesh 클래스
		Mesh는 다각형(Polygon)으로 만들어진 Geometry이다.
		이 클래스는 필요에 따라 n면의 다각형으로 형태를 정의할 수 있다.
		사용자는 삼각형, 사각형 및 기타 다각형을 자유롭게 혼합할 수
		있다. FBX SDK의 Mesh 관련 용어는 알려진 표준과 조금 다르므로
		다음과 같이 정의한다.

		- 제어점(Control Point)는 XYZ 좌표이며, 버택스와 같다.
		- 다각형 정점(Polygon Vertex)은 제어점에 대한 인덱스이다.
		  (동일한 제어점을 여러 다각형 정점이 참조 할 수 있음)
		- 다각형은 다각형 정점의 집합이다. 다각형을 정의하기 위한
		  최소 유효 정점 수는 3개이다.

		제어점을 초기화, 설정 및 엑세스하는 메소드는 FbxGeometryBase
		클래스에서 제공한다.
	***************************************************************/
	FbxMesh* mesh = node->GetMesh();
	//assert(mesh != NULL);

	// vertexCount에 node가 가지고 있는 제어점 수(정점 갯수)를 가져온다.
	// 단, mesh는 반드시 존재함으로 vertexCount는 1이상이어야 한다.
	int vertexCount = mesh->GetControlPointsCount();
	//assert(vertexCount > 0);

	// ModelBoneWeights를 담고 있는 벡터를 선언하고
	// 벡터의 초기값으로 vertexCount만큼 ModelBoneWeights를 넣어준다. 
	vector<ModelBoneWeights> boneWeights(vertexCount, ModelBoneWeights());
	ProcessBoneWeights(mesh, boneWeights);

	
	Model* model = new Model(String::StringToWString(node->GetName()), modelBuffer);
	
	FbxVector4* controlPoints = mesh->GetControlPoints();
	for (int i = 0; i < mesh->GetPolygonCount(); i++)
	{
		ModelMaterial* material = NULL;

		for (int vi = 0; vi < 3; vi++)
		{
			int vertexIndex = mesh->GetPolygonVertex(i, vi);
			if (vertexIndex < 0 || vertexIndex >= vertexCount)
				continue;

			if (material == NULL)
				material = LinkMaterialWithPolygon(mesh, 0, i, 0, vertexIndex);

			FbxVector4 fbxPos = controlPoints[vertexIndex];
			FbxVector4 fbxNormal;
			mesh->GetPolygonVertexNormal(i, vi, fbxNormal);
			fbxNormal.Normalize();

			D3DXVECTOR3 vecPos = ModelUtility::ToVector3(fbxPos);
			D3DXVECTOR3 vecNormal = ModelUtility::ToVector3(fbxNormal);

			D3DXVECTOR2 vecUv = GetUV(mesh, 0, i, vi, vertexIndex);
			model->AddVertex(material, vecPos, vecNormal, vecUv, boneWeights[vertexIndex]);
		}//for(vi)
	}//for(i)
	
	D3DXMATRIX temp = GetAbsoluteTransformFromCurrentTake(node, FbxTime(0));
	model->SetAbsoluteTransform(temp);
	model->SetGeometricOffset(GetGeometricOffset(node));
	model->CreateBuffer();

	models.push_back(model);
}

void ModelScene::ProcessAnimations(wstring file)
{
	//animationController = new ModelAnimationController();

	FbxNode* rootNode = scene->GetRootNode();
	if (rootNode == NULL) return;

	string test = rootNode->GetName();
	float frameRate = (float)FbxTime::GetFrameRate(scene->GetGlobalSettings().GetTimeMode());

	FbxArray<FbxString *> takeArray;
	FbxDocument* document = dynamic_cast<FbxDocument *>(scene);
	if (document != NULL)
		document->FillAnimStackNameArray(takeArray);

	int test2 = importer->GetAnimStackCount();
	int a = 0;

	for (int i = 0; i < importer->GetAnimStackCount(); i++)
	{
		int temp = importer->GetAnimStackCount();
		FbxTakeInfo* takeInfo = importer->GetTakeInfo(i);
		wstring takeName = String::StringToWString(takeInfo->mName.Buffer());
		takeName = file;
		
		FbxTime start = FbxTime(FbxLongLong(0x7fffffffffffffff));
		FbxTime stop = FbxTime(FbxLongLong(-0x7fffffffffffffff));

		FbxTimeSpan span = takeInfo->mLocalTimeSpan;
	

		double tempStart = span.GetStart().GetSecondDouble();
		double tempStop = span.GetStop().GetSecondDouble();

		if (tempStart < tempStop)
		{
			// 구동시간 동안 총 몇 프레임이 수행될지를 keyFrames에 담아줌
			int keyFrames = (int)((tempStop - tempStart) * (double)frameRate);

			ModelAnimation* animation = new ModelAnimation(takeName, keyFrames, frameRate);
			//wstring temp = animation->GetName();
			animationController->AddAnimation(animation);
				
			string test2 = rootNode->GetName();
			ProcessAnimation(rootNode, takeName, frameRate, (float)tempStart, (float)tempStop);

			animationController->SetCurrentAnimation(animation->GetName());
			animationController->Play();
		}
	}
	takeArray.Clear();

	//TODO: defualt 스택 세팅
}

void ModelScene::ProcessAnimation(FbxNode * node, wstring takeName, float frameRate, float start, float stop)
{
	FbxNodeAttribute* nodeAttribute = node->GetNodeAttribute();

	if (nodeAttribute != NULL)
	{
		if (nodeAttribute->GetAttributeType() == FbxNodeAttribute::eSkeleton)
		{
			int a = 0;
			if (skeleton != NULL)
			{
				string test = node->GetName();
				wstring temp = String::StringToWString(node->GetName());
				ModelSkeletonBone* bone = skeleton->FindBone(temp);

				if (bone != NULL)
				{
					ModelAnimationKeyFrames* animationKeyFrames = new ModelAnimationKeyFrames(takeName);

					double time = 0;
					while (time <= (double)stop)
					{
						FbxTime takeTime;
						takeTime.SetSecondDouble(time);

						D3DXMATRIX matAbsoluteTransform = GetAbsoluteTransformFromCurrentTake(node, takeTime);
						D3DXMATRIX matParentAbsoluteTransform = GetAbsoluteTransformFromCurrentTake(node->GetParent(), takeTime);
						
						D3DXMATRIX matInvParentAbsoluteTransform;
						D3DXMatrixInverse(&matInvParentAbsoluteTransform, NULL, &matParentAbsoluteTransform);
						
						D3DXMATRIX matTransform = matAbsoluteTransform * matInvParentAbsoluteTransform;
						animationKeyFrames->AddKeyFrame(matTransform);

						time += 1.0f / frameRate;
					}

					bone->AddAnimationKeyFrames(animationKeyFrames);
				}//if(bone)
			}//if(skeleton)
		}
		else if (nodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			Model* model = NULL;
			for each(Model* temp in models)
			{
				wstring nodeName = String::StringToWString(node->GetName());
				if (nodeName == temp->GetName())
				{
					model = temp;

					break;
				}
			}

			if (model != NULL)
			{
				ModelAnimationKeyFrames* animationKeyFrames = new ModelAnimationKeyFrames(takeName);

				double time = 0;
				while (time <= (double)stop)
				{
					FbxTime takeTime;
					takeTime.SetSecondDouble(time);

					D3DXMATRIX matAbsoluteTransform = GetAbsoluteTransformFromCurrentTake(node, takeTime);
					animationKeyFrames->AddKeyFrame(matAbsoluteTransform);

					time += 1.0f / frameRate;
				}

				model->AddAnimationKeyFrames(animationKeyFrames);
			}
		}//if(nodeAttribute->GetAttributeType())
	}//if(nodeAttribute)

	for (int i = 0; i < node->GetChildCount(); ++i)
		ProcessAnimation(node->GetChild(i), takeName, frameRate, start, stop);
}

void ModelScene::ProcessSkeleton(FbxNode * node)
{
	/***************************************************************
	NOTE : FbxSkeleton 클래스
		본 체인(bone chain)을 형성하는 요소를 나타내기 위해 노드
		속성을 특수화하는 클래스.

	NOTE : FindBoneIndex 함수 (ModelSkeleton 클래스의 멤버함수)
		skeletonBones의 벡터 값(Pair) 중 boneName이
		temp.first(wstring 값)과 동일할 경우 index 값을 리턴함.
	***************************************************************/
	FbxSkeleton* fbxSkeleton = node->GetSkeleton();
	if (fbxSkeleton == NULL)
		return;

	if (skeleton == NULL)
		skeleton = new ModelSkeleton();

	
	int parentBoneIndex = -1;
	FbxNode* parentNode = node->GetParent(); // 최상위 노드(부모노드)인지 확인
	
	// 부모노드가 없을 땐 해당 스켈레톤이 최상위 노드인 것을 의미함
	// 아래는 부모노드일 경우에 작업을 수행함
	if (parentNode != NULL)
	{
		wstring nodeName = String::StringToWString(parentNode->GetName());
		parentBoneIndex = skeleton->FindBoneIndex(nodeName);
	}

	wstring tempName = String::StringToWString(node->GetName());
	ModelSkeletonBone* skeletonBone = new ModelSkeletonBone(tempName, parentBoneIndex);
	skeleton->AddSkeletonBone(skeletonBone);
}

void ModelScene::ProcessBoneWeights(FbxMesh * mesh, vector<ModelBoneWeights>& meshBoneWeights)
{
	if (skeleton == NULL)
		return;

	for (int i = 0; i < mesh->GetDeformerCount(); ++i)
	{
		/************************************************************
		NOTE : FbxDeformer 클래스
			FbxMesh가 가지고있는 애니메이션 정보.
			보통 1개의 Mesh당 1개의 Deformer가 있다.
			Skin Deformer(FbxSkin) 및
			Vertex Cache Deformer(FbxVertexCacheDeformer)의
			기본 클래스이다.
			해당 Deformer 타입은 FbxDeformer::eSkin과
			FbxDeformer::eVertexCache이다.
			Deformer는 외형을 움직이기 위해 Geometry(FbxGeometry)에
			바인딩 된다.
			일반적으로 Deformer 하위의 일부 객체와 Deformer를 통해
			애니메이션이 표시된다.
			Skin Deformer(FbxSkin)에는 클러스터(FbxCluster)가 포함되어
			있다. 각 클러스터는 서로 다른 가중치(weights)를 가지고
			있어, 지오메트리 제어점의 하위집합에 영향을 미친다.
			///////////////////////////////////////////////////////
			(아래 글 수정필요)
			///////////////////////////////////////////////////////
			예를 들어 휴머노이드 형태의 매쉬는 피부(skin)을 붙일 수
			있는데, 이는 휴머노이드 매쉬가 본들(bones)에 의해
			변형되는 방식이라고 설명할 수 있다.
			///////////////////////////////////////////////////////
			(수정본)
			예를 들어 휴머노이드 모양의 메쉬에 스킨이 부착 되어 있는데,
			설명하자면 휴머노이드 메쉬가 본에 의해 변형 되는 것을
			말한다.
			///////////////////////////////////////////////////////
			본들이 움직일 때, 클러스터는 본들이 움직이기 위해
			Geometry를 작용한다.
			정점 저장 디포머(Vertex Cache Deformer)는 Cache(FbxCache)
			가 포함되어 있다.
			캐시는 모든 Geometry 제어점에 대한 애니메이션 정보를
			포함하고 있다.
		************************************************************/
		FbxDeformer* deformer = mesh->GetDeformer(i);

		if (deformer == NULL)
			continue;


		if (deformer->GetDeformerType() == FbxDeformer::eSkin)
		{
			
			FbxSkin* skin = (FbxSkin *)deformer;

			if (skin == NULL)
				continue;


			ProcessBoneWeights(skin, meshBoneWeights);
		}
	}
}

void ModelScene::ProcessBoneWeights(FbxSkin * skin, vector<ModelBoneWeights>& meshBoneWeights)
{
	/***************************************************************
	NOTE : FbxCluster 클래스 (또는 링크 클래스)
		Skin에서 얻은 joint(관절) 정보
		클러스터 또는 링크는 geometry(FbxGeometry 클래스)에서 동작하는
		개체(entity)이다. 더 정확히, 클러스터는 Geometry 제어점의
		하위집합에서 동작한다.
		(예 : 팔이 있으면 팔에 대한 정점 정보를 모아놓은 것)
	***************************************************************/
	FbxCluster::ELinkMode linkMode = FbxCluster::eNormalize;

	int clusterCount = skin->GetClusterCount();
	vector<ModelBoneWeights> skinBoneWeights(meshBoneWeights.size(), ModelBoneWeights());
	for (int i = 0; i < clusterCount; i++)
	{
		FbxCluster* cluster = skin->GetCluster(i);
		if (cluster == NULL)
			continue;


		linkMode = cluster->GetLinkMode();
		FbxNode* pLinkNode = cluster->GetLink();

		if (pLinkNode == NULL)
			continue;


		wstring lineNodeName = String::StringToWString(pLinkNode->GetName());
		int nBoneIndex = skeleton->FindBoneIndex(lineNodeName);
		if (nBoneIndex < 0)
			continue;


		ModelSkeletonBone* skeletonBone = skeleton->GetSkeletonBone(nBoneIndex);

		FbxAMatrix matClusterTransformMatrix;
		FbxAMatrix matClusterLinkTransformMatrix;
		cluster->GetTransformMatrix(matClusterTransformMatrix);
		cluster->GetTransformLinkMatrix(matClusterLinkTransformMatrix);

		//TODO: 체크
		skeletonBone->SetBindPoseTransform(ModelUtility::ToMatrix(matClusterLinkTransformMatrix));
		skeletonBone->SetBoneReferenceTransform(ModelUtility::ToMatrix(matClusterTransformMatrix));

		int* indices = cluster->GetControlPointIndices();
		double* weights = cluster->GetControlPointWeights();

		for (int j = 0; j < cluster->GetControlPointIndicesCount(); ++j)
			skinBoneWeights[indices[j]].AddBoneWeight(nBoneIndex, (float)weights[j]);
	}

	switch (linkMode)
	{
		case FbxCluster::eNormalize:
		{
			for (int i = 0; i < (int)skinBoneWeights.size(); ++i)
				skinBoneWeights[i].Normalize();
		}
		break;

		case FbxCluster::eAdditive:
			break;

		case FbxCluster::eTotalOne:
			break;
	}

	for (size_t i = 0; i < meshBoneWeights.size(); i++)
		meshBoneWeights[i].AddBoneWeights(skinBoneWeights[i]);
}

ModelMaterial * ModelScene::LinkMaterialWithPolygon(FbxMesh * mesh, int layerIndex, int polygonIndex, int polygonVertexIndex, int vertexIndex)
{
	if (layerIndex < 0 || layerIndex > mesh->GetLayerCount())
		return NULL;

	
	FbxNode* node = mesh->GetNode();
	if (node == NULL) return NULL;

	FbxLayerElementMaterial* fbxMaterial = mesh->GetLayer(layerIndex)->GetMaterials();
	if (fbxMaterial == NULL) return NULL;

	int mappingIndex = GetMappingIndex
	(
		fbxMaterial->GetMappingMode()
		, polygonIndex
		, 0
		, vertexIndex
	);
	if (mappingIndex < 0) return NULL;

	FbxLayerElement::EReferenceMode refMode = fbxMaterial->GetReferenceMode();
	if (refMode == FbxLayerElement::eDirect)
	{
		if (mappingIndex < node->GetMaterialCount())
			return GetMaterial(node->GetMaterial(mappingIndex));
	}
	else if (refMode == FbxLayerElement::eIndexToDirect)
	{
		FbxLayerElementArrayTemplate<int>& indexArr = fbxMaterial->GetIndexArray();

		if (mappingIndex < indexArr.GetCount())
		{
			int tempIndex = indexArr.GetAt(mappingIndex);

			if (tempIndex < node->GetMaterialCount())
				return GetMaterial(node->GetMaterial(tempIndex));
		}//if(mappingIndex)
	}//if(refMode)

	return NULL;
}

int ModelScene::GetMappingIndex(FbxLayerElement::EMappingMode mode, int polygonIndex, int polygonVertexIndex, int vertexIndex)
{
	switch (mode)
	{
		case FbxLayerElement::eAllSame: 
			return 0;
		case FbxLayerElement::eByControlPoint: 
			return vertexIndex;
		case FbxLayerElement::eByPolygonVertex: 
			return polygonIndex * 3 + polygonVertexIndex;
		case FbxLayerElement::eByPolygon:
			return polygonIndex;
	}
	
	return -1;
}

D3DXVECTOR2 ModelScene::GetUV(FbxMesh * mesh, int layerIndex, int polygonIndex, int polygonVertexIndex, int vertexIndex)
{
	int layerCount = mesh->GetLayerCount();
	if (layerIndex >= layerCount) return D3DXVECTOR2();

	FbxLayer* layer = mesh->GetLayer(layerIndex);
	if(layer == NULL) return D3DXVECTOR2();

	FbxLayerElementUV* uv = layer->GetUVs(FbxLayerElement::eTextureDiffuse);
	if(uv == NULL) return D3DXVECTOR2();


	FbxLayerElement::EMappingMode mappingMode = uv->GetMappingMode();
	FbxLayerElement::EReferenceMode refMode = uv->GetReferenceMode();

	const FbxLayerElementArrayTemplate<FbxVector2>& uvArray = uv->GetDirectArray();
	const FbxLayerElementArrayTemplate<int>& uvIndexArray = uv->GetIndexArray();

	int mappingIndex = 0;
	if (mappingMode == FbxLayerElement::eByControlPoint)
	{
		mappingIndex = vertexIndex;

		if (refMode == FbxLayerElement::eDirect)
		{
			if(mappingIndex < uvArray.GetCount())
				return ModelUtility::ToUv(uvArray.GetAt(mappingIndex));
		}
		else if (refMode == FbxLayerElement::eIndexToDirect)
		{
			if (mappingIndex < uvArray.GetCount())
			{
				int tempIndex = uvIndexArray.GetAt(mappingIndex);

				if(tempIndex < uvArray.GetCount())
					return ModelUtility::ToUv(uvArray.GetAt(tempIndex));
			}
		}//if(refMode)
	}
	else if (mappingMode == FbxLayerElement::eByPolygonVertex)
	{
		mappingIndex = mesh->GetTextureUVIndex
		(
			polygonIndex
			, polygonVertexIndex
			, FbxLayerElement::eTextureDiffuse
		);

		switch (refMode)
		{
			case FbxLayerElement::eDirect:
			case FbxLayerElement::eIndexToDirect:
				if (mappingIndex < uvArray.GetCount())
					return ModelUtility::ToUv(uvArray.GetAt(mappingIndex));
		}//switch(refMode)
	}//if(mappingMode)

	return D3DXVECTOR2();
}

ModelMaterial * ModelScene::GetMaterial(FbxSurfaceMaterial * fbxMaterial)
{
	for (size_t i = 0; i < fbxMaterials.size(); i++)
	{
		if (fbxMaterials[i] == fbxMaterial)
			return materials[i];
	}

	return NULL;
}

D3DXMATRIX ModelScene::GetGeometricOffset(FbxNode * node)
{
	if (node == NULL)
	{
		D3DXMATRIX matrix;
		D3DXMatrixIdentity(&matrix);

		return matrix;
	}

	FbxVector4 T = node->GetGeometricTranslation(FbxNode::eSourcePivot);
	FbxVector4 R = node->GetGeometricRotation(FbxNode::eSourcePivot);
	FbxVector4 S = node->GetGeometricScaling(FbxNode::eSourcePivot);

	FbxAMatrix offset = FbxAMatrix(T, R, S);
	/*offset.SetIdentity();
	offset.SetTRS(T, R, S);*/

	return ModelUtility::ToMatrix(offset);
}

D3DXMATRIX ModelScene::GetAbsoluteTransformFromCurrentTake(FbxNode * node, FbxTime time)
{
	if (node == NULL)
	{
		D3DXMATRIX mat;
		D3DXMatrixIdentity(&mat);

		return mat;
	}
	
	/*FbxAnimEvaluator* e = node->GetAnimationEvaluator();
	FbxAMatrix matrix = e->GetNodeGlobalTransform(node, time, FbxNode::eSourcePivot, false);*/
	FbxAMatrix matrix = node->EvaluateGlobalTransform(time);
	
	return ModelUtility::ToMatrix(matrix);
}