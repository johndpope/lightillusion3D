// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "VertexArray.h"

VertexArray::VertexArray()
{
	// Create vertex array
	
}

void VertexArray::load(vector<float>& position, vector<float>& normal, vector<float>& texcoord, unsigned int numVerts) {
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(3, vbo);


	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, numVerts * 3 * sizeof(float), position.data(), GL_STATIC_DRAW);

	// Create vertex buffer
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, numVerts * 3 * sizeof(float), normal.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, numVerts * 2 * sizeof(float), normal.data(), GL_STATIC_DRAW);




	// Create index buffer


	// Specify the vertex attributes
	// (For now, assume one vertex format)
	// Position is 3 floats
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (GLubyte*)NULL);
	// Normal is 3 floats
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(float), (GLubyte*)NULL);
	// Texture coordinates is 2 floats
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (GLubyte*)NULL);
}

VertexArray::~VertexArray()
{
	
	glDeleteBuffers(3, vbo);
	glDeleteVertexArrays(1, &vao);
}

void VertexArray::SetActive()
{
	glBindVertexArray(vao);
}
