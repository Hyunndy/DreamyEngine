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
//.FBX로드를 위한 클래스
//1) FBXManager : FBX SDK용 메모리 관리자. FBX SDK 객체를 인스턴스화 할 때 마다 직접, 간접적으로 사용하고, 객체를 삭제할 때도 사용한다.
//FBX APP의 순서: FBXMANAGER 생성 후 시작 -> FBXSCENE 생성 -> SCENE객체를 사용해 대부분의 객체를 생성
//2) FBXSCENE
//3) FBX Mesh,light, animation......
//4) FbxIOSettiong : 씬의 요소를 파일에서 가져오거나 파일로 내보낼지 여부를 지정한다.
//씬의 요소: camera, light, mesh, texture, material.....
//FBXIOSettiong객체는 FbxImporter, FbxExporter객체에 전달되기 전에 인스턴스화 되고 구성되어야 한다.
//5) Importer : 씬을 가져오는 기능은 FbxImporter클래스에 의해 추상화됨.
//FbxImporter 객체는 반드시 세 개의 매개변수와 함께 초기화 함수가 호출되어야 한다.
//**FBX SDK 씬 그래프는 FbxScene클래스에 의해 추상화 된다.
//**씬은 노드 계층으로 구성된다.
//**씬의 루트 노드는 FbxScene::GetRootNode() 멤버 함수를 통해 접근한다.
//**씬의 요소(mesh, light, camera 등)는 FbxNode와 FbxNodeAttribute의 하위 클래스와 결합하여 정의된다.
//** 노드는 주로 씬 요소의 위치, 회전 및 크기를 지정하는데 사용된다.
//** Node = Transformation Data + Node Attribute
//** Node Attribute = FbxMesh, FbxLight, FbxCamera
//** Mesh = 메쉬란 다각형(polygon)으로 만들어진 지오메트리. 필요에 따라 n면의 다각형으로 형상을 정의할 수 있다. 근데 삼각형이 DirectX.
//6) FbxMesh : 정점(vertices)이라고 불리는 제어점(control point)의 리스트로 정의된다.
//FbxMesh의 싱글 인스턴스는 메모리 소비를 줄이기 위해 FbxNode의 여러 인스턴스에 바인딩 될 수 있다.
//메쉬는 노멀 맵, 재질 맵, 텍스쳐 맵 등을 정의하기 위해 레이어 및 레이어 요소(Layer element)개념을 사용한다.
//
//제어점(control point)는 XYZ좌표이며 꼭지점과 같다.
//polygon vertex는 제어점에 대한 index이다.
//polygon은 polygon vertex의 그룹이다. polygon을 정의하기 위한 최소 유효 polygon vertex의 수는 3개이다.
//7) Layer : FBX에는 메쉬의 표면을 덮는 레이어라는 개념을 사용한다.
//FBXLayer은 많은 FBXLayerElemet를 포함한다. normal, binormal, tangents, Uvs, etc....
//노말과 같은 레이어 요소는 다양한 방법으로 메쉬 표면에 매핑될 수 있다.;
//1) 제어점(control point) : eByControlPoint : 각 정점마다 하나의 매핑 좌표가 있다.
//**노말 벡터의 배열과 제어점의 배열이 주어지면 노말 벡터의 배열이 제어점의 배열로 참조되는 방법을 알 수 있다.
//**참조되는 법은 GetReferenceMode()멤버 함수로 알 수 있다.
//1-1) FbxLayerElement::eDirect : n번째 요소에 대한 매핑 정보가 FbxLayerElementTemplate::mDirectArray서의 n번째 위치에서 발견되었음을 나타냅니다.
//1-2) FbxLayerElement::eIndexToDirect : 이것은 FbxLayerElementTemplate::mIndexArray의 각 요소에 FbxLayerElementTemplate::mDirectArray의 요소를 나타내는 인덱스가 있음을 나타냅니다.
//2) 인덱스(polygonindex) : eByPolygonVertex : 각 꼭지점(정점) 마다 하나의 매핑 좌표가 있다.
//3) etc...
//Normal, Binormal, Tangents는 읽는 방법이 똑같다.
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
////정점, 인덱스 배열
//static vector<VertexType> vertices;
//static vector<unsigned int> indices;
//// 정점,인덱스를 맵핑할 맵
//static unordered_map<VertexType, unsigned int> indexMapping;
////static vector<sun::VertexWithBlending>                  s_vertices;;     // 정점
////static vector<uint>                                     s_indices;      // 인덱스
////static unordered_map<sun::VertexWithBlending, uint>    s_indexMapping;  // 정점+인덱스 맵핑
////
////static sun::Position* s_rawPositions;                    // 정점 위치, 애니메이션
////static uint s_rawPositionCount;
////
////static FbxAMatrix s_rootMatrix;                            // STR
////
////static sun::Skeleton s_skeleton;                         // 뼈(std::vector<Joint> joints;)
////
////static bool s_hasAnimation;
////
////static FbxTime s_AnimationStart, s_AnimationEnd;           // 애니메이션 시작과 종료 시간
////static size_t s_AnimationLength = 1;                  // 애니메이션 길이(종료 - 시작)
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
//	//읽고 파싱한다.
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
//	// 정점 버퍼와 인덱스 버퍼
//	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
//
//	// 각 버퍼의 크기 정보를 갖고 있는 변수들(이걸 이용해서 나중에 합칠 수 있다.)
//	int m_vertexCount, m_indexCount;
//	//sun::VertexWithBlending* m_vertices;
//
//	TextureClass* m_Texture;
//
//
//};