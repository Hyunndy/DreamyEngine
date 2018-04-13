#pragma once
#include "../stdafx.h"

class ModelUtility
{
public:
	static D3DXVECTOR2 ToVector2(const FbxVector2& vec)
	{
		return D3DXVECTOR2((float)vec.mData[0], (float)vec.mData[1]);
	}

	static D3DXVECTOR3 ToVector3(const FbxVector4& vec)
	{
		return D3DXVECTOR3((float)vec.mData[0], (float)vec.mData[1], (float)vec.mData[2]);
	}

	static D3DXCOLOR ToColor(const FbxVector4& vec)
	{
		return D3DXCOLOR((float)vec.mData[0], (float)vec.mData[1], (float)vec.mData[2], 1);
	}

	static D3DXVECTOR2 ToUv(const FbxVector2& vec)
	{
		D3DXVECTOR2 temp = ToVector2(vec);

		return D3DXVECTOR2(temp.x, 1.0f - temp.y);
	}

	static D3DXMATRIX ToMatrix(const FbxAMatrix &matrix)
	{
		FbxVector4 r1 = matrix.GetRow(0);
		FbxVector4 r2 = matrix.GetRow(1);
		FbxVector4 r3 = matrix.GetRow(2);
		FbxVector4 r4 = matrix.GetRow(3);

		return D3DXMATRIX
		(
			(float)r1.mData[0], (float)r1.mData[1], (float)r1.mData[2], (float)r1.mData[3],
			(float)r2.mData[0], (float)r2.mData[1], (float)r2.mData[2], (float)r2.mData[3],
			(float)r3.mData[0], (float)r3.mData[1], (float)r3.mData[2], (float)r3.mData[3],
			(float)r4.mData[0], (float)r4.mData[1], (float)r4.mData[2], (float)r4.mData[3]
		);
	}
};
