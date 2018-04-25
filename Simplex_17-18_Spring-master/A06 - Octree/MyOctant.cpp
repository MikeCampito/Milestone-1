#include "MyOctant.h"
using namespace Simplex;

uint MyOctant::m_uOctantCount; //will store the number of octants instantiated
uint MyOctant::m_uMaxLevel;//will store the maximum level an octant can go to
uint MyOctant::m_uIdealEntityCount; //will tell how many ideal Entities this object will contain
bool MyOctant::m_OctreeOn = true;	//tells whether the Octree is on or off

Simplex::MyOctant::MyOctant(uint a_nMaxLevel, uint a_nIdealEntityCount)
{
	//setup static members
	m_uOctantCount = 1;
	m_uMaxLevel = a_nMaxLevel;
	m_uIdealEntityCount = a_nIdealEntityCount;

	//setup Managers
	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();

	//setup dimensions
	m_v3Min = m_pEntityMngr->GetRigidBody(0)->GetMinGlobal();
	m_v3Max = m_pEntityMngr->GetRigidBody(0)->GetMaxGlobal();
	//go through all entities to find Octree's dimensions
	for (uint i = 0; i < m_pEntityMngr->GetEntityCount(); i++)
	{
		//min
		if (m_v3Min.x > m_pEntityMngr->GetRigidBody(i)->GetMinGlobal().x)
		{
			m_v3Min.x = m_pEntityMngr->GetRigidBody(i)->GetMinGlobal().x;
		}
		if (m_v3Min.y > m_pEntityMngr->GetRigidBody(i)->GetMinGlobal().y)
		{
			m_v3Min.y = m_pEntityMngr->GetRigidBody(i)->GetMinGlobal().y;
		}
		if (m_v3Min.z > m_pEntityMngr->GetRigidBody(i)->GetMinGlobal().z)
		{
			m_v3Min.z = m_pEntityMngr->GetRigidBody(i)->GetMinGlobal().z;
		}
		//max
		if (m_v3Max.x < m_pEntityMngr->GetRigidBody(i)->GetMaxGlobal().x)
		{
			m_v3Max.x = m_pEntityMngr->GetRigidBody(i)->GetMaxGlobal().x;
		}
		if (m_v3Max.y < m_pEntityMngr->GetRigidBody(i)->GetMaxGlobal().y)
		{
			m_v3Max.y = m_pEntityMngr->GetRigidBody(i)->GetMaxGlobal().y;
		}
		if (m_v3Max.z < m_pEntityMngr->GetRigidBody(i)->GetMaxGlobal().z)
		{
			m_v3Max.z = m_pEntityMngr->GetRigidBody(i)->GetMaxGlobal().z;
		}
	}

	m_v3Size = m_v3Max - m_v3Min;
	m_v3Center = m_v3Min + m_v3Size / 2.0f;

	//setup entity list
	for (uint i = 0; i < m_pEntityMngr->GetEntityCount(); i++)
	{
		m_EntityList.push_back(i);
	}

	//other members
	m_uLevel = 1;
	m_pRoot = this;

	//add Octant to m_lChildWithObjects
	m_lChildWithObjects.push_back(this);

	//Construct tree
	ConstructTree();

}

Simplex::MyOctant::MyOctant(vector3 a_v3Center, vector3 a_fSize)
{
	//setup Octant's dimensions
	m_v3Center = a_v3Center;
	m_v3Size = a_fSize;
	m_v3Min = a_v3Center - a_fSize/2.0f;
	m_v3Max = a_v3Center + a_fSize/2.0f;
}

