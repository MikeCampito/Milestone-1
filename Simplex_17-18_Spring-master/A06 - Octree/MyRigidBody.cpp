#include "MyRigidBody.h"
using namespace Simplex;
//Allocation
void MyRigidBody::Init(void)
{
	m_pMeshMngr = MeshManager::GetInstance();
	m_bVisibleBS = false;
	m_bVisibleOBB = true;
	m_bVisibleARBB = false;

	m_fRadius = 0.0f;

	m_v3ColorColliding = C_RED;
	m_v3ColorNotColliding = C_WHITE;

	m_v3CenterL = ZERO_V3;
	m_v3CenterG = ZERO_V3;
	m_v3MinL = ZERO_V3;
	m_v3MaxL = ZERO_V3;

	m_v3MinG = ZERO_V3;
	m_v3MaxG = ZERO_V3;

	m_v3HalfWidth = ZERO_V3;
	m_v3ARBBSize = ZERO_V3;

	m_m4ToWorld = IDENTITY_M4;

	m_uCollidingCount = 0;
	m_CollidingArray = nullptr;
}
void MyRigidBody::Swap(MyRigidBody& other)
{
	std::swap(m_pMeshMngr, other.m_pMeshMngr);
	std::swap(m_bVisibleBS, other.m_bVisibleBS);
	std::swap(m_bVisibleOBB, other.m_bVisibleOBB);
	std::swap(m_bVisibleARBB, other.m_bVisibleARBB);

	std::swap(m_fRadius, other.m_fRadius);

	std::swap(m_v3ColorColliding, other.m_v3ColorColliding);
	std::swap(m_v3ColorNotColliding, other.m_v3ColorNotColliding);

	std::swap(m_v3CenterL, other.m_v3CenterL);
	std::swap(m_v3CenterG, other.m_v3CenterG);
	std::swap(m_v3MinL, other.m_v3MinL);
	std::swap(m_v3MaxL, other.m_v3MaxL);

	std::swap(m_v3MinG, other.m_v3MinG);
	std::swap(m_v3MaxG, other.m_v3MaxG);

	std::swap(m_v3HalfWidth, other.m_v3HalfWidth);
	std::swap(m_v3ARBBSize, other.m_v3ARBBSize);

	std::swap(m_m4ToWorld, other.m_m4ToWorld);

	std::swap(m_uCollidingCount, other.m_uCollidingCount);
	std::swap(m_CollidingArray, other.m_CollidingArray);
}
void MyRigidBody::Release(void)
{
	m_pMeshMngr = nullptr;
	ClearCollidingList();
}
//Accessors
bool MyRigidBody::GetVisibleBS(void) { return m_bVisibleBS; }
void MyRigidBody::SetVisibleBS(bool a_bVisible) { m_bVisibleBS = a_bVisible; }
bool MyRigidBody::GetVisibleOBB(void) { return m_bVisibleOBB; }
void MyRigidBody::SetVisibleOBB(bool a_bVisible) { m_bVisibleOBB = a_bVisible; }
bool MyRigidBody::GetVisibleARBB(void) { return m_bVisibleARBB; }
void MyRigidBody::SetVisibleARBB(bool a_bVisible) { m_bVisibleARBB = a_bVisible; }
float MyRigidBody::GetRadius(void) { return m_fRadius; }
vector3 MyRigidBody::GetColorColliding(void) { return m_v3ColorColliding; }
vector3 MyRigidBody::GetColorNotColliding(void) { return m_v3ColorNotColliding; }
void MyRigidBody::SetColorColliding(vector3 a_v3Color) { m_v3ColorColliding = a_v3Color; }
void MyRigidBody::SetColorNotColliding(vector3 a_v3Color) { m_v3ColorNotColliding = a_v3Color; }
vector3 MyRigidBody::GetCenterLocal(void) { return m_v3CenterL; }
vector3 MyRigidBody::GetMinLocal(void) { return m_v3MinL; }
vector3 MyRigidBody::GetMaxLocal(void) { return m_v3MaxL; }
vector3 MyRigidBody::GetCenterGlobal(void) { return m_v3CenterG; }
vector3 MyRigidBody::GetMinGlobal(void) { return m_v3MinG; }
vector3 MyRigidBody::GetMaxGlobal(void) { return m_v3MaxG; }
vector3 MyRigidBody::GetHalfWidth(void) { return m_v3HalfWidth; }
matrix4 MyRigidBody::GetModelMatrix(void) { return m_m4ToWorld; }
MyRigidBody::PRigidBody* MyRigidBody::GetColliderArray(void) { return m_CollidingArray; }
uint MyRigidBody::GetCollidingCount(void) { return m_uCollidingCount; }
void MyRigidBody::SetModelMatrix(matrix4 a_m4ModelMatrix)
{
	//to save some calculations if the model matrix is the same there is nothing to do here
	if (a_m4ModelMatrix == m_m4ToWorld)
		return;

	//Assign the model matrix
	m_m4ToWorld = a_m4ModelMatrix;

	m_v3CenterG = vector3(m_m4ToWorld * vector4(m_v3CenterL, 1.0f));

	//Calculate the 8 corners of the cube
	vector3 v3Corner[8];
	//Back square
	v3Corner[0] = m_v3MinL;
	v3Corner[1] = vector3(m_v3MaxL.x, m_v3MinL.y, m_v3MinL.z);
	v3Corner[2] = vector3(m_v3MinL.x, m_v3MaxL.y, m_v3MinL.z);
	v3Corner[3] = vector3(m_v3MaxL.x, m_v3MaxL.y, m_v3MinL.z);

	//Front square
	v3Corner[4] = vector3(m_v3MinL.x, m_v3MinL.y, m_v3MaxL.z);
	v3Corner[5] = vector3(m_v3MaxL.x, m_v3MinL.y, m_v3MaxL.z);
	v3Corner[6] = vector3(m_v3MinL.x, m_v3MaxL.y, m_v3MaxL.z);
	v3Corner[7] = m_v3MaxL;

	//Place them in world space
	for (uint uIndex = 0; uIndex < 8; ++uIndex)
	{
		v3Corner[uIndex] = vector3(m_m4ToWorld * vector4(v3Corner[uIndex], 1.0f));
	}

	//Identify the max and min as the first corner
	m_v3MaxG = m_v3MinG = v3Corner[0];

	//get the new max and min for the global box
	for (uint i = 1; i < 8; ++i)
	{
		if (m_v3MaxG.x < v3Corner[i].x) m_v3MaxG.x = v3Corner[i].x;
		else if (m_v3MinG.x > v3Corner[i].x) m_v3MinG.x = v3Corner[i].x;

		if (m_v3MaxG.y < v3Corner[i].y) m_v3MaxG.y = v3Corner[i].y;
		else if (m_v3MinG.y > v3Corner[i].y) m_v3MinG.y = v3Corner[i].y;

		if (m_v3MaxG.z < v3Corner[i].z) m_v3MaxG.z = v3Corner[i].z;
		else if (m_v3MinG.z > v3Corner[i].z) m_v3MinG.z = v3Corner[i].z;
	}

	//we calculate the distance between min and max vectors
	m_v3ARBBSize = m_v3MaxG - m_v3MinG;
}
//The big 3
MyRigidBody::MyRigidBody(std::vector<vector3> a_pointList)
{
	Init();
	//Count the points of the incoming list
	uint uVertexCount = a_pointList.size();

	//If there are none just return, we have no information to create the BS from
	if (uVertexCount == 0)
		return;

	//Max and min as the first vector of the list
	m_v3MaxL = m_v3MinL = a_pointList[0];

	//Get the max and min out of the list
	for (uint i = 1; i < uVertexCount; ++i)
	{
		if (m_v3MaxL.x < a_pointList[i].x) m_v3MaxL.x = a_pointList[i].x;
		else if (m_v3MinL.x > a_pointList[i].x) m_v3MinL.x = a_pointList[i].x;

		if (m_v3MaxL.y < a_pointList[i].y) m_v3MaxL.y = a_pointList[i].y;
		else if (m_v3MinL.y > a_pointList[i].y) m_v3MinL.y = a_pointList[i].y;

		if (m_v3MaxL.z < a_pointList[i].z) m_v3MaxL.z = a_pointList[i].z;
		else if (m_v3MinL.z > a_pointList[i].z) m_v3MinL.z = a_pointList[i].z;
	}

	//with model matrix being the identity, local and global are the same
	m_v3MinG = m_v3MinL;
	m_v3MaxG = m_v3MaxL;

	//with the max and the min we calculate the center
	m_v3CenterL = (m_v3MaxL + m_v3MinL) / 2.0f;

	//we calculate the distance between min and max vectors
	m_v3HalfWidth = (m_v3MaxL - m_v3MinL) / 2.0f;

	//Get the distance between the center and either the min or the max
	m_fRadius = glm::distance(m_v3CenterL, m_v3MinL);
}
MyRigidBody::MyRigidBody(MyRigidBody const& other)
{
	m_pMeshMngr = other.m_pMeshMngr;

	m_bVisibleBS = other.m_bVisibleBS;
	m_bVisibleOBB = other.m_bVisibleOBB;
	m_bVisibleARBB = other.m_bVisibleARBB;

	m_fRadius = other.m_fRadius;

	m_v3ColorColliding = other.m_v3ColorColliding;
	m_v3ColorNotColliding = other.m_v3ColorNotColliding;

	m_v3CenterL = other.m_v3CenterL;
	m_v3CenterG = other.m_v3CenterG;
	m_v3MinL = other.m_v3MinL;
	m_v3MaxL = other.m_v3MaxL;

	m_v3MinG = other.m_v3MinG;
	m_v3MaxG = other.m_v3MaxG;

	m_v3HalfWidth = other.m_v3HalfWidth;
	m_v3ARBBSize = other.m_v3ARBBSize;

	m_m4ToWorld = other.m_m4ToWorld;

	m_uCollidingCount = other.m_uCollidingCount;
	m_CollidingArray = other.m_CollidingArray;
}
MyRigidBody& MyRigidBody::operator=(MyRigidBody const& other)
{
	if (this != &other)
	{
		Release();
		Init();
		MyRigidBody temp(other);
		Swap(temp);
	}
	return *this;
}
MyRigidBody::~MyRigidBody() { Release(); };
//--- other Methods
void MyRigidBody::AddCollisionWith(MyRigidBody* other)
{
	//if its already in the list return
	if (IsInCollidingArray(other))
		return;
	/*
	check if the object is already in the colliding set, if
	the object is already there return with no changes
	*/

	//insert the entry
	PRigidBody* pTemp;
	pTemp = new PRigidBody[m_uCollidingCount + 1];
	if (m_CollidingArray)
	{
		memcpy(pTemp, m_CollidingArray, sizeof(MyRigidBody*) * m_uCollidingCount);
		delete[] m_CollidingArray;
		m_CollidingArray = nullptr;
	}
	pTemp[m_uCollidingCount] = other;
	m_CollidingArray = pTemp;

	++m_uCollidingCount;
}
void MyRigidBody::RemoveCollisionWith(MyRigidBody* other)
{
	//if there are no dimensions return
	if (m_uCollidingCount == 0)
		return;

	//we look one by one if its the one wanted
	for (uint i = 0; i < m_uCollidingCount; i++)
	{
		if (m_CollidingArray[i] == other)
		{
			//if it is, then we swap it with the last one and then we pop
			std::swap(m_CollidingArray[i], m_CollidingArray[m_uCollidingCount - 1]);
			PRigidBody* pTemp;
			pTemp = new PRigidBody[m_uCollidingCount - 1];
			if (m_CollidingArray)
			{
				memcpy(pTemp, m_CollidingArray, sizeof(uint) * (m_uCollidingCount - 1));
				delete[] m_CollidingArray;
				m_CollidingArray = nullptr;
			}
			m_CollidingArray = pTemp;

			--m_uCollidingCount;
			return;
		}
	}
}
void MyRigidBody::ClearCollidingList(void)
{
	m_uCollidingCount = 0;
	if (m_CollidingArray)
	{
		delete[] m_CollidingArray;
		m_CollidingArray = nullptr;
	}
}
uint MyRigidBody::SAT(MyRigidBody* const a_pOther)
{
	/*
	Your code goes here instead of this comment;

	For this method, if there is an axis that separates the two objects
	then the return will be different than 0; 1 for any separating axis
	is ok if you are not going for the extra credit, if you could not
	find a separating axis you need to return 0, there is an enum in
	Simplex that might help you [eSATResults] feel free to use it.
	(eSATResults::SAT_NONE has a value of 0)
	*/

	float ra, rb; //comparison floats for later calculations
	vector3 globcen1, globcen2; //Global center declarations
	globcen1 = GetCenterGlobal();
	globcen2 = a_pOther->GetCenterGlobal();

	vector3 halfwidth2 = a_pOther->GetHalfWidth(); //Halfwidth shortcut for the second version

												   //Rotation axes declarations
	vector3 axes1[3];
	axes1[0] = vector3(GetModelMatrix()[0][0], GetModelMatrix()[0][1], GetModelMatrix()[0][2]);
	axes1[1] = vector3(GetModelMatrix()[1][0], GetModelMatrix()[1][1], GetModelMatrix()[1][2]);
	axes1[2] = vector3(GetModelMatrix()[2][0], GetModelMatrix()[2][1], GetModelMatrix()[2][2]);
	vector3 axes2[3];
	axes2[0] = vector3(a_pOther->GetModelMatrix()[0][0], a_pOther->GetModelMatrix()[0][1], a_pOther->GetModelMatrix()[0][2]);
	axes2[1] = vector3(a_pOther->GetModelMatrix()[1][0], a_pOther->GetModelMatrix()[1][1], a_pOther->GetModelMatrix()[1][2]);
	axes2[2] = vector3(a_pOther->GetModelMatrix()[2][0], a_pOther->GetModelMatrix()[2][1], a_pOther->GetModelMatrix()[2][2]);

	matrix3 rotmat1, rotmat2;  //Rotmat is R, rotmat2 is AbsR

							   //Nested for loop to fill the rotation matrix (transforms into object one's space)
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			rotmat1[i][j] = glm::dot(axes1[i], axes2[j]);  //Dot product converts the space
		}
	}

	//t gets the distance between centers
	vector3 t = globcen2 - globcen1;

	//translates t into the first object's space
	t = vector3(glm::dot(t, axes1[0]), glm::dot(t, axes1[1]), glm::dot(t, axes1[2]));

	//Creates and fills an absolute value version of rotmat1
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			rotmat2[i][j] = glm::abs(rotmat1[i][j]) + float(DBL_EPSILON); //Epsilon solves the zero problem
		}
	}

	//Test axes L = A0, L = A1, L = A2
	for (int i = 0; i < 3; i++) {

		//Individual if statements because halfwidth[i] isn't valid
		if (i == 0) {
			ra = m_v3HalfWidth.x;
		}
		else if (i == 1) {
			ra = m_v3HalfWidth.y;
		}
		else if (i == 2) {
			ra = m_v3HalfWidth.z;
		}
		rb = halfwidth2.x * rotmat2[i][0] + halfwidth2.y * rotmat2[i][1] + halfwidth2.z * rotmat2[i][2];

		//Individual if statements because t[i] isn't valid
		if (i == 0) {
			if (abs(t.x) > ra + rb) {
				return 1;
			}
		}
		else if (i == 1) {
			if (abs(t.y) > ra + rb) {
				return 1;
			}
		}
		else if (i == 2) {
			if (abs(t.z) > ra + rb) {
				return 1;
			}
		}
	}

	//Test axes L = B0, L = B1, L = B2
	for (int i = 0; i < 3; i++) {
		ra = m_v3HalfWidth.x * rotmat2[0][i] + m_v3HalfWidth.y * rotmat2[1][i] + m_v3HalfWidth.z * rotmat2[2][i];

		//again, individual if statements
		if (i == 0) {
			rb = m_v3HalfWidth.x;
		}
		else if (i == 1) {
			rb = m_v3HalfWidth.y;
		}
		else if (i == 2) {
			rb = m_v3HalfWidth.z;
		}

		if (abs(t.x * rotmat1[0][i] + t.y * rotmat1[1][i] + t.z * rotmat1[2][i]) > ra + rb) {
			return 1;
		}
	}

	//Now begins the cross product tests

	//Test axis L = A0 x B0
	ra = m_v3HalfWidth.y * rotmat2[2][0] + m_v3HalfWidth.z * rotmat2[1][0];
	rb = halfwidth2.y * rotmat2[0][2] + halfwidth2.z * rotmat2[0][1];
	if (abs(t.z * rotmat1[1][0] - t.y * rotmat1[2][0]) > ra + rb) {
		return 1;
	}

	//Test axis L = A0 x B1
	ra = m_v3HalfWidth.y * rotmat2[2][1] + m_v3HalfWidth.z * rotmat2[1][1];
	rb = halfwidth2.x * rotmat2[0][2] + halfwidth2.z * rotmat2[0][0];
	if (abs(t.z * rotmat1[1][1] - t.y * rotmat1[2][1]) > ra + rb) {
		return 1;
	}

	//Test axis L = A0 x B2
	ra = m_v3HalfWidth.y * rotmat2[2][2] + m_v3HalfWidth.z * rotmat2[1][2];
	rb = halfwidth2.x * rotmat2[0][1] + halfwidth2.y * rotmat2[0][0];
	if (abs(t.z * rotmat1[1][2] - t.y * rotmat1[2][2]) > ra + rb) {
		return 1;
	}

	//Test axis L = A1 x B0
	ra = m_v3HalfWidth.x * rotmat2[2][0] + m_v3HalfWidth.z * rotmat2[0][0];
	rb = halfwidth2.y * rotmat2[1][2] + halfwidth2.z * rotmat2[1][1];
	if (abs(t.x * rotmat1[2][0] - t.z * rotmat1[0][0]) > ra + rb) {
		return 1;
	}

	//Test axis L = A1 x B1
	ra = m_v3HalfWidth.x * rotmat2[2][1] + m_v3HalfWidth.z * rotmat2[0][1];
	rb = halfwidth2.x * rotmat2[1][2] + halfwidth2.z * rotmat2[1][0];
	if (abs(t.x * rotmat1[2][1] - t.z * rotmat1[0][1]) > ra + rb) {
		return 1;
	}

	//Test axis L = A1 x B2
	ra = m_v3HalfWidth.x * rotmat2[2][2] + m_v3HalfWidth.z * rotmat2[0][2];
	rb = halfwidth2.x * rotmat2[1][1] + halfwidth2.y * rotmat2[1][0];
	if (abs(t.x * rotmat1[2][2] - t.z * rotmat1[0][2]) > ra + rb) {
		return 1;
	}

	//Test axis L = A2 x B0
	ra = m_v3HalfWidth.x * rotmat2[1][0] + m_v3HalfWidth.y * rotmat2[0][0];
	rb = halfwidth2.y * rotmat2[2][2] + halfwidth2.z * rotmat2[2][1];
	if (abs(t.y * rotmat1[0][0] - t.x * rotmat1[1][0]) > ra + rb) {
		return 1;
	}

	//Test axis L = A2 x B1
	ra = m_v3HalfWidth.x * rotmat2[1][1] + m_v3HalfWidth.y * rotmat2[0][1];
	rb = halfwidth2.x * rotmat2[2][2] + halfwidth2.z * rotmat2[2][0];
	if (abs(t.y * rotmat1[0][1] - t.x * rotmat1[1][1]) > ra + rb) {
		return 1;
	}

	//Test axis L = A2 x B2
	ra = m_v3HalfWidth.x * rotmat2[1][2] + m_v3HalfWidth.y * rotmat2[0][2];
	rb = halfwidth2.x * rotmat2[2][1] + halfwidth2.y * rotmat2[2][0];
	if (abs(t.y * rotmat1[0][2] - t.x * rotmat1[1][2]) > ra + rb) {
		return 1;
	}
	//there is no axis test that separates this two objects
	return 0;
}
bool MyRigidBody::IsColliding(MyRigidBody* const a_pOther)
{
	//check if spheres are colliding
	bool bColliding = true;
	bColliding = (glm::distance(GetCenterGlobal(), a_pOther->GetCenterGlobal()) < m_fRadius + a_pOther->m_fRadius);


	//if they are check the Axis Aligned Bounding Box
	if (bColliding) //they are colliding with bounding sphere
	{
		if (m_checkSAT) {
			int intbool = SAT(a_pOther);
			if (intbool == 1) {
				bColliding = false;
			}
			else {
				this->AddCollisionWith(a_pOther);
				a_pOther->AddCollisionWith(this);
			}
		}
		else {
			if (this->m_v3MaxG.x < a_pOther->m_v3MinG.x) //this to the right of other
				bColliding = false;
			if (this->m_v3MinG.x > a_pOther->m_v3MaxG.x) //this to the left of other
				bColliding = false;

			if (this->m_v3MaxG.y < a_pOther->m_v3MinG.y) //this below of other
				bColliding = false;
			if (this->m_v3MinG.y > a_pOther->m_v3MaxG.y) //this above of other
				bColliding = false;

			if (this->m_v3MaxG.z < a_pOther->m_v3MinG.z) //this behind of other
				bColliding = false;
			if (this->m_v3MinG.z > a_pOther->m_v3MaxG.z) //this in front of other
				bColliding = false;

			if (bColliding) //they are colliding with bounding box also
			{
				this->AddCollisionWith(a_pOther);
				a_pOther->AddCollisionWith(this);
			}
		}
	}
	else //they are not colliding with bounding sphere
	{
		this->RemoveCollisionWith(a_pOther);
		a_pOther->RemoveCollisionWith(this);
	}
	return bColliding;
}

