#ifndef SE_MATHDX_H
#define SE_MATHDX_H

#include "globals.h"
#include "alignedbase.h"
SE_ALIGN16 class seMath : public seAligned
{
  public:
	SE_ALIGN16 struct Frustum : public seAligned
	{
		VEC3 Origin;
		VEC4 Orientation;

		FLOAT RightSlope;
		FLOAT LeftSlope;
		FLOAT TopSlope;
		FLOAT BottomSlope;
		FLOAT Near, Far;
	};

	SE_INLINE static void ComputeFrustumFromProjection(Frustum *pOut, const XMMATRIX &pProjection)
	{
		static XMVECTOR HomogenousPoints[6] = {
			{1.0f, 0.0f, 1.0f, 1.0f},  // right (at far plane)
			{-1.0f, 0.0f, 1.0f, 1.0f}, // left
			{0.0f, 1.0f, 1.0f, 1.0f},  // top
			{0.0f, -1.0f, 1.0f, 1.0f}, // bottom

			{0.0f, 0.0f, 0.0f, 1.0f}, // near
			{0.0f, 0.0f, 1.0f, 1.0f}  // far
		};

		XMVECTOR Determinant;
		XMMATRIX matInverse = XMMatrixInverse(&Determinant, pProjection);

		XMVECTOR Points[6];

		for (INT i = 0; i < 6; i++)
		{
			Points[i] = XMVector4Transform(HomogenousPoints[i], matInverse); // Transform point.
		}

		pOut->Origin = XMFLOAT3(0.0f, 0.0f, 0.0f);
		pOut->Orientation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

		Points[0] = Points[0] * XMVectorReciprocal(XMVectorSplatZ(Points[0]));
		Points[1] = Points[1] * XMVectorReciprocal(XMVectorSplatZ(Points[1]));
		Points[2] = Points[2] * XMVectorReciprocal(XMVectorSplatZ(Points[2]));
		Points[3] = Points[3] * XMVectorReciprocal(XMVectorSplatZ(Points[3]));

		pOut->RightSlope = XMVectorGetX(Points[0]);
		pOut->LeftSlope = XMVectorGetX(Points[1]);
		pOut->TopSlope = XMVectorGetY(Points[2]);
		pOut->BottomSlope = XMVectorGetY(Points[3]);

		Points[4] = Points[4] * XMVectorReciprocal(XMVectorSplatW(Points[4]));
		Points[5] = Points[5] * XMVectorReciprocal(XMVectorSplatW(Points[5]));

		pOut->Near = XMVectorGetZ(Points[4]);
		pOut->Far = XMVectorGetZ(Points[5]);
	}
};

#endif // !SE_MATHDX_H
