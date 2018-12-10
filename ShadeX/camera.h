#ifndef SE_CAMERADX_H
#define SE_CAMERADX_H

#include "globals.h"
#include "alignedbase.h"
#include "d3d.h"

SE_ALIGN16 class Camera : public seAligned
{
  public:
	Camera();
	Camera(const Camera &);
	virtual ~Camera();

	bool Initialize(const MAT4 &projectionMatrix);
	void Release();

	void SetPosition(const VEC3 &vec);
	void SetRotation(const VEC3 &vec);

	void Translate(float x, float y, float z);
	void Rotate(float x, float y, float z);

	void SetPosition(float, float, float);
	void SetRotation(float, float, float);

	const VEC3 &GetPosition() const;
	const VEC3 &GetRotation() const;

	void Update();
	const MAT4 &GetViewMatrix() const;
	const MAT4 &GetProjectionMatrix() const;

  private:
	void UpdateMatrix();

  private:
	VEC3 m_position;
	VEC3 m_rotation;
	VEC3 m_up;
	VEC3 m_right;
	VEC3 m_front;
	VEC3 m_lookAt;
	MAT4 m_projectionMatrix;
	MAT4 m_viewMatrix;
	MAT4 m_rotationMatrix;
	bool m_hasChanged;
};

#endif