Simplex::MyOctant::MyOctant(MyOctant const & other)
{
	//static members
	m_uOctantCount = other.m_uOctantCount;
	m_uMaxLevel = other.m_uMaxLevel;
	m_uIdealEntityCount = other.m_uIdealEntityCount;
	m_OctreeOn = other.m_OctreeOn;

	//managers
	m_pMeshMngr = other.m_pMeshMngr;
	m_pEntityMngr = other.m_pEntityMngr;

	//Counts
	m_uID = other.m_uID;
	m_uLevel = other.m_uLevel;
	m_uChildren = other.m_uChildren;

	//Dimensions
	m_v3Size = other.m_v3Size;
	m_v3Center = other.m_v3Center;
	m_v3Min = other.m_v3Min;
	m_v3Max = other.m_v3Max;

	//parent and child
	m_pParent = other.m_pParent;
	for (int i = 0; i < 8; i++)
	{
		m_pChild[i] = other.m_pChild[i];
	}

	//entities
	m_EntityList = other.m_EntityList;

	//Root Octant Stuff
	m_pRoot = other.m_pRoot;
	m_lChild = other.m_lChild;
	m_lChildWithObjects = other.m_lChildWithObjects;
}

MyOctant & Simplex::MyOctant::operator=(MyOctant const & other)
{
	return *this;
}

Simplex::MyOctant::~MyOctant(void)
{
	Release();
}

vector3 Simplex::MyOctant::GetSize(void)
{
	return m_v3Size;
}

vector3 Simplex::MyOctant::GetCenterGlobal(void)
{
	return m_v3Center;
}

vector3 Simplex::MyOctant::GetMinGlobal(void)
{
	return m_v3Min;
}

vector3 Simplex::MyOctant::GetMaxGlobal(void)
{
	return m_v3Max;
}

bool Simplex::MyOctant::IsColliding(uint a_uRBIndex)
{
	//get entity's rigidbody
	MyRigidBody* entityRB = m_pEntityMngr->GetEntity(a_uRBIndex)->GetRigidBody();

	//get min and max of the Octant and Entity
	vector3 vecOctantMax = GetMaxGlobal();
	vector3 vecEntityMax = entityRB->GetMaxGlobal();
	vector3 vecOctantMin = GetMinGlobal();
	vector3 vecEntityMin = entityRB->GetMinGlobal();

	//check if it is within Octant
	if (GetMaxGlobal().x < entityRB->GetMinGlobal().x)
		return false;
	if (GetMinGlobal().x > entityRB->GetMaxGlobal().x)
		return false;

	if (GetMaxGlobal().y < entityRB->GetMinGlobal().y)
		return false;
	if (GetMinGlobal().y > entityRB->GetMaxGlobal().y)
		return false;

	if (GetMaxGlobal().z < entityRB->GetMinGlobal().z)
		return false;
	if (GetMinGlobal().z > entityRB->GetMaxGlobal().z)
		return false;

	//if none of the tests succeeded, colliding
	return true;
}

void Simplex::MyOctant::Display(uint a_nIndex, vector3 a_v3Color)
{
	//check if Octant is active
	if (m_OctreeOn)
	{
		//display this Octant
		Display();

		//call this method for all children
		for (int i = 0; i < m_uChildren; i++)
		{
			GetChild(i)->Display(0, a_v3Color);
		}
	}
}

void Simplex::MyOctant::Display(vector3 a_v3Color)
{
	//check if Octree is active
	if (m_OctreeOn)
	{
		//add the Octant's cube to the render list
		m_pMeshMngr->AddWireCubeToRenderList(glm::translate(m_v3Center) * glm::scale(GetSize()), a_v3Color);
	}
}

void Simplex::MyOctant::DisplayLeafs(vector3 a_v3Color)
{
	//check if Octree is active
	if (m_OctreeOn)
	{
		//go through the list of leafs with objects
		for (MyOctant* node : m_pRoot->m_lChildWithObjects)
		{
			//check if this Octant has no children
			if (node->IsLeaf())
			{
				if (node->m_EntityList.size() > 0)
				{
					//call this Octant's Display function
					node->Display(C_YELLOW);
				}
			}
		}
	}
}

void Simplex::MyOctant::ClearEntityList(void)
{
	//go through each Octant with objects
	for (MyOctant* node : m_pRoot->m_lChild)
	{
		//clear the entity list
		node->m_EntityList.clear();
	}
}

