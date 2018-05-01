#pragma once

/*----------------------------------------------------------------------------------------------
이름: MatrixClass
용도: Model들에 붙일 Matrix들.
----------------------------------------------------------------------------------------------*/
#include <d3d11.h>
#include <d3dx10math.h>
using namespace std;

class MatrixClass
{
public:
	MatrixClass();
	MatrixClass(const MatrixClass&);
	~MatrixClass();

	D3DXMATRIX GetTranslationMatrix();
	D3DXMATRIX GetRotationMatrix();
	D3DXMATRIX GetRotationXMatrix();
	D3DXMATRIX GetRotationYMatrix();
	D3DXMATRIX GetRotationZMatrix();
	D3DXMATRIX GetScailingMatrix();
	D3DXMATRIX GetFinalMatrix();

	//void Translation(D3DXMATRIX&, float, float, float);
	//void RotationX(D3DXMATRIX&, float);
	//void RotationY(D3DXMATRIX&, float);
	//void RotationZ(D3DXMATRIX&, float);
	//void Scale(D3DXMATRIX&, float, float, float);

	void Translation(float, float, float);
	void RotationX(float);
	void RotationY(float);
	void RotationZ(float);

	void RotationMultiply(D3DXMATRIX&, D3DXMATRIX&);
	void Scale(float, float, float);

	void Multiply(D3DXMATRIX&, D3DXMATRIX&);

	public :
	D3DXMATRIX TranslationMatrix;


	D3DXMATRIX RotationXMatrix;
	D3DXMATRIX RotationYMatrix;
	D3DXMATRIX RotationZMatrix;
	D3DXMATRIX RotationMatrix;
	D3DXMATRIX ScaleMatrix;
	D3DXMATRIX FinalMatrix;



};

