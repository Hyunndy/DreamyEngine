#pragma once

/*
이름: ParticleSystem
용도: 
- 파티클은 대게 쿼드에 배치된 단일 텍스처를 사용하여 만들어진다.
- 단일 텍스처들은 몇 가지 기본 물리를 사용하여 눈, 비, 연기 , 불, 나뭇잎 같은애들을 나타내기 위해 각 프레임의 수백배로 렌더링된다.
- 블렌드를 사용하여 파티클을 함께 블렌드하여 계층화 된 파티클이 누적되어 서로 색상을 추가한다.
*/

#include <d3d11.h>
#include <D3DX10math.h>

#include "TextureClass.h"
#include "MatrixClass.h"

class ParticleSystem : public MatrixClass
{
private:

	//파티클 정의 변수들
	struct ParticleType
	{
		float posX, posY, posZ;
		float red, green, blue;
		float velocity;
		bool active;
	};

	//렌더링 되는 파티클 각각의 정점요소
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

	bool Initialize(ID3D11Device*, WCHAR*);
	void Shutdown();

	// Frame함수는 파티클이 그려질 수 있게 매 프레임 마다 버텍스 버퍼를 리빌딩 하는 것이다.
	bool Frame(float, ID3D11DeviceContext*);
	void Render(ID3D11DeviceContext*);

	ID3D11ShaderResourceView* GetTexture();
	int GetIndexCount();

	bool active;

private:

	bool LoadTexture(ID3D11Device*, WCHAR*);
	void ReleaseTexture();

	bool InitializeParticleSystem();
	void ShutdownParticleSystem();

	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();

	void EmitParticles(float);
	void UpdateParticles(float);
	void KillParticles();

	bool UpdateBuffers(ID3D11DeviceContext*);

	void RenderBuffers(ID3D11DeviceContext*);

private:

	//The following private class variables are the ones used for the particle properties.
	// They define how the particle system will work and changing each of them has a unique effect on how the particle system will react. 
	//If you plan to add more functionality to the particle system you would add it here by using additional variables for modifying the particles.
	float m_particleDeviationX, m_particleDeviationY, m_particleDeviationZ; //
	float m_particleVelocity, m_particleVelocityVariation; //속도
	float m_particleSize, m_particlesPerSecond;
	int m_maxParticles;

	// 파티클 방출의 타이밍을 위해 축적된 시간과 count를 기록한다.
	int m_currentParticleCount;
	float m_accumulatedTime;


	//We use a single texture for all the particles in this tutorial.

	TextureClass* m_Texture;

	// 한 프레임 마다 수백개의 파티클이 렌더링 되야 하므로 ParticleType들이 필요하다.
	ParticleType* m_particleList;

	// 파티클 시스템은 1개의 vertex, index 버퍼를 사용해 렌더링된다.
	// 정점 버퍼는 동적이 될거라는걸 알아두어라.
	int m_vertexCount, m_indexCount;
	VertexType* m_vertices;
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;

	float time;



};

