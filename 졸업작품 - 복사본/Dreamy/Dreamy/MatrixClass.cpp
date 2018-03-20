#include "MatrixClass.h"



MatrixClass::MatrixClass()
{
}

MatrixClass::MatrixClass(const MatrixClass& other)
{
}

MatrixClass::~MatrixClass()
{
	
}

void MatrixClass::Translation(float x, float y, float z)
{
	D3DXMatrixTranslation(&TranslationMatrix, x, y, z);
}

void MatrixClass::RotationX(float xAngle)
{
	D3DXMatrixRotationX(&RotationXMatrix, xAngle);
}

void MatrixClass::RotationY(float yAngle)
{
	D3DXMatrixRotationY(&RotationYMatrix, yAngle);
}
void MatrixClass::RotationZ(float zAngle)
{
	D3DXMatrixRotationZ(&RotationZMatrix, zAngle);
}

void MatrixClass::Scale(float x, float y, float z)
{
	D3DXMatrixScaling(&ScaleMatrix, x, y, z);
}

void MatrixClass::Multiply(D3DXMATRIX& firstMatrix, D3DXMATRIX& secondMatrix)
{
	D3DXMatrixMultiply(&FinalMatrix, &firstMatrix, &secondMatrix);
}

void MatrixClass::RotationMultiply(D3DXMATRIX& firstMatrix, D3DXMATRIX& secondMatrix)
{
	D3DXMatrixMultiply(&RotationMatrix, &firstMatrix, &secondMatrix);
}

D3DXMATRIX MatrixClass::GetTranslationMatrix()
{
	return TranslationMatrix;
}

D3DXMATRIX MatrixClass::GetRotationXMatrix()
{
	return RotationXMatrix;
}

D3DXMATRIX MatrixClass::GetRotationYMatrix()
{
	return RotationYMatrix;
}

D3DXMATRIX MatrixClass::GetRotationZMatrix()
{
	return RotationZMatrix;
}

D3DXMATRIX MatrixClass::GetScailingMatrix()
{
	return ScaleMatrix;
}

D3DXMATRIX MatrixClass::GetFinalMatrix()
{
	return FinalMatrix;
}

D3DXMATRIX MatrixClass::GetRotationMatrix()
{
	return RotationMatrix;
}