#include "MyMesh.h"

MyMesh::MyMesh()
{
	m_uShaderProgramID = LoadShaders("Shaders//Basic.vs", "Shaders//BasicColor.fs");
}

MyMesh::~MyMesh()
{
}

void MyMesh::CompleteMesh(vector3 a_v3Color)
{
	uint posCount = m_lPos.size();
	uint colCount = m_lCol.size();
	for (uint i = colCount; i < posCount; i++)
	{
		AddVertexColor(a_v3Color);
	}
}



void MyMesh::AddVertexPosition(vector3 a_v3Pos) { m_lPos.push_back(a_v3Pos); m_uVertexCount = m_lPos.size(); }
void MyMesh::AddVertexColor(vector3 a_v3Col) { m_lCol.push_back(a_v3Col); }

void MyMesh::CompileMesh(void)
{
	if (m_bInitialized)
		return;

	CompleteMesh();

	m_lVertex.clear();
	for (uint i = 0; i < m_uVertexCount; i++)
	{
		m_lVertex.push_back(m_lPos[i]);
		m_lVertex.push_back(m_lCol[i]);
	}

	glGenVertexArrays(1, &m_uVAO);//Generate vertex array object

	glGenBuffers(1, &m_uVBO);//Generate Vertex Buffered Object

	glBindVertexArray(m_uVAO);//Bind the VAO
	glBindBuffer(GL_ARRAY_BUFFER, m_uVBO);//Bind the VBO

	//Generate space for the VBO (vertex count times size of vec3)
	glBufferData(GL_ARRAY_BUFFER, m_lVertex.size() * sizeof(glm::vec3), &m_lVertex[0], GL_STATIC_DRAW);

	//count the attributes
	int attributeCount = 2;

	// Position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, attributeCount * sizeof(glm::vec3), (GLvoid*)0);

	// Color attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, attributeCount * sizeof(glm::vec3), (GLvoid*)(1 * sizeof(glm::vec3)));

	glBindVertexArray(0);//Bind the VAO
}

void MyMesh::Render(matrix4 a_m4Model, matrix4 a_m4View, matrix4 a_m4Projection)
{
	if (!m_bInitialized)
		return;

	glUseProgram(m_uShaderProgramID);
	glBindVertexArray(m_uVAO);//Bind the VAO

	 //read uniforms and send values
	GLuint MVP = glGetUniformLocation(m_uShaderProgramID, "MVP");//Model View Projection
	glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(a_m4Projection * a_m4View * a_m4Model));

	GLuint SolidColor = glGetUniformLocation(m_uShaderProgramID, "SolidColor"); //custom color of the shape
	glUniform3f(SolidColor, 1.0f, 1.0f, 1.0f);

	//draw content
	glDrawArrays(GL_TRIANGLES, 0, 3);
}