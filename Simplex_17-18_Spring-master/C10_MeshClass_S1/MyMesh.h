/*----------------------------------------------
Programmer: Alberto Bobadilla (labigm@gmail.com)
Date: 2018/02
----------------------------------------------*/
#ifndef __MYMESH_H_
#define __MYMESH_H_

#include "Definitions.h"

class MyMesh
{
public:

	bool m_bInitialized = false;
	uint m_uVertexCount = 0;

	GLuint  m_uVAO = 0;
	GLuint  m_uVBO = 0;
	GLuint m_uShaderProgramID = 0;

	std::vector<vector3> m_lVertex;
	std::vector<vector3> m_lPos;
	std::vector<vector3> m_lCol;

	MyMesh();
	~MyMesh();

	void AddVertexPosition(vector3 a_v3Value);
	void AddVertexColor(vector3 a_v3Value);

	void CompileMesh(void);
	void CompleteMesh(vector3 a_v3Color = vector3(0.25f, 0.25f, 0.25f));
	void Render(matrix4 a_m4Model, matrix4 a_mView, matrix4 a_mProjection);
};



#endif //__MYMESH_H_