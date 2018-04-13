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
	// paladin_prop_j_nordstrom.fbx ������ �Ž��� ���̷����� ������ ����
	// ������ fbx ���ϵ�(�� : sword and shield idle.fbx)�� �ִϸ��̼� ������ ������ ����

	// SDK Manager �޸� �Ҵ�
	manager = FbxManager::Create();
	//assert(manager != NULL);

	// FbxScene �޸� �Ҵ�
	scene = FbxScene::Create(manager, "");
	//assert(scene != NULL);

	modelBuffer = new ModelBuffer();

	/*****************************************************************
	NOTE : FbxGeometryConverter Ŭ����
		FbxMesh, FbxNurbs, FbxPatch�� ���� ������ ��� �Ӽ���
		��ȯ�ϴ� ����� �����ϸ�, �ַ� �ﰢ����(Triangulation) ��
		NURBS�� Patch surface ���� ��ȯ�̶�� �ΰ��� �ֿ� ������
		������ �ΰ� �ִ� Ŭ����.
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

	//SDK�� ������ ���´�.
	int sdkMajor, sdkMinor, sdkRevision;
	FbxManager::GetFileFormatVersion(sdkMajor, sdkMinor, sdkRevision);

	// ����� ����
	/*****************************************************************
	NOTE : FbxIOSettings Ŭ����
		Importer�� Exporter�� ����� ������ �����ؾ��ϸ�,
		Fbx File�� �ִ� ���� �ɼǵ� �� � �͵��� ���������
		�����ϴ� ������ �ϴ� Ŭ����.
		�ɼ��� ReadXMLFile(), WriteXmlFile(), WriteXmlPropToFile()
		�Լ��� ���� XML ������ �ҷ��鿩�� ������ �� �ִ�.
	*****************************************************************/
	ios = FbxIOSettings::Create(manager, IOSROOT);
	// �ȿ� �ؽ��İ� ����Ǿ��� ���� �ִٴ� ���� ������
	ios->SetBoolProp(IMP_FBX_TEXTURE, true);

	manager->SetIOSettings(ios);

	//������ ����
	/*****************************************************************
	NOTE : FbxImporter Ŭ����
		FbxFile�� SDK ��ü�� �������� Ŭ����
	
	NOTE : Initialize �Լ�
		�����ǰ�θ�� FbxIoSettings ���� ������ �ʱ�ȭ �۾��� ����,
		�ʱ�ȭ ���� ��� true�� ��ȯ�ϰ� �ƴ� ��� false�� ��ȯ.

	NOTE : Import �Լ�
		���� �����ִ� ������ ������ �������� �۾��� ����.
		�۾��� ����� ��� true�� ��ȯ�ϰ� �ƴ� ��� false�� ��ȯ.
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
		NOTE : FbxSurfaceMaterial Ŭ����
			���͸��� ������ �����ϰ� �ִ� Ŭ����.
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
	NOTE : FbxNode Ŭ����
		�� �׷���(Scene Graph)�� ��Ҹ� ���ϴ� Ŭ����.
		�� �׷����� FbxNode ��ü�� Ʈ���� ���Ѵ�.
		Ʈ�� ���� ����(Tree Management Service)��	�� Ŭ���� ����
		���ԵǾ� �ִ�.
		Ʈ�� ���� �ܿ��� �� Ŭ������ ������ ��ü�� ��ġ�� �����ϴµ�
		�ʿ��� ��� �Ӽ��� �����Ѵ�.
		�� �������� �⺻ Translation, Rotation �� Scaling �Ӽ���
		�ܴ���(stiffness)�� ����(dampening)�� ����
		�ǹ�(�߽���), ����Ʈ(�Ѱ���) �� IK����(IK joints) �Ӽ��� ����
		��� �ɼ��� �����ϰ� �ִ�.
	***************************************************************/

	/***************************************************************
	NOTE : FbxNodeAttribute Ŭ����
		��� ������ ��� �Ӽ��� ���� �⺻ Ŭ����.
	***************************************************************/

	FbxNodeAttribute* nodeAttribute = node->GetNodeAttribute();
	if (nodeAttribute != NULL)
	{
		// ���� node�� AttributeType�� ������ attribute�� ���� ��
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
	// ������� �ڽ��� Node�� ���� �۾��� �����Ѵ�.

	// �ڽ��� ������ ��� ����Լ��� ȣ���Ѵ�.
	for (int i = 0; i < node->GetChildCount(); i++)
		ProcessNode(node->GetChild(i), attribute);
}

