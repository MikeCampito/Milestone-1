#include "AppClass.h"
void Application::InitVariables(void)
{
	////Change this to your name and email
	//m_sProgrammer = "Alberto Bobadilla - labigm@rit.edu";

	////Alberto needed this at this position for software recording.
	//m_pWindow->setPosition(sf::Vector2i(710, 0));

	m_pMesh = new MyMesh();
	m_pMesh->AddVertexPosition(vector3(0.0f));
	m_pMesh->AddVertexColor(vector3(1.0f, 0.0f, 0.0f));
	m_pMesh->AddVertexPosition(vector3(1.0f));
	m_pMesh->AddVertexColor(vector3(0.0f, 0.0f, 1.0f));
	m_pMesh->AddVertexPosition(vector3(-1.0f, 0.0f, 0.0f));
	m_pMesh->AddVertexColor(vector3(0.0f, 1.0f, 0.0f));
	m_pMesh->CompileMesh();
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
	
	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();

	m_pMesh->Render(IDENTITY_M4, IDENTITY_M4, IDENTITY_M4);
	
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
	//release GUI
	ShutdownGUI();

	if (m_pMesh != nullptr)
	{
		delete m_pMesh;
		m_pMesh = nullptr;
	}
}