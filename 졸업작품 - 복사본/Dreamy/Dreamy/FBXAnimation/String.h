#pragma once

#include "stdafx_fbx.h"

using namespace std;

class String
{
public:
	static void SplitString(vector<wstring>* result, wstring origin, wstring tok);
	static void SplitFilePath(wstring fullPath, wstring* filePath = NULL, wstring* fileName = NULL);

	static bool StartsWith(wstring str, wstring comp);

	static bool Contain(wstring str, wstring comp);
	static void Replace(wstring* str, wstring comp, wstring rep);

	static wstring StringToWString(string value);
	static string WStringToString(wstring value);
};