void ModelScene::ProcessMesh(FbxNode * node)
{
	FbxNodeAttribute* attribute = node->GetNodeAttribute();
	converter->Triangulate(attribute, true, true);

	/***************************************************************
	NOTE : FbxMesh Ŭ����
		Mesh�� �ٰ���(Polygon)���� ������� Geometry�̴�.
		�� Ŭ������ �ʿ信 ���� n���� �ٰ������� ���¸� ������ �� �ִ�.
		����ڴ� �ﰢ��, �簢�� �� ��Ÿ �ٰ����� �����Ӱ� ȥ���� ��
		�ִ�. FBX SDK�� Mesh ���� ���� �˷��� ǥ�ذ� ���� �ٸ��Ƿ�
		������ ���� �����Ѵ�.

		- ������(Control Point)�� XYZ ��ǥ�̸�, ���ý��� ����.
		- �ٰ��� ����(Polygon Vertex)�� �������� ���� �ε����̴�.
		  (������ �������� ���� �ٰ��� ������ ���� �� �� ����)
		- �ٰ����� �ٰ��� ������ �����̴�. �ٰ����� �����ϱ� ����
		  �ּ� ��ȿ ���� ���� 3���̴�.

		�������� �ʱ�ȭ, ���� �� �������ϴ� �޼ҵ�� FbxGeometryBase
		Ŭ�������� �����Ѵ�.
	***************************************************************/
	FbxMesh* mesh = node->GetMesh();
	//assert(mesh != NULL);

	// vertexCount�� node�� ������ �ִ� ������ ��(���� ����)�� �����´�.
	// ��, mesh�� �ݵ�� ���������� vertexCount�� 1�̻��̾�� �Ѵ�.
	int vertexCount = mesh->GetControlPointsCount();
	//assert(vertexCount > 0);

	// ModelBoneWeights�� ��� �ִ� ���͸� �����ϰ�
	// ������ �ʱⰪ���� vertexCount��ŭ ModelBoneWeights�� �־��ش�. 
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
			// �����ð� ���� �� �� �������� ��������� keyFrames�� �����
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

	//TODO: defualt ���� ����
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
	NOTE : FbxSkeleton Ŭ����
		�� ü��(bone chain)�� �����ϴ� ��Ҹ� ��Ÿ���� ���� ���
		�Ӽ��� Ư��ȭ�ϴ� Ŭ����.

	NOTE : FindBoneIndex �Լ� (ModelSkeleton Ŭ������ ����Լ�)
		skeletonBones�� ���� ��(Pair) �� boneName��
		temp.first(wstring ��)�� ������ ��� index ���� ������.
	***************************************************************/
	FbxSkeleton* fbxSkeleton = node->GetSkeleton();
	if (fbxSkeleton == NULL)
		return;

	if (skeleton == NULL)
		skeleton = new ModelSkeleton();

	
	int parentBoneIndex = -1;
	FbxNode* parentNode = node->GetParent(); // �ֻ��� ���(�θ���)���� Ȯ��
	
	// �θ��尡 ���� �� �ش� ���̷����� �ֻ��� ����� ���� �ǹ���
	// �Ʒ��� �θ����� ��쿡 �۾��� ������
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
		NOTE : FbxDeformer Ŭ����
			FbxMesh�� �������ִ� �ִϸ��̼� ����.
			���� 1���� Mesh�� 1���� Deformer�� �ִ�.
			Skin Deformer(FbxSkin) ��
			Vertex Cache Deformer(FbxVertexCacheDeformer)��
			�⺻ Ŭ�����̴�.
			�ش� Deformer Ÿ���� FbxDeformer::eSkin��
			FbxDeformer::eVertexCache�̴�.
			Deformer�� ������ �����̱� ���� Geometry(FbxGeometry)��
			���ε� �ȴ�.
			�Ϲ������� Deformer ������ �Ϻ� ��ü�� Deformer�� ����
			�ִϸ��̼��� ǥ�õȴ�.
			Skin Deformer(FbxSkin)���� Ŭ������(FbxCluster)�� ���ԵǾ�
			�ִ�. �� Ŭ�����ʹ� ���� �ٸ� ����ġ(weights)�� ������
			�־�, ������Ʈ�� �������� �������տ� ������ ��ģ��.
			///////////////////////////////////////////////////////
			(�Ʒ� �� �����ʿ�)
			///////////////////////////////////////////////////////
			���� ��� �޸ӳ��̵� ������ �Ž��� �Ǻ�(skin)�� ���� ��
			�ִµ�, �̴� �޸ӳ��̵� �Ž��� ����(bones)�� ����
			�����Ǵ� ����̶�� ������ �� �ִ�.
			///////////////////////////////////////////////////////
			(������)
			���� ��� �޸ӳ��̵� ����� �޽��� ��Ų�� ���� �Ǿ� �ִµ�,
			�������ڸ� �޸ӳ��̵� �޽��� ���� ���� ���� �Ǵ� ����
			���Ѵ�.
			///////////////////////////////////////////////////////
			������ ������ ��, Ŭ�����ʹ� ������ �����̱� ����
			Geometry�� �ۿ��Ѵ�.
			���� ���� ������(Vertex Cache Deformer)�� Cache(FbxCache)
			�� ���ԵǾ� �ִ�.
			ĳ�ô� ��� Geometry �������� ���� �ִϸ��̼� ������
			�����ϰ� �ִ�.
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
	NOTE : FbxCluster Ŭ���� (�Ǵ� ��ũ Ŭ����)
		Skin���� ���� joint(����) ����
		Ŭ������ �Ǵ� ��ũ�� geometry(FbxGeometry Ŭ����)���� �����ϴ�
		��ü(entity)�̴�. �� ��Ȯ��, Ŭ�����ʹ� Geometry ��������
		�������տ��� �����Ѵ�.
		(�� : ���� ������ �ȿ� ���� ���� ������ ��Ƴ��� ��)
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

		//TODO: üũ
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