#pragma once

/*
�̸�: ParticleSystem
�뵵: 
- ��ƼŬ�� ��� ���忡 ��ġ�� ���� �ؽ�ó�� ����Ͽ� ���������.
- ���� �ؽ�ó���� �� ���� �⺻ ������ ����Ͽ� ��, ��, ���� , ��, ������ �����ֵ��� ��Ÿ���� ���� �� �������� ������ �������ȴ�.
- ���带 ����Ͽ� ��ƼŬ�� �Բ� �����Ͽ� ����ȭ �� ��ƼŬ�� �����Ǿ� ���� ������ �߰��Ѵ�.
*/

#include "D3D.h"
#include "TextureClass.h"
#include "MatrixClass.h"

class ParticleSystem : public MatrixClass
{
private:

	//��ƼŬ ���� ������
	struct ParticleType
	{
		float posX, posY, posZ;
		float red, green, blue;
		float velocity;
		bool active;
	};

	//������ �Ǵ� ��ƼŬ ������ �������
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
		D3DXVECTOR4 color;
	};

public:
	ParticleSystem();
	ParticleSystem(const ParticleSystem&);
	~ParticleSystem();

	bool Initialize(WCHAR*);
	void Shutdown();

	// Frame�Լ��� ��ƼŬ�� �׷��� �� �ְ� �� ������ ���� ���ؽ� ���۸� ������ �ϴ� ���̴�.
	bool Frame(float);
	void Render();

	ID3D11ShaderResourceView* GetTexture();
	int GetIndexCount();

public:
	bool active = false;

private:

	bool LoadTexture( WCHAR*);
	void ReleaseTexture();

	bool InitializeParticleSystem();
	void ShutdownParticleSystem();

	bool InitializeBuffers();
	void ShutdownBuffers();

	void EmitParticles(float);
	void UpdateParticles(float);
	void KillParticles();

	bool UpdateBuffers();

	void RenderBuffers();

private:

	//The following private class variables are the ones used for the particle properties.
	// They define how the particle system will work and changing each of them has a unique effect on how the particle system will react. 
	//If you plan to add more functionality to the particle system you would add it here by using additional variables for modifying the particles.
	float m_particleDeviationX, m_particleDeviationY, m_particleDeviationZ; //
	float m_particleVelocity, m_particleVelocityVariation; //�ӵ�
	float m_particleSize, m_particlesPerSecond;
	int m_maxParticles;
	float deg;

	// ��ƼŬ ������ Ÿ�̹��� ���� ������ �ð��� count�� ����Ѵ�.
	int m_currentParticleCount;
	float m_accumulatedTime;


	//We use a single texture for all the particles in this tutorial.

	TextureClass* m_Texture;

	// �� ������ ���� ���鰳�� ��ƼŬ�� ������ �Ǿ� �ϹǷ� ParticleType���� �ʿ��ϴ�.
	ParticleType* m_particleList;

	// ��ƼŬ �ý����� 1���� vertex, index ���۸� ����� �������ȴ�.
	// ���� ���۴� ������ �ɰŶ�°� �˾Ƶξ��.
	int m_vertexCount, m_indexCount;
	VertexType* m_vertices;
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;

	float time;



};

