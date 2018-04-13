//#pragma once
//
//#include <D3D11.h>
//#include <D3DX10math.h>
//#include <iostream>
//#include <string>
//#include <vector>
//#include <unordered_map>
//
//#include <fbxsdk.h>
//
////#include <unordered_map>
//
////#include "FBXMath.h"
//#include "TextureClass.h"
////#include "FBXUtil.h"
//
//using namespace std;
////using namespace sunny;
////using namespace maths;
///*
//.FBX�ε带 ���� Ŭ����
//1) FBXManager : FBX SDK�� �޸� ������. FBX SDK ��ü�� �ν��Ͻ�ȭ �� �� ���� ����, ���������� ����ϰ�, ��ü�� ������ ���� ����Ѵ�.
//FBX APP�� ����: FBXMANAGER ���� �� ���� -> FBXSCENE ���� -> SCENE��ü�� ����� ��κ��� ��ü�� ����
//2) FBXSCENE
//3) FBX Mesh,light, animation......
//4) FbxIOSettiong : ���� ��Ҹ� ���Ͽ��� �������ų� ���Ϸ� �������� ���θ� �����Ѵ�.
//���� ���: camera, light, mesh, texture, material.....
//FBXIOSettiong��ü�� FbxImporter, FbxExporter��ü�� ���޵Ǳ� ���� �ν��Ͻ�ȭ �ǰ� �����Ǿ�� �Ѵ�.
//5) Importer : ���� �������� ����� FbxImporterŬ������ ���� �߻�ȭ��.
//FbxImporter ��ü�� �ݵ�� �� ���� �Ű������� �Բ� �ʱ�ȭ �Լ��� ȣ��Ǿ�� �Ѵ�.
//**FBX SDK �� �׷����� FbxSceneŬ������ ���� �߻�ȭ �ȴ�.
//**���� ��� �������� �����ȴ�.
//**���� ��Ʈ ���� FbxScene::GetRootNode() ��� �Լ��� ���� �����Ѵ�.
//**���� ���(mesh, light, camera ��)�� FbxNode�� FbxNodeAttribute�� ���� Ŭ������ �����Ͽ� ���ǵȴ�.
//** ���� �ַ� �� ����� ��ġ, ȸ�� �� ũ�⸦ �����ϴµ� ���ȴ�.
//** Node = Transformation Data + Node Attribute
//** Node Attribute = FbxMesh, FbxLight, FbxCamera
//** Mesh = �޽��� �ٰ���(polygon)���� ������� ������Ʈ��. �ʿ信 ���� n���� �ٰ������� ������ ������ �� �ִ�. �ٵ� �ﰢ���� DirectX.
//6) FbxMesh : ����(vertices)�̶�� �Ҹ��� ������(control point)�� ����Ʈ�� ���ǵȴ�.
//FbxMesh�� �̱� �ν��Ͻ��� �޸� �Һ� ���̱� ���� FbxNode�� ���� �ν��Ͻ��� ���ε� �� �� �ִ�.
//�޽��� ��� ��, ���� ��, �ؽ��� �� ���� �����ϱ� ���� ���̾� �� ���̾� ���(Layer element)������ ����Ѵ�.
//
//������(control point)�� XYZ��ǥ�̸� �������� ����.
//polygon vertex�� �������� ���� index�̴�.
//polygon�� polygon vertex�� �׷��̴�. polygon�� �����ϱ� ���� �ּ� ��ȿ polygon vertex�� ���� 3���̴�.
//7) Layer : FBX���� �޽��� ǥ���� ���� ���̾��� ������ ����Ѵ�.
//FBXLayer�� ���� FBXLayerElemet�� �����Ѵ�. normal, binormal, tangents, Uvs, etc....
//�븻�� ���� ���̾� ��Ҵ� �پ��� ������� �޽� ǥ�鿡 ���ε� �� �ִ�.;
//1) ������(control point) : eByControlPoint : �� �������� �ϳ��� ���� ��ǥ�� �ִ�.
//**�븻 ������ �迭�� �������� �迭�� �־����� �븻 ������ �迭�� �������� �迭�� �����Ǵ� ����� �� �� �ִ�.
//**�����Ǵ� ���� GetReferenceMode()��� �Լ��� �� �� �ִ�.
//1-1) FbxLayerElement::eDirect : n��° ��ҿ� ���� ���� ������ FbxLayerElementTemplate::mDirectArray���� n��° ��ġ���� �߰ߵǾ����� ��Ÿ���ϴ�.
//1-2) FbxLayerElement::eIndexToDirect : �̰��� FbxLayerElementTemplate::mIndexArray�� �� ��ҿ� FbxLayerElementTemplate::mDirectArray�� ��Ҹ� ��Ÿ���� �ε����� ������ ��Ÿ���ϴ�.
//2) �ε���(polygonindex) : eByPolygonVertex : �� ������(����) ���� �ϳ��� ���� ��ǥ�� �ִ�.
//3) etc...
//Normal, Binormal, Tangents�� �д� ����� �Ȱ���.
//
//*/
//struct vec4
//{
//	float x, y, z, w;
//
//	vec4()
//		: x(0.0f), y(0.0f), z(0.0f), w(0.0f)
//	{}
//	vec4(float x, float y, float z, float w)
//		: x(x), y(y), z(z), w(w)
//	{}
//
//	bool operator==(const vec4& other) const
//	{
//		return x == other.x && y == other.y && z == other.z && w == other.w;
//	}
//
//	bool operator!=(const vec4& other) const
//	{
//		return !(*this == other);
//	}
//
//	unsigned int GetHash() const
//	{
//		return (*(unsigned int*)&x) ^ ((*(unsigned int*)&y) << 14) ^ ((*(unsigned int*)&z) << 23) ^ ((*(unsigned int*)&w) << 31);
//	}
//};
//
//struct vec3
//{
//	float x, y, z;
//
//	vec3()
//		: x(0.0f), y(0.0f), z(0.0f)
//	{}
//	vec3(float x, float y, float z)
//		: x(x), y(y), z(z)
//	{}
//
//	bool operator==(const vec3& other) const
//	{
//		return x == other.x && y == other.y && z == other.z;
//	}
//
//	bool operator!=(const vec3& other) const
//	{
//		return !(*this == other);
//	}
//
//	unsigned int GetHash() const
//	{
//		return (*(unsigned int*)&x) ^ ((*(unsigned int*)&y) << 14) ^ ((*(unsigned int*)&z) << 23);
//	}
//
//};
//
//struct vec2
//{
//	float x, y;
//
//	vec2()
//		: x(0.0f), y(0.0f)
//	{}
//	vec2(float x, float y)
//		: x(x), y(y)
//	{}
//
//	bool operator==(const vec2& other) const
//	{
//		return x == other.x && y == other.y;
//	}
//
//	bool operator!=(const vec2& other) const
//	{
//		return !(*this == other);
//	}
//
//	unsigned int GetHash() const
//	{
//		return (*(unsigned int*)&x) ^ ((*(unsigned int*)&y) << 14);
//	}
//
//};
//
//struct Position
//{
//	vec3 position;
//};
//struct VertexType
//{
//	Position position;
//	vec2 texture;
//	vec3 normal;
//	vec3 tangent;
//	vec3 binormal;
//
//
//	bool operator==(const VertexType& other)const
//	{
//		return position.position == other.position.position && texture == other.texture && normal == other.normal && tangent == other.tangent && binormal == other.binormal;
//	}
//
//};
//
//struct ModelType
//{
//	float x, y, z;
//	float tu, tv;
//	float nx, ny, nz;
//	float tx, ty, tz;
//	float bx, by, bz;
//};
//
//
//template<>
//struct std::hash<VertexType>
//{
//	const size_t operator()(const VertexType& key)const
//	{
//		return key.position.position.GetHash() ^ key.texture.GetHash() ^ key.normal.GetHash() ^ key.tangent.GetHash() ^ key.binormal.GetHash();
//	}
//};
//
////����, �ε��� �迭
//static vector<VertexType> vertices;
//static vector<unsigned int> indices;
//// ����,�ε����� ������ ��
//static unordered_map<VertexType, unsigned int> indexMapping;
////static vector<sun::VertexWithBlending>                  s_vertices;;     // ����
////static vector<uint>                                     s_indices;      // �ε���
////static unordered_map<sun::VertexWithBlending, uint>    s_indexMapping;  // ����+�ε��� ����
////
////static sun::Position* s_rawPositions;                    // ���� ��ġ, �ִϸ��̼�
////static uint s_rawPositionCount;
////
////static FbxAMatrix s_rootMatrix;                            // STR
////
////static sun::Skeleton s_skeleton;                         // ��(std::vector<Joint> joints;)
////
////static bool s_hasAnimation;
////
////static FbxTime s_AnimationStart, s_AnimationEnd;           // �ִϸ��̼� ���۰� ���� �ð�
////static size_t s_AnimationLength = 1;                  // �ִϸ��̼� ����(���� - ����)
////static FbxAnimStack* s_animStack;
////
//
//
//class FBXModel
//{
//private:
//
//public:
//	FBXModel();
//	~FBXModel();
//
//	bool Initialize(ID3D11Device* device, char*, WCHAR*);
//	void Render(ID3D11DeviceContext*);
//
//	bool FBXLoad(char*);
//
//	void LoadJoint(FbxNode* node, int depth, int index, int parentIndex);
//	void LoadNode(FbxNode* node);
//
//	void ShutDown();
//
//	bool InitializeBuffers(ID3D11Device* device);
//
//	//�а� �Ľ��Ѵ�.
//	vec3 ParseNormal(const FbxMesh* mesh, int controlPointIndex, int vertexCounter);
//	vec3 ParseBinormal(const FbxMesh* mesh, int controlPointIndex, int vertexCounter);
//	vec3 ParseTangent(const FbxMesh* mesh, int controlPointIndex, int vertexCounter);
//	vec2 ParseUV(const FbxMesh* mesh, int ControlPointIndex, int Textureindex);
//	bool ParseMesh(FbxMesh* mesh);
//	void ParseControlPoints(FbxMesh* mesh);
//	void ParseAnimation(FbxNode* node);
//
//	void InsertVertex(const unsigned int rawPositionIndex, const vec2& uv, const vec3& normal, const vec3& tangent, const vec3& binormal);
//
//
//	//bool InsertVertex(ID3D11Device*);
//
//	void RenderBuffers(ID3D11DeviceContext* deviceContext);
//	void ShutdownBuffers();
//	int GetIndexCount();
//
//	bool LoadTexture(ID3D11Device* device, WCHAR* filename);
//	ID3D11ShaderResourceView* GetTexture();
//
//
//private:
//	FbxManager* m_SdkManager = nullptr;
//	FbxImporter* m_Importer = nullptr;
//	FbxScene* m_Scene = nullptr;
//
//	Position* positions;
//	VertexType* m_Model;
//
//
//	int m_count;
//	int lastcount = 0;
//	int m_indexCount2;
//
//
//	// ���� ���ۿ� �ε��� ����
//	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
//
//	// �� ������ ũ�� ������ ���� �ִ� ������(�̰� �̿��ؼ� ���߿� ��ĥ �� �ִ�.)
//	int m_vertexCount, m_indexCount;
//	//sun::VertexWithBlending* m_vertices;
//
//	TextureClass* m_Texture;
//
//
//};