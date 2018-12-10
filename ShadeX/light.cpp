#include "light.h"

Light::Light()
{
}

Light::Light(const Light &other)
{
	UNREFERENCED_PARAMETER(other);
}

Light::~Light()
{
}

void Light::SetAmbientColor(float r, float g, float b, float a)
{
	m_ambientColor = VEC4(r, g, b, a);
}

void Light::SetDiffuseColor(float r, float g, float b, float a)
{
	m_diffuseColor = VEC4(r, g, b, a);
}

void Light::SetSpecular(float r, float g, float b, float a, float power)
{
	m_specularColor = VEC4(r, g, b, a);
	m_power = power;
}

void Light::SetPosition(float x, float y, float z)
{
	m_position = VEC3(x, y, z);
	XMStoreFloat3(&m_direction, XMVector3Normalize(-1 * XMLoadFloat3(&m_position)));
	m_projectionMatrix = XMMatrixTranspose(XMMatrixOrthographicLH(100, 100, 10, 100));
	m_viewMatrix = XMMatrixTranspose(XMMatrixLookAtLH(XMLoadFloat3(&m_position), XMVectorZero(), XMVectorSet(0, 1, 0, 0)));
}

void Light::BuildMatrix(const Camera *camera)
{
	return;
}

const VEC4 &Light::GetAmbientColor() const
{
	return m_ambientColor;
}

const VEC4 &Light::GetDiffuseColor() const
{
	return m_diffuseColor;
}

const VEC4 &Light::GetSpecularColor() const
{
	return m_specularColor;
}

float Light::GetSpecularPower() const
{
	return m_power;
}

const VEC3 &Light::GetDirection() const
{
	return m_direction;
}

const MAT4 &Light::GetViewMatrix() const
{
	return m_viewMatrix;
}

const MAT4 &Light::GetProjectionMatrix() const
{
	return m_projectionMatrix;
}