void MyRigidBody::AddToRenderList(void)
{
	if (m_bVisibleBS)
	{
		if (m_uCollidingCount > 0)
			m_pMeshMngr->AddWireSphereToRenderList(glm::translate(m_m4ToWorld, m_v3CenterL) * glm::scale(vector3(m_fRadius)), C_BLUE_CORNFLOWER);
		else
			m_pMeshMngr->AddWireSphereToRenderList(glm::translate(m_m4ToWorld, m_v3CenterL) * glm::scale(vector3(m_fRadius)), C_BLUE_CORNFLOWER);
	}
	if (m_bVisibleOBB)
	{
		if (m_uCollidingCount > 0)
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(m_m4ToWorld, m_v3CenterL) * glm::scale(m_v3HalfWidth * 2.0f), m_v3ColorColliding);
		else
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(m_m4ToWorld, m_v3CenterL) * glm::scale(m_v3HalfWidth * 2.0f), m_v3ColorNotColliding);
	}
	if (m_bVisibleARBB)
	{
		if (m_uCollidingCount > 0)
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(m_v3CenterG) * glm::scale(m_v3ARBBSize), C_YELLOW);
		else
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(m_v3CenterG) * glm::scale(m_v3ARBBSize), C_YELLOW);
	}
}
bool MyRigidBody::IsInCollidingArray(MyRigidBody* a_pEntry)
{
	//see if the entry is in the set
	for (uint i = 0; i < m_uCollidingCount; i++)
	{
		if (m_CollidingArray[i] == a_pEntry)
			return true;
	}
	return false;
}
