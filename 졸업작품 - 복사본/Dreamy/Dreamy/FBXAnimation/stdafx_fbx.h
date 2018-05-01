#pragma once

#include <Windows.h>
//#include <assert.h>
#include <time.h>
#pragma comment(lib, "winmm.lib")

#include <string>
#include <vector>
#include <list>
#include <map>
#include <functional>
#include <fstream>
#include <algorithm>
using namespace std;

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "d3dx10.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")


#define FBXSDK_SHARED
#include <fbxsdk.h>
#pragma comment(lib, "libfbxsdk.lib")

using namespace fbxsdk;

#define SAFE_RELEASE(p) { if (p) { (p)->Release(); (p)=NULL; } }
#define SAFE_DELETE(p) { if (p) { delete (p); (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if (p) { delete[] (p); (p)=NULL; } }
#define SAFE_SHUTDOWN(p) { if(p) { (p)->Shutdown();  delete (p); (p)=0; } }


#include "String.h"
#include "VertexLayout.h"

