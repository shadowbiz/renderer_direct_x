#include "scene.h"

Scene::Scene()
{
	m_D3D = NULL;
	m_camera = NULL;
	m_light = NULL;
	m_model = NULL;
	m_plane = NULL;
	m_gameObject1 = NULL;
	//m_gameObjectPlane	= NULL;
	m_gameObject3 = NULL;
	m_gameObject4 = NULL;
	m_terrain = NULL;
}

Scene::Scene(const Scene &other)
{
	UNREFERENCED_PARAMETER(other);
}

Scene::~Scene()
{
}

void Scene::Release()
{
	SAFE_RELEASE_DELETE(m_camera);
	SAFE_RELEASE_DELETE(m_gameObject1);
	//SAFE_RELEASE_DELETE(m_gameObjectPlane);
	SAFE_RELEASE_DELETE(m_gameObject3);
	SAFE_RELEASE_DELETE(m_gameObject4);
	SAFE_RELEASE_DELETE(m_model);
	SAFE_RELEASE_DELETE(m_plane);
	SAFE_RELEASE_DELETE(m_terrain);
	SAFE_DELETE(m_light);
	m_gameObjects.clear();
}

bool Scene::Initialize(D3D *d3D, HWND hwnd)
{
	m_D3D = d3D;
	m_camera = new Camera;
	if (!m_camera)
	{
		return false;
	}
	bool result = m_camera->Initialize(m_D3D->GetProjectionMatrix());
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize camera.", L"Error", MB_OK);
		return false;
	}
	m_camera->SetPosition(0.0f, 35.0f, -35.0f);
	m_camera->SetRotation(45, 0, 0);
	m_light = new Light;
	if (!m_light)
	{
		return false;
	}
	m_light->SetAmbientColor(0.05f, 0.05f, 0.05f, 1.0f);
	m_light->SetDiffuseColor(0.7f, 0.7f, 0.7f, 1.0f);
	m_light->SetSpecular(0.6, 0.6, 0.6, 1, 6);
	m_light->SetPosition(120, 120.0f, 120);

	m_model = new Model;
	if (!m_model)
	{
		return false;
	}
	result = m_model->Initialize(m_D3D->GetDevice(), hwnd, "data/sphere2.obj");
	if (!result)
	{
		return false;
	}
	m_plane = new Model;
	result = m_plane->Initialize(m_D3D->GetDevice(), hwnd, Model::ModelType::ModelPlane);
	if (!result)
	{
		return false;
	}

	m_gameObject1 = new GameObject;
	if (!m_gameObject1)
	{
		return false;
	}
	result = m_gameObject1->Initialize(m_D3D->GetDevice(), hwnd, m_model, L"data/seafloor.dds");
	if (!result)
	{
		return false;
	}
#if 0
	m_gameObjectPlane = new GameObject;
	if (!m_gameObjectPlane) { 
		return false;
	}
	result = m_gameObjectPlane->Initialize(m_D3D->GetDevice(), hwnd, m_plane, L"data/seafloor.dds");
	if (!result) { 
		return false;
}
#endif

	m_gameObject3 = new GameObject;
	if (!m_gameObject3)
	{
		return false;
	}
	result = m_gameObject3->Initialize(m_D3D->GetDevice(), hwnd, m_model, L"data/seafloor.dds");
	if (!result)
	{
		return false;
	}
	m_gameObject4 = new GameObject;
	if (!m_gameObject4)
	{
		return false;
	}
	result = m_gameObject4->Initialize(m_D3D->GetDevice(), hwnd, m_model, L"data/seafloor.dds");
	if (!result)
	{
		return false;
	}

	m_gameObjects.push_back(m_gameObject1);
	m_gameObjects.push_back(m_gameObject3);
	m_gameObjects.push_back(m_gameObject4);

	m_terrain = new Terrain;
	if (!m_terrain)
	{
		return false;
	}
	result = m_terrain->Initialize(m_D3D->GetDevice(), "data/heightmap01.bmp");
	if (!result)
	{
		return false;
	}

	return true;
}

bool Scene::Update(float deltaTime, const Input *input)
{
	static float pos = 0;
	static float delta = 0;
	delta += deltaTime;
	if (delta > 360)
	{
		delta = -360;
	}
	pos = 2 * sinf(delta);

	float fx, fy;
	input->GetMouseDelta(fx, fy);
	m_camera->Rotate(fy * deltaTime * 5, fx * deltaTime * 5, 0);
	input->GetKeyboardDelta(fx, fy);
	m_camera->Translate(fx * deltaTime, 0, fy * deltaTime);
	m_camera->Update();

	float px = 20 * sinf(delta);
	float py = 20 * cosf(delta);
	//m_light->SetPosition(px, 10.0f, py);
	m_light->BuildMatrix(m_camera);

	m_gameObject1->SetPosition(30, 3 + pos, 20);
	m_gameObject1->SetRotation(0, -delta, 0);
	m_gameObject1->Update(deltaTime);

	m_gameObject3->SetPosition(32, 1, 20);
	m_gameObject3->Update(deltaTime);

	m_gameObject4->SetPosition(30, 3, 22);
	m_gameObject4->SetRotation(0, delta, 0);
	m_gameObject4->Update(deltaTime);

	//m_gameObjectPlane->Update(deltaTime);

	return true;
}

bool Scene::ShadowPass(DepthShader *depthShader)
{
	for (GameObject *go : m_gameObjects)
	{
		go->Render(m_D3D->GetDeviceContext());
		bool result = depthShader->Render(m_D3D->GetDeviceContext(), go->GetIndexCount(), go->GetWorldMatrix(), m_light->GetViewMatrix(), m_light->GetProjectionMatrix());
		if (!result)
		{
			return false;
		}
	}
	m_terrain->Render(m_D3D->GetDeviceContext());
	bool result = depthShader->Render(m_D3D->GetDeviceContext(), m_terrain->GetIndexCount(), m_D3D->GetIdentityMatrix(), m_light->GetViewMatrix(), m_light->GetProjectionMatrix());
	if (!result)
	{
		return false;
	}
	return true;
}

bool Scene::Render(ShadowShader *shader, ShadowMapBufferRenderer *shadowMap)
{
	for (GameObject *go : m_gameObjects)
	{
		go->Render(m_D3D->GetDeviceContext());
		bool result = shader->Render(m_D3D->GetDeviceContext(), go->GetIndexCount(), go->GetWorldMatrix(), m_camera, m_light, go->GetTexture(), shadowMap);
		if (!result)
		{
			return false;
		}
	}
	m_terrain->Render(m_D3D->GetDeviceContext());
	bool result = shader->Render(m_D3D->GetDeviceContext(), m_terrain->GetIndexCount(), m_D3D->GetIdentityMatrix(), m_camera, m_light, m_gameObject1->GetTexture(), shadowMap);
	return true;
}

Camera *Scene::GetCamera() const
{
	return m_camera;
}
