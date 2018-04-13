#pragma once

class Model;
class ModelMaterial;
class ModelBuffer;
class ModelSkeleton;
class ModelAnimationController;
class ModelBoneWeights;
class ModelScene
{
public:
	ModelScene();
	~ModelScene();

	void LoadScene(wstring file, bool isMaterial, bool isSkeleton, bool isMesh, bool isAnimation);

	void SetCurrentAnimation(wstring filePath);

	void Update();
	void Render();

	static wstring TexturePath;

private:
	void ProcessScene(bool isMaterial, bool isSkeleton, bool isMesh, bool isAnimation);
	void ProcessMaterial();
	void ProcessNode(FbxNode* node, FbxNodeAttribute::EType attribute);
	void ProcessMesh(FbxNode* node);
	void ProcessAnimation(FbxNode* node, wstring takeName, float frameRate, float start, float stop);
	void ProcessAnimations(wstring file);
	void ProcessSkeleton(FbxNode* node);

	void ProcessBoneWeights(FbxMesh* mesh, vector<ModelBoneWeights>& meshBoneWeights);
	void ProcessBoneWeights(FbxSkin* skin, vector<ModelBoneWeights>& meshBoneWeights);

	


	ModelMaterial* LinkMaterialWithPolygon
	(
		FbxMesh* mesh, int layerIndex
		, int polygonIndex, int polygonVertexIndex
		, int vertexIndex
	);

	int GetMappingIndex
	(
		FbxLayerElement::EMappingMode mode
		, int polygonIndex
		, int polygonVertexIndex
		, int vertexIndex
	);

	D3DXVECTOR2 GetUV
	(
		FbxMesh* mesh, int layerIndex
		, int polygonIndex, int polygonVertexIndex
		, int vertexIndex
	);

	ModelMaterial* GetMaterial(FbxSurfaceMaterial* fbxMaterial);
	D3DXMATRIX GetGeometricOffset(FbxNode* node);
	D3DXMATRIX GetAbsoluteTransformFromCurrentTake(FbxNode* node, FbxTime time);


	FbxManager* manager;
	FbxScene* scene;
	FbxGeometryConverter* converter;
	FbxImporter* importer;
	FbxIOSettings* ios;

	ModelSkeleton* skeleton;

	vector<Model *> models;
	vector<ModelMaterial *> materials;
	vector<FbxSurfaceMaterial *> fbxMaterials;

	ModelBuffer* modelBuffer;
	ModelAnimationController* animationController;

	wstring animationFileName;

};