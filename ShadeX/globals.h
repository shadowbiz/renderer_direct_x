#ifndef SE_GLOBALSDX_H
#define SE_GLOBALSDX_H

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <d3d11.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>

#include <memory>
#include <stdlib.h>
#include <malloc.h>
#include <mmsystem.h>
#include <pdh.h>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <stdio.h>
#include <ctime>

using namespace std;
using namespace DirectX;

#pragma warning(disable : 4996)
#pragma warning(disable : 4100)
#pragma warning(disable : 4127)
#pragma warning(disable : 4290)

#define SE_ALIGN16 __declspec(align(16))
#define SE_INLINE __forceinline

#define DELTA_TIME 1.0 / 60.0

#define VEC4 DirectX::XMFLOAT4
#define VEC3 DirectX::XMFLOAT3
#define VEC2 DirectX::XMFLOAT2
#define MAT4 DirectX::XMMATRIX

#define VEC3VECTOR vector<VEC3>
#define VEC2VECTOR vector<VEC2>
#define INDEXVECTOR vector<DWORD>
#define VERTEXMAP map<VertexType, DWORD>
#define PACKEDVECTOR vector<VertexType>

#define USE_PRECOMPILED_SHADERS !_DEBUG

#define DEG2RAD(a) (a) * XM_PI / 180.0
#define RAD2DEG(a) (a) * 180.0f / XM_PI

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)  \
    {                   \
        if (p)          \
        {               \
            delete (p); \
            (p) = NULL; \
        }               \
    }
#endif
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) \
    {                        \
        if (p)               \
        {                    \
            delete[](p);     \
            (p) = NULL;      \
        }                    \
    }
#endif
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)     \
    {                       \
        if (p)              \
        {                   \
            (p)->Release(); \
            (p) = NULL;     \
        }                   \
    }
#endif
#ifndef SAFE_RELEASE_DELETE
#define SAFE_RELEASE_DELETE(p) \
    {                          \
        if (p)                 \
        {                      \
            (p)->Release();    \
            delete (p);        \
            (p) = NULL;        \
        }                      \
    }
#endif

#endif // !SE_GLOBALSDX_H
