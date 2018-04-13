#include "../stdafx.h"
#include "ModelPart.h"
#include "Model.h"
#include "ModelMaterial.h"
#include "ModelBoneWeights.h"
#include "ModelBuffer.h"

ModelPart::ModelPart(Model* model, ModelMaterial* material, ModelBuffer* modelBuffer)
	: Shader(L"./Model/Model.fx")
	, model(model), material(material), modelBuffer(modelBuffer)
	, vertexBuffer(NULL), indexBuffer(NULL)
{
	CreateInputLayout(VertexTextureNormalBlend ::desc, VertexTextureNormalBlend::count);
}

ModelPart::~ModelPart()
{
	SAFE_DELETE_ARRAY(vertex);
	SAFE_DELETE_ARRAY(index);

	SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(indexBuffer);
}

void ModelPart::Update(bool isAnimation)
{
	D3DXMATRIX world;
	if (isAnimation == true)
	{
		if (isSkinnedModel == true)
			world = model->GetGeometricOffset();
		else
			world = model->GetGeometricOffset() * model->GetAnimationTransform();
	}
	else
		world = model->GetGeometricOffset();

	worldBuffer->SetWorld(world);

	Shader::Update();
}

void ModelPart::Render()
{
	ID3D11DeviceContext* dc = D3D::GetDeviceContext();

	UINT stride = sizeof(VertexTextureNormalBlend);
	UINT offset = 0;

	dc->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	dc->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	ID3D11ShaderResourceView* diffuseView = material->GetDiffuseView();
	dc->PSSetShaderResources(0, 1, &diffuseView);
	
	Shader::Render();

	dc->DrawIndexed(indexCount, 0, 0);
}

void ModelPart::AddVertex(D3DXVECTOR3 & position, D3DXVECTOR3 & normal, D3DXVECTOR2 & uv, const ModelBoneWeights& boneWeights)
{
	positions.push_back(position);
	normals.push_back(normal);
	uvs.push_back(uv);
	indices.push_back((UINT)indices.size());

	this->boneWeights.push_back(boneWeights);
	if (boneWeights.GetBoneWeightCount() > 0)
		isSkinnedModel = true;
}

void ModelPart::CreateBuffer()
{
	vertexCount = positions.size();
	indexCount = indices.size();

	vertex = new VertexTextureNormalBlend[vertexCount];
	for (UINT i = 0; i < vertexCount; i++)
	{
		vertex[i].position = positions[i];
		vertex[i].normal = normals[i];
		vertex[i].uv = uvs[i];
	}

	for (UINT i = 0; i < boneWeights.size(); i++)
	{
		ModelBlendWeights weight = boneWeights[i].GetBlendWeights();
		vertex[i].blendIndices = weight.BlendIndices;
		vertex[i].blendWeights = weight.BlendWeights;
	}

	index = new UINT[indexCount];
	for (UINT i = 0; i < indexCount; i++)
		index[i] = indices[i];


	HRESULT hr;
	D3D11_BUFFER_DESC desc;
	D3D11_SUBRESOURCE_DATA data;


	//1. Vertex Buffer
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(VertexTextureNormalBlend) * vertexCount;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
	data.pSysMem = vertex;

	hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
	//assert(SUCCEEDED(hr));


	//2. Index Buffer
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(UINT) * indexCount;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
	data.pSysMem = index;

	hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &indexBuffer);
	//assert(SUCCEEDED(hr));
}