void Simplex::MyOctant::Subdivide(void)
{
	//first check if Octant is beyond m_uMaxLevel
	if (m_uLevel >= m_uMaxLevel)
	{
		//at the subdivison limit, don't subdivide
		return;
	}

	//check if Octant already has children
	if (m_uChildren != 0)
	{
		//already has children, don't subdivide
		return;
	}

	//check if Octant has few entities
	if (m_EntityList.size() <= m_uIdealEntityCount)
	{
		//not enough entities to subdivide
		return;
	}

	//if this far, subdivide
	//create 8 children
	m_uChildren = 8;
	m_pChild[0] = new MyOctant(GetMinGlobal() + GetSize() / 4.0f, GetSize() / 2.0f);
	m_pChild[1] = new MyOctant(vector3(GetCenterGlobal().x + GetSize().x/4.0f, GetMinGlobal().y + GetSize().y / 4.0f, GetMinGlobal().z + GetSize().z / 4.0f), GetSize() / 2.0f);
	m_pChild[2] = new MyOctant(vector3(GetMinGlobal().x + GetSize().x / 4.0f, GetCenterGlobal().y + GetSize().y / 4.0f, GetMinGlobal().z + GetSize().z / 4.0f), GetSize() / 2.0f);
	m_pChild[3] = new MyOctant(vector3(GetCenterGlobal().x + GetSize().x / 4.0f, GetCenterGlobal().y + GetSize().y / 4.0f, GetMinGlobal().z + GetSize().z / 4.0f), GetSize() / 2.0f);
	m_pChild[4] = new MyOctant(vector3(GetMinGlobal().x + GetSize().x / 4.0f, GetMinGlobal().y + GetSize().y / 4.0f, GetCenterGlobal().z + GetSize().z / 4.0f), GetSize() / 2.0f);
	m_pChild[5] = new MyOctant(vector3(GetCenterGlobal().x + GetSize().x / 4.0f, GetMinGlobal().y + GetSize().y / 4.0f, GetCenterGlobal().z + GetSize().z / 4.0f), GetSize() / 2.0f);
	m_pChild[6] = new MyOctant(vector3(GetMinGlobal().x + GetSize().x / 4.0f, GetCenterGlobal().y + GetSize().y / 4.0f, GetCenterGlobal().z + GetSize().z / 4.0f), GetSize() / 2.0f);
	m_pChild[7] = new MyOctant(vector3(GetCenterGlobal().x + GetSize().x / 4.0f, GetCenterGlobal().y + GetSize().y / 4.0f, GetCenterGlobal().z + GetSize().z / 4.0f), GetSize() / 2.0f);

	//setup children members
	for (uint i = 0; i < 8; i++)
	{
		//increment Octant Count and ID
		m_pChild[i]->m_uID = m_uOctantCount;
		m_uOctantCount++;
		
		//setup managers
		m_pChild[i]->m_pMeshMngr = m_pMeshMngr;
		m_pChild[i]->m_pEntityMngr = m_pEntityMngr;

		//set level
		m_pChild[i]->m_uLevel = m_uLevel + 1;
		
		//setup parent Octant
		m_pChild[i]->m_pParent = this;

		//check for entities inside child
		for (uint entityRB : m_EntityList)
		{
			//call IsColliding for each of the parent Ocatant's entities
			if (m_pChild[i]->IsColliding(entityRB))
			{
				//add entity to child's entity list
				m_pChild[i]->m_EntityList.push_back(entityRB);
			}
		}

		//check if child has entities
		if (m_pChild[i]->m_EntityList.size() > 0)
		{
			//add child to m_lChildWithObjects of root
			m_pRoot->m_lChildWithObjects.push_back(m_pChild[i]);
		}
		else
		{
			//add child to m_lChildNoObjects
			m_pRoot->m_lChildNoObjects.push_back(m_pChild[i]);
		}

		//add child to list of children
		m_pChild[i]->m_pRoot = m_pRoot;
		m_pRoot->m_lChild.push_back(m_pChild[i]);
	}
}

