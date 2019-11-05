#include <iostream>
#include<stdio.h>

#include<cuda_runtime.h>
#include<Windows.h>
#include<cuda.h>
#include <device_launch_parameters.h>
#include<glm/glm.hpp>

#include<vector>
using namespace std;
#define BLOCK 512


//ÇŒÇÀíËêî
#define K 1.2
//å∏êäó¶
#define lambda 0.99
//ë¨ìxÇÕÇ±Ç¡ÇøÇ…Ç®Ç±Ç§

float (*d_v)[3];
float (*h_v)[3];

float(*d_realpoints)[3];
float(*h_realpoints)[3];
float(*h_MV)[4];
float(*d_MV)[4];

float *h_dbg;
float *d_dbg;


__global__ void d_spring(unsigned int num_points, float(*realpoints)[3], float(*virtualpoints)[3], float(*vel)[3], float dt, float(*M)[4], float* dbg);


void initField(unsigned int num_points,vector<float>& position) {

	h_v =(float(*)[3])malloc(num_points *3 * sizeof(float));
	h_realpoints =(float(*)[3])malloc(num_points *3 * sizeof(float));
	h_MV =(float(*)[4])malloc(4*4 * sizeof(float));
	h_dbg = (float*)malloc(sizeof(float));
	
	cudaMalloc((void**)&d_v, sizeof(float)*3 * num_points);
	cudaMalloc((void**)&d_realpoints, sizeof(float)*3 * num_points);
	cudaMalloc((void**)&d_MV, sizeof(float)*4*4);

	cudaMalloc((void**)&d_dbg, sizeof(float));

	for (int i = 0; i < num_points; i++) {
		for (int j = 0; j < 3; j++) {
			h_v[i][j] = 0.0f;
			h_realpoints[i][j] = position[i * 3 + j];
			//h_realpoints[i][j] = 2.0f;
		}
	}

	cudaMemcpy(d_v, h_v, num_points * 3 * sizeof(float), cudaMemcpyHostToDevice);
	cudaMemcpy(d_realpoints, h_realpoints, num_points * 3 * sizeof(float), cudaMemcpyHostToDevice);
	cudaMemcpy(d_MV, h_MV, 4*4 * sizeof(float), cudaMemcpyHostToDevice);
}

void launchVertexProcess(unsigned int NUM_POINTS,float (*virtualpoints)[3],float dt,glm::mat4& M) {
	dim3 grid(NUM_POINTS /512 + 1, 1);
	dim3 block(512, 1, 1);

	

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			h_MV[i][j]=M[j][i];

		}
		
	}
	cudaMemcpy(d_MV, h_MV, 4 * 4 * sizeof(float), cudaMemcpyHostToDevice);



	d_spring <<< grid, block >>> (NUM_POINTS,d_realpoints,virtualpoints,d_v,dt, d_MV,d_dbg);

	
	//cudaMemcpy(h_dbg, d_dbg, sizeof(float), cudaMemcpyDeviceToHost);
	//cudaMemcpy(h_v, d_v, NUM_POINTS * 3 * sizeof(float), cudaMemcpyDeviceToHost);
	//printf("[%.10f]\n", h_dbg[0]);
	//std::cout << h_realpoints[0][0] << std::endl;

	//printf("finish\n");
}

__global__ void d_spring(unsigned int num_points, float(*realpoints)[3], float(*virtualpoints)[3], float(*vel)[3], float dt, float(*M)[4], float* dbg) {
	unsigned int index = blockDim.x * blockIdx.x + threadIdx.x;


	if (index > num_points) {
		return;
	}

	float p[3];

	p[0] = M[0][0] * realpoints[index][0] + M[0][1] * realpoints[index][1] + M[0][2] * realpoints[index][2] + M[0][3] * 1.0f;
	p[1] = M[1][0] * realpoints[index][0] + M[1][1] * realpoints[index][1] + M[1][2] * realpoints[index][2] + M[1][3] * 1.0f;
	p[2] = M[2][0] * realpoints[index][0] + M[2][1] * realpoints[index][1] + M[2][2] * realpoints[index][2] + M[2][3] * 1.0f;
	float w = M[3][0] * realpoints[index][0] + M[3][1] * realpoints[index][1] + M[3][2] * realpoints[index][2] + M[3][3] * 1.0f;


	for (int i = 0; i < 3; i++) {
		//p[i] /= w;
		vel[index][i] += -K * dt * (virtualpoints[index][i] - p[i]);
		virtualpoints[index][i] += vel[index][i] * dt;
		vel[index][i] *= lambda;

	}
	/*
	if (index == 0) {
		dbg[0] = 2.0f;
	}
	*/

}
