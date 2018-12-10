#include "gameobject.h"

GameObject::GameObject()
{
	m_model = NULL;
	m_texture = NULL;
	m_hasChanged = true;
	m_textureCreated = false;
}

GameObject::GameObject(const GameObject &other)
{
	UNREFERENCED_PARAMETER(other);
}

GameObject::~GameObject()
{
}

bool GameObject::Initialize(ID3D11Device *device, HWND hwnd, Model *model, Texture *texture)
{
	m_model = model;
	m_texture = texture;
	m_worldMatrix = XMMatrixIdentity();
	m_translationMatrix = XMMatrixIdentity();
	m_rotationMatrix = XMMatrixIdentity();
	m_position = XMVectorZero();
	m_rotation = XMVectorZero();
	return true;
}

bool GameObject::Initialize(ID3D11Device *device, HWND hwnd, Model *model, const WCHAR *texture)
{
	bool result = LoadTexture(device, texture);
	if (!result)
	{
		MessageBox(hwnd, L"Could not load model texture file.", L"Error", MB_OK);
		return false;
	}
	m_textureCreated = true;
	m_model = model;
	m_worldMatrix = XMMatrixIdentity();
	m_translationMatrix = XMMatrixIdentity();
	m_rotationMatrix = XMMatrixIdentity();
	m_position = XMVectorZero();
	m_rotation = XMVectorZero();
	return true;
}

void GameObject::Release()
{
	ReleaseTexture();
}

ID3D11ShaderResourceView *GameObject::GetTexture() const
{
	return m_texture->GetTexture();
}

bool GameObject::LoadTexture(ID3D11Device *device, const WCHAR *filename)
{
	m_texture = new Texture;
	if (!m_texture)
	{
		return false;
	}
	bool result = m_texture->Initialize(device, filename);
	if (!result)
	{
		return false;
	}
	return true;
}

void GameObject::ReleaseTexture()
{
	if (m_textureCreated)
	{
		SAFE_RELEASE_DELETE(m_texture);
	}
}

void GameObject::Render(ID3D11DeviceContext *devCont)
{
	m_model->Render(devCont);
}

void GameObject::Update(float deltaTime)
{
	UNREFERENCED_PARAMETER(deltaTime);
	if (m_hasChanged)
	{
		UpdateMatrices();
	}
}

void GameObject::UpdateMatrices()
{
	m_translationMatrix = XMMatrixTranslationFromVector(m_position);
	m_rotationMatrix = XMMatrixRotationRollPitchYawFromVector(m_rotation);
	m_worldMatrix = XMMatrixTranspose(XMMatrixMultiply(m_rotationMatrix, m_translationMatrix));
	m_hasChanged = false;
}

void GameObject::SetPosition(const XMVECTOR &position)
{
	m_position = position;
	m_hasChanged = true;
}

void GameObject::SetPosition(const VEC3 &position)
{
	m_position = XMVectorSet(position.x, position.y, position.z, 0);
	m_hasChanged = true;
}

void GameObject::SetPosition(float x, float y, float z)
{
	m_position = XMVectorSet(x, y, z, 0);
	m_hasChanged = true;
}

void GameObject::SetRotation(const XMVECTOR &position)
{
	m_rotation = position;
	m_hasChanged = true;
}

void GameObject::SetRotation(const VEC3 &position)
{
	m_rotation = XMVectorSet(position.x, position.y, position.z, 0);
	m_hasChanged = true;
}

void GameObject::SetRotation(float x, float y, float z)
{
	m_rotation = XMVectorSet(x, y, z, 0);
	m_hasChanged = true;
}

const XMMATRIX &GameObject::GetWorldMatrix() const
{
	return m_worldMatrix;
}

UINT GameObject::GetIndexCount() const
{
	return m_model->GetIndexCount();
}