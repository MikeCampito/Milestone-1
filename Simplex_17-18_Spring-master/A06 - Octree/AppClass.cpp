#include "AppClass.h"
using namespace Simplex;
void Application::InitVariables(void)
{
	//Set the position and target of the camera
	m_pCameraMngr->SetPositionTargetAndUp(
<<<<<<< HEAD
		vector3(0.0f, 2.0f, 10.0f), //Position
		vector3(0.0f, 2.0f, 11.0f),	//Target
=======
		vector3(0.0f, 2.0f, 100.0f), //Position
		vector3(0.0f, 2.0f, 99.0f),	//Target
>>>>>>> eb0e29b4d39ba186b0cab49f8c57cc5231466cff
		AXIS_Y);					//Up

	m_pLightMngr->SetPosition(vector3(0.0f, 3.0f, 13.0f), 1); //set the position of first light (0 is reserved for ambient light)

#ifdef DEBUG
	uint uInstances = 30;
#else
	uint uInstances = 1849;
#endif
	int nSquare = static_cast<int>(std::sqrt(uInstances));
	m_uObjects = nSquare * nSquare;
	uint uIndex = -1;
	for (int i = 0; i < nSquare; i++)
	{
		for (int j = 0; j < nSquare; j++)
		{
			uIndex++;
			m_pEntityMngr->AddEntity("Minecraft\\Cow.obj");
			vector3 v3Position = vector3(glm::sphericalRand(34.0f));
			matrix4 m4Position = glm::translate(v3Position);
			m_pEntityMngr->SetModelMatrix(m4Position);
				m_pEntityMngr->UsePhysicsSolver();
		}
	}
	/*for (int i = 0; i < 100; i++)
	{
		m_pEntityMngr->AddEntity("Minecraft\\Cube.obj", "Cube_" + std::to_string(i));
		vector3 v3Position = vector3(glm::sphericalRand(12.0f));
		v3Position.y = 0.0f;
		matrix4 m4Position = glm::translate(v3Position);
		m_pEntityMngr->SetModelMatrix(m4Position);
		m_pEntityMngr->UsePhysicsSolver();
		//m_pEntityMngr->SetMass(i+1);
	}*/
	m_uOctantLevels = 3;
	m_pEntityMngr->Update();

	Octree = new MyOctant(3, 5);
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();
	timer += .01;
	if (timer > 3) {
		m_pEntityMngr->ClearDimensionSetAll();
		Octree->KillBranches();
		SafeDelete(Octree);
		Octree = new MyOctant(m_uOctantLevels, 5);
		timer = 0;
	}

	//Is the ArcBall active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();

	//Update Entity Manager
	m_pEntityMngr->Update();

	//Add objects to render list
	m_pEntityMngr->AddEntityToRenderList(-1, true);

	//Catch cows
	this->catchCows();
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	//display octree
	if (displayOctree)
	{
		if (displayLeafs)
			Octree->DisplayLeafs();
		else
			Octree->Display(0);
	}


	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();

	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();

	//draw gui,
	DrawGUI();

	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	//release GUI
	ShutdownGUI();
}


void Simplex::Application::ThrowNet(void)
{
	vector3 startPos = this->m_pCameraMngr->GetPosition();
	m_pEntityMngr->AddEntity("Minecraft\\Net.obj");
	this->netID = m_pEntityMngr->GetEntityIndex(m_pEntityMngr->GetUniqueID());
	m_pEntityMngr->UsePhysicsSolver(true, this->netID);
	matrix4 m4Position = this->m_pCameraMngr->GetCamera()->GetCameraPlane();
	m_pEntityMngr->SetModelMatrix(m4Position);
	vector3 launchVector = this->m_pCameraMngr->GetForward();
<<<<<<< HEAD
	m_pEntityMngr->GetEntity(this->netID)->GetSolver()->SetNet(true);
	
=======
	launchVector = launchVector * 100.0f;
	std::cout << "(" << launchVector.x << ", " << launchVector.y << ", " << launchVector.z << ")" << std::endl;

	for (int i = 0; i < Octree->GetOctantsWithChildren().size(); i++) {
		m_pEntityMngr->GetEntity(netID)->AddDimension(Octree->GetOctantsWithChildren().at(i)->getM_UID());
	}



>>>>>>> eb0e29b4d39ba186b0cab49f8c57cc5231466cff
	m_pEntityMngr->ApplyForce(launchVector, netID);
	
	
}

void Simplex::Application::RecallNet(void) {
<<<<<<< HEAD
	vector3 recallPos = this->m_pCameraMngr->GetPosition();
	vector3 recallVector = recallPos - m_pEntityMngr->GetEntity(netID)->GetPosition();
=======
	printf("Net recalled");
	this->recalling = true;
	this->recallReturnPosition = this->m_pCameraMngr->GetPosition();
	vector3 recallVector = recallReturnPosition - m_pEntityMngr->GetEntity(netID)->GetPosition();
>>>>>>> eb0e29b4d39ba186b0cab49f8c57cc5231466cff
	m_pEntityMngr->ApplyForce(recallVector, netID);
	
}

void Simplex::Application::catchCows(void) {
<<<<<<< HEAD
	if (m_pEntityMngr->GetEntity(netID)->GetCollidingCount() > 0) {
=======
	if (recalling) {
		if (m_pEntityMngr->GetEntity(netID)->GetCollidingCount() > 0) {
			Simplex::MyRigidBody::PRigidBody* cows = m_pEntityMngr->GetEntity(netID)->GetColliderArray();
			for (int i = 0; i < sizeof(cows); i += sizeof(Simplex::MyRigidBody::PRigidBody)) {
				for (int j = 0; j < m_pEntityMngr->GetEntityCount(); j += sizeof(MyEntity)) {
					printf("Pulling cow!");
					vector3 recallVector = recallReturnPosition - m_pEntityMngr->GetEntity(j)->GetPosition();
					m_pEntityMngr->ApplyForce(recallVector, j);
					pulledCows.push_back(j);
				}
			}
		}
	}
	float distanceSqrd = pow((recallReturnPosition.x - m_pEntityMngr->GetEntity()->GetPosition().x), 2) + pow((recallReturnPosition.z - m_pEntityMngr->GetEntity()->GetPosition().z), 2);
	if (distanceSqrd <= 4) {
		printf("Net Caught!");
		m_pEntityMngr->RemoveEntity(netID);
		this->recalling = false;
		for (int i = 0; i < pulledCows.size(); i++) {
			m_pEntityMngr->RemoveEntity(pulledCows.at(i));
			points++;
			std::cout << std::endl;
			std::cout << "You've scored " << points << " points!" << std::endl;
			std::cout << std::endl;
		}
		pulledCows.clear();
>>>>>>> eb0e29b4d39ba186b0cab49f8c57cc5231466cff
	}
}