MyOctant * Simplex::MyOctant::GetChild(uint a_nChild)
{
	//check if a_nChild is greater than 7
	if (a_nChild > 7)
	{
		return nullptr;
	}

	//return the child from the children list
	return m_pChild[a_nChild];
}

MyOctant * Simplex::MyOctant::GetParent(void)
{
	//get the parent Octant
	return m_pParent;
}

bool Simplex::MyOctant::IsLeaf(void)
{
	//check if Octant has children
	if (m_uChildren == 0)
	{
		//has no children
		return true;
	}
	else
	{
		//has children
		return false;
	}
}

bool Simplex::MyOctant::ContainsMoreThan(uint a_nEntities)
{
	//check if entity list is more than the desired entities
	if (m_EntityList.size() > a_nEntities)
	{
		//has more entities
		return true;
	}
	else
	{
		//has the same or less entities
		return false;
	}
}

void Simplex::MyOctant::KillBranches(void)
{
	//check if Octant has children
	if (m_uChildren == 0)
	{
		//no children to destroy, leave
		return;
	}

	//has children
	//call KillBranches for each child
	for (MyOctant* child : m_pChild)
	{
		child->KillBranches();
	}

	//back here, all children's branches are destroyed
	//now destroy children
	for (uint i = 0; i < m_uChildren; i++)
	{
		m_pChild[i]->Release();
	}

	//set children to 0
	m_uChildren = 0;

	//clear dimensions entities are in
	for (int i = 0; i < m_pEntityMngr->GetEntityCount(); i++)
	{
		m_pEntityMngr->GetEntity(i)->ClearDimensionSet();
	}

	//check if this is the root
	if (this == m_pRoot)
	{
		//clear lists
		m_lChild.clear();
		m_lChildWithObjects.clear();
		m_lChildNoObjects.clear();

		//add the root to lists
		m_lChild.push_back(this);
		m_lChildWithObjects.push_back(this);
		m_lChildNoObjects.push_back(this);
	}
}

void Simplex::MyOctant::ConstructTree(uint a_nMaxLevel)
{
	//check if OctreeOn is true
	if (m_OctreeOn)
	{
		//subdivide Octant
		Subdivide();

		//call ConstructTree for each child
		for (int i = 0; i < m_uChildren; i++)
		{
			GetChild(i)->ConstructTree();
		}

		//assign dimensions for entities if tree construction is done
		if (this == m_pRoot)
		{
			AssignIDtoEntity();
		}
	}
}

void Simplex::MyOctant::AssignIDtoEntity(void)
{
	//go through the Octants with objects
	for (MyOctant* octant : m_pRoot->m_lChildWithObjects)
	{
		//check if Octant is a leaf
		if (octant->IsLeaf())
		{
			//go through each of the Octant's objects and assign dimension
			for (uint entity : octant->m_EntityList)
			{
				m_pEntityMngr->GetEntity(entity)->AddDimension(octant->m_uID);
			}
		}
	}
}

uint Simplex::MyOctant::GetOctantCount(void)
{
	return m_uOctantCount;
}

void Simplex::MyOctant::RefreshTree(void)
{
	//destroy the tree
	KillBranches();

	//recreate it
	ConstructTree();
}

void Simplex::MyOctant::ToggleOctree(void)
{
	//check if true
	if (m_OctreeOn)
	{
		//set to false
		m_OctreeOn = false;
		//kill the branches
		KillBranches();
	}
		
	else
	{
		//set to true
		m_OctreeOn = true;
		//make tree
		ConstructTree();
	}
}

void Simplex::MyOctant::Release(void)
{
	//release data
	m_pMeshMngr = nullptr;
	m_pEntityMngr = nullptr;
	m_pParent = nullptr;

	//release children
	for (MyOctant* child : m_pChild)
		child = nullptr;

	m_pRoot = nullptr;
}



