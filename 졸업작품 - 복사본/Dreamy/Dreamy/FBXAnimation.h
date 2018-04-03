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

static vector<sun::VertexWithBlending>                  vertices;;     // ����
static vector<uint>                                     indices;      // �ε���
static unordered_map<sun::VertexWithBlending, uint>    indexMapping;  // ����+�ε��� ����

static sun::Position* s_rawPositions;                    // ���� ��ġ, �ִϸ��̼�
static uint s_rawPositionCount;

static FbxAMatrix s_rootMatrix;                            // STR

static sun::Skeleton s_skeleton;                         // ��(std::vector<Joint> joints;)

static bool s_hasAnimation;

static FbxTime s_AnimationStart, s_AnimationEnd;           // �ִϸ��̼� ���۰� ���� �ð�
static size_t s_AnimationLength = 1;                  // �ִϸ��̼� ����(���� - ����)
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

	//�а� �Ľ��Ѵ�.
	vec3 ParseNormal(const FbxMesh* mesh, int controlPointIndex, int vertexCounter);
	vec3 ParseBinormal(const FbxMesh* mesh, int controlPointIndex, int vertexCounter);
	vec3 ParseTangent(const FbxMesh* mesh, int controlPointIndex, int vertexCounter);
	vec2 ParseUV(const FbxMesh* mesh, int ControlPointIndex, int Textureindex);
	bool ParseMesh(FbxMesh* mesh);
	void ParseControlPoints(FbxMesh* mesh);
	void ParseAnimation(FbxNode* node);

	void InsertVertex(const unsigned int rawPositionIndex, const vec2& uv, const vec3& normal, const vec3& tangent, const vec3& binormal);

	//�ִϸ��̼� ���
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


	// ���� ���ۿ� �ε��� ����
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;

	// �� ������ ũ�� ������ ���� �ִ� ������(�̰� �̿��ؼ� ���߿� ��ĥ �� �ִ�.)
	int m_vertexCount, m_indexCount;
	//sun::VertexWithBlending* m_vertices;

	TextureClass* m_Texture;
};

