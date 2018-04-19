#include "AppClass.h"
void Application::InitVariables(void)
{
	//Make MyMesh object
	m_pMesh = new MyMesh();
	m_pMesh->GenerateCone(1.0f,1.0f, 7, C_BROWN);

	//Make MyMesh object
	m_pMesh1 = new MyMesh();
	m_pMesh1->GenerateCone(0.5f, 0.5f, 7, C_WHITE);
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix();
	float fovy = 45.0f;
	float ratio = m_pSystem->GetWindowWidth() / static_cast<float>(m_pSystem->GetWindowHeight());
	float fNear = 0.01f;
	m4Projection = glm::perspective(fovy, ratio, fNear, fFar);

	float left = -fDistance;
	float right = fDistance;
	float top = fDistance;
	float bottom = -fDistance;

	//float left = m_pSystem->GetWindowWidth() / -2.0f;
	//float right = m_pSystem->GetWindowWidth() / 2.0f;
	//float top = m_pSystem->GetWindowHeight() / 2.0f;
	//float bottom = m_pSystem->GetWindowHeight() / -2.0f;

	m4Projection = glm::ortho(left, right, bottom, top, fNear, fFar);

	matrix4 m4View = m_pCameraMngr->GetViewMatrix();
	vector3 v3Position(0.0f, 0.0f, 15.0f);
	vector3 v3Target(0.0f, 0.0f, 0.0f);
	vector3 v3Up(0.0f,1.0f,0.0f);
	m4View = glm::lookAt(v3Position, v3Target, v3Up);

	m_pMesh->Render( m4Projection, m4View, ToMatrix4(m_qArcBall));
	m_pMesh1->Render(m4Projection, m4View, glm::translate(vector3( 3.0f, 0.0f, 0.0f)));
		
	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();
	
	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();
	
	//draw gui
	DrawGUI();
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	if (m_pMesh != nullptr)
	{
		delete m_pMesh;
		m_pMesh = nullptr;
	}
	SafeDelete(m_pMesh1);
	//release GUI
	ShutdownGUI();
}