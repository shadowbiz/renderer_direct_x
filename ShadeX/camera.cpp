////////////////////////////////////////////////////////////////////////////////
// Filename: cameraclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "camera.h"

Camera::Camera()
{
}

Camera::Camera(const Camera &other)
{
	UNREFERENCED_PARAMETER(other);
}

Camera::~Camera()
{
}

bool Camera::Initialize(const MAT4 &projectionMatrix)
{
	m_projectionMatrix = projectionMatrix;
	m_position = VEC3(0, 0, 0);
	m_rotation = VEC3(0, 0, 0);
	m_lookAt = VEC3(0, 0, 0);
	m_front = VEC3(0, 0, 1);
	m_right = VEC3(1, 0, 0);
	m_up = VEC3(0, 1, 0);
	m_viewMatrix = XMMatrixIdentity();
	m_rotationMatrix = XMMatrixIdentity();
	m_hasChanged = true;
	return true;
}

void Camera::Release()
{
}

void Camera::SetPosition(float x, float y, float z)
{
	m_position = VEC3(x, y, z);
	m_hasChanged = true;
}

void Camera::SetRotation(float x, float y, float z)
{
	m_rotation = VEC3(x, y, z);
	m_hasChanged = true;
}

void Camera::SetPosition(const VEC3 &vec)
{
	m_position = vec;
	m_hasChanged = true;
}

void Camera::SetRotation(const VEC3 &vec)
{
	m_rotation = vec;
	m_hasChanged = true;
}

void Camera::Rotate(float x, float y, float z)
{
	XMStoreFloat3(&m_rotation, XMVectorAdd(XMLoadFloat3(&m_rotation), XMVectorSet(x, y, z, 0)));
	if (m_rotation.x > 70)
	{
		m_rotation.x = 70;
	}
	else if (m_rotation.x < 40)
	{
		m_rotation.x = 40;
	}
	m_rotationMatrix = XMMatrixRotationRollPitchYawFromVector(DEG2RAD(XMLoadFloat3(&m_rotation)));
	m_hasChanged = true;
}

void Camera::Translate(float x, float y, float z)
{
	XMStoreFloat3(&m_position, XMVectorAdd(XMLoadFloat3(&m_position), x * XMLoadFloat3(&m_right)));
	XMStoreFloat3(&m_position, XMVectorAdd(XMLoadFloat3(&m_position), z * XMLoadFloat3(&m_front)));
	m_hasChanged = true;
}

const VEC3 &Camera::GetPosition() const
{
	return m_position;
}

const VEC3 &Camera::GetRotation() const
{
	return m_rotation;
}

void Camera::UpdateMatrix()
{
	XMStoreFloat3(&m_front, XMVector3Normalize(XMVector3TransformCoord(XMVectorSet(0, 0, 1, 0), m_rotationMatrix)));
	XMStoreFloat3(&m_up, XMVector3Normalize(XMVector3TransformCoord(XMVectorSet(0, 1, 0, 0), m_rotationMatrix)));
	XMStoreFloat3(&m_right, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&m_up), XMLoadFloat3(&m_front))));
	XMStoreFloat3(&m_lookAt, XMVectorAdd(XMLoadFloat3(&m_position), XMLoadFloat3(&m_front)));
	XMStoreFloat3(&m_front, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&m_right), XMVectorSet(0, 1, 0, 0))));
	m_viewMatrix = XMMatrixTranspose(XMMatrixLookAtLH(XMLoadFloat3(&m_position), XMLoadFloat3(&m_lookAt), XMLoadFloat3(&m_up)));
	m_hasChanged = false;
}

void Camera::Update()
{
	if (m_hasChanged)
	{
		UpdateMatrix();
	}
}

const MAT4 &Camera::GetViewMatrix() const
{
	return m_viewMatrix;
}

const MAT4 &Camera::GetProjectionMatrix() const
{
	return m_projectionMatrix;
}