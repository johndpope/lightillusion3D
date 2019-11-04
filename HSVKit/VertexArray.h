// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include<vector>
#include <GLFW/gg.h>
#include <GLFW/glfw3.h>
#include<cuda.h>
#include<cuda_runtime.h>
#include<cuda_gl_interop.h>
using namespace std;

class VertexArray
{
public:
	VertexArray();
	~VertexArray();

	void SetActive();
	void load(vector<float>& position, vector<float>& normal, vector<float>& texcoord, unsigned int numVerts);
	unsigned int GetNumIndices() const { return mNumIndices; }
	unsigned int GetNumVerts() const { return mNumVerts; }
	struct cudaGraphicsResource* GetResource() const { return vbo_res; }
	//頂点リソースをcudaで参照するハンドル
	struct cudaGraphicsResource* vbo_res;
private:
	// How many vertices in the vertex buffer?
	unsigned int mNumVerts;
	// How many indices in the index buffer
	unsigned int mNumIndices;



	GLuint vao;
	//pos=0,normal=1,texcoord=2
	GLuint vbo[3];
	
};