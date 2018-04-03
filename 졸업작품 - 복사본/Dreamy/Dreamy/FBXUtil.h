#pragma once

#include <string>
#include <unordered_map>

#include <fbxsdk.h>

#include "FBXMath.h"

typedef unsigned char byte;
typedef unsigned int uint;
typedef std::string String;

#define BIT(x) (1 << x)

using namespace sunny;

namespace math = sunny::maths;
using namespace math;

namespace sun
{
	struct KeyFrame
	{
		FbxLongLong frameNum;

		FbxAMatrix globalTransform;

		KeyFrame* next;

		KeyFrame() : next(nullptr) {};
	};

	struct Joint
	{
		String name;

		int parentIndex;

		FbxAMatrix globalBindPositionInverse;

		KeyFrame* animation;

		FbxNode* node;

		Joint() : animation(nullptr), node(nullptr), parentIndex(-1)
		{
			globalBindPositionInverse.SetIdentity();
		}

		~Joint()
		{
			while (animation)
			{
				KeyFrame* temp = animation->next;
				delete animation;
				animation = temp;
			}
		}
	};

	typedef std::vector<Joint> Skeleton;


	struct BlendingIndexWeightPair
	{
		uint blendingIndex;
		float blendingWeight;

		BlendingIndexWeightPair() : blendingIndex(0), blendingWeight(0.f)
		{}
	};

	struct Position
	{
		vec3 pos;

	//	std::vector<BlendingIndexWeightPair> blendingInfo;
	//	
	//	Position()
	//	{
	//		blendingInfo.reserve(8);
	//	}
	};

	struct VertexWithBlending
	{
		Position position;
		vec2 uv;
		vec3 normal;
		vec3 tangent;
		vec3 binormal;

		bool operator==(const VertexWithBlending& other) const
		{
			return position.pos == other.position.pos && uv == other.uv && normal == other.normal && tangent == other.tangent && binormal == other.binormal;
		}
	};

	struct Vertex
	{
		vec3 position;
		vec2 uv;
		vec3 normal;
		vec3 tangent;
		vec3 binormal;

	};
}

template<>
struct std::hash<sun::VertexWithBlending>
{
	const size_t operator()(const sun::VertexWithBlending& key) const
	{
		return key.position.pos.GetHash() ^ key.uv.GetHash() ^ key.normal.GetHash() ^ key.tangent.GetHash() ^ key.binormal.GetHash();
	}
};