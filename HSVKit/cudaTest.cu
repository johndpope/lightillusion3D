
#include <iostream>
#include<stdio.h>

#include<cuda_runtime.h>
#include<Windows.h>
#include<cuda.h>
#include <device_launch_parameters.h>
#define BLOCK 16
#define WIDTH 1024

float h_A[WIDTH * WIDTH];
float h_B[WIDTH * WIDTH];
float h_C[WIDTH * WIDTH];

float* d_A, * d_B, * d_C;



void h_multiply(float* A, float* B, float* C);
__global__ void d_multiply0(float* A, float* B, float* C);




void cudaTest()
{
	unsigned int i;

	cudaMalloc((void**)&d_A, sizeof(float) * WIDTH * WIDTH);
	cudaMalloc((void**)&d_B, sizeof(float) * WIDTH * WIDTH);
	cudaMalloc((void**)&d_C, sizeof(float) * WIDTH * WIDTH);
	for (i = 0; i < (WIDTH * WIDTH); i++) {
		h_A[i] = (float)i;
		h_B[i] = (float)i;
	}

	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);

	LARGE_INTEGER start, end;

	QueryPerformanceCounter(&start);

	cudaMemcpy(d_A, h_A, sizeof(float) * WIDTH * WIDTH, cudaMemcpyHostToDevice);
	cudaMemcpy(d_A, h_A, sizeof(float) * WIDTH * WIDTH, cudaMemcpyHostToDevice);

	dim3 grid(WIDTH / BLOCK, WIDTH / BLOCK);
	dim3 block(BLOCK, BLOCK);

	d_multiply0 <<< grid, block >>> (d_A, d_B, d_C);

	cudaMemcpy(h_C, d_C, sizeof(float) * WIDTH * WIDTH, cudaMemcpyDeviceToHost);

	QueryPerformanceCounter(&end);

	double time = static_cast<double>(end.QuadPart - start.QuadPart) * 1000.0 / freq.QuadPart;
	printf("デバイス計測時間 %lf[ms]\n", time);

	cudaFree(d_A);
	cudaFree(d_B);
	cudaFree(d_C);

	QueryPerformanceCounter(&start);
	h_multiply(h_A, h_B, h_C);
	QueryPerformanceCounter(&end);

	time = static_cast<double>(end.QuadPart - start.QuadPart) * 1000.0 / freq.QuadPart;
	printf("ホスト計測時間 %lf[ms]\n", time);
	printf("ホスト計算結果: %f\n", h_C[WIDTH * WIDTH - 1]);

}

__global__ void d_multiply0(float* A, float* B, float* C) {
	unsigned int r = blockDim.y * blockIdx.y + threadIdx.y;
	unsigned int c = blockDim.x * blockIdx.x + threadIdx.x;
	unsigned int i;
	float tmp = 0.0;
	for (i = 0; i < WIDTH; i++) {
		tmp += A[WIDTH * r + i] * B[WIDTH * i + c];

	}
	C[WIDTH * r + c] = tmp;
}

void h_multiply(float* A, float* B, float* C) {
	unsigned int r, c, i;
	float tmp;
	for (r = 0; r < WIDTH; r++) {
		for (c = 0; c < WIDTH; c++) {
			tmp = 0.0;
			for (i = 0; i < WIDTH; i++) {
				tmp += A[WIDTH * r + i] * B[WIDTH * i + c];

			}
			C[WIDTH * r + c] = tmp;
		}
	}
}