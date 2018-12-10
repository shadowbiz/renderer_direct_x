#ifndef SE_LIGHTDX_H
#define SE_LIGHTDX_H

#include "globals.h"
#include "alignedbase.h"
#include "camera.h"
#include "math.h"

SE_ALIGN16 class Light : public seAligned
{
  public:
	Light();
	Light(const Light &other);
	~Light();

	void SetAmbientColor(float r, float g, float b, float a);
	void SetDiffuseColor(float r, float g, float b, float a);
	void SetPosition(float x, float y, float z);
	void SetSpecular(float x, float y, float z, float a, float power);

	const VEC4 &GetDiffuseColor() const;
	const VEC4 &GetAmbientColor() const;
	const VEC4 &GetSpecularColor() const;
	const VEC3 &GetDirection() const;
	float GetSpecularPower() const;

	void BuildMatrix(const Camera *camera);

	const MAT4 &GetViewMatrix() const;
	const MAT4 &GetProjectionMatrix() const;

  private:
	VEC4 m_ambientColor;
	VEC4 m_specularColor;
	VEC4 m_diffuseColor;
	VEC3 m_position;
	VEC3 m_direction;
	float m_power;
	MAT4 m_viewMatrix;
	MAT4 m_projectionMatrix;
	bool m_beenChanged;
};

#endif // !SE_LIGHTDX_H
