#pragma once

#include <D3D11.h>
#include <D3DX10math.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

#include <fbxsdk.h>

#include "FBXMath.h"
#include "TextureClass.h"
#include "FBXUtil.h"

using namespace std;
using namespace sunny;

namespace math = sunny::maths;
using namespace math;

static vector<sun::VertexWithBlending>                  vertices;;     // 정점
static vector<uint>                                     indices;      // 인덱스
static unordered_map<sun::VertexWithBlending, uint>    indexMapping;  // 정점+인덱스 맵핑

static sun::Position* s_rawPositions;                    // 정점 위치, 애니메이션
static uint s_rawPositionCount;

static FbxAMatrix s_rootMatrix;                            // STR

static sun::Skeleton s_skeleton;                         // 뼈(std::vector<Joint> joints;)

static bool s_hasAnimation;

static FbxTime s_AnimationStart, s_AnimationEnd;           // 애니메이션 시작과 종료 시간
static size_t s_AnimationLength = 1;                  // 애니메이션 길이(종료 - 시작)
static FbxAnimStack* s_animStack;


class FBXAnimation
{
public:
	FBXAnimation();
	~FBXAnimation();


	bool Initialize(ID3D11Device* device, char*, WCHAR*);
	void Render(ID3D11DeviceContext*);

	bool FBXLoad(char*);

	void LoadJoint(FbxNode* node, int depth, int index, int parentIndex);
	void LoadNode(FbxNode* node);

	void ShutDown();

	bool InitializeBuffers(ID3D11Device* device);

	//읽고 파싱한다.
	vec3 ParseNormal(const FbxMesh* mesh, int controlPointIndex, int vertexCounter);
	vec3 ParseBinormal(const FbxMesh* mesh, int controlPointIndex, int vertexCounter);
	vec3 ParseTangent(const FbxMesh* mesh, int controlPointIndex, int vertexCounter);
	vec2 ParseUV(const FbxMesh* mesh, int ControlPointIndex, int Textureindex);
	bool ParseMesh(FbxMesh* mesh);
	void ParseControlPoints(FbxMesh* mesh);
	void ParseAnimation(FbxNode* node);

	void InsertVertex(const unsigned int rawPositionIndex, const vec2& uv, const vec3& normal, const vec3& tangent, const vec3& binormal);

	//애니메이션 재생
	void PlayAnimation();
	uint m_frame;
	void SetFrame(uint frame);

	//bool InsertVertex(ID3D11Device*);

	void RenderBuffers(ID3D11DeviceContext* deviceContext);
	void ShutdownBuffers();
	int GetIndexCount();

	bool LoadTexture(ID3D11Device* device, WCHAR* filename);
	ID3D11ShaderResourceView* GetTexture();


private:
	FbxManager* m_SdkManager = nullptr;
	FbxImporter* m_Importer = nullptr;
	FbxScene* m_Scene = nullptr;

	sun::Position* positions;
	sun::VertexWithBlending* m_Model;


	int m_count;
	int lastcount = 0;
	int m_indexCount2;


	// 정점 버퍼와 인덱스 버퍼
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;

	// 각 버퍼의 크기 정보를 갖고 있는 변수들(이걸 이용해서 나중에 합칠 수 있다.)
	int m_vertexCount, m_indexCount;
	//sun::VertexWithBlending* m_vertices;

	TextureClass* m_Texture;
};

