#include"kernel.h"
#include "cuda_runtime.h"
#include "device_launch_parameters.h"

__global__ void addKernel(int* c, const int* a, const int* b) {
	int i = threadIdx.x;
	c[i] = a[i] + b[i];
}

extern "C" void cudaAdd(int* c, const int* a, const int* b, size_t size) {
	int* dev_a = nullptr;
	int* dev_b = nullptr;
	int* dev_c = nullptr;

	// Allocate GPU buffers for three vectors (two input, one output)
	cudaMalloc(&dev_a, size * sizeof(int));
	cudaMalloc(&dev_b, size * sizeof(int));
	cudaMalloc(&dev_c, size * sizeof(int));

	// Copy input vectors from host memory to GPU buffers
	cudaMemcpy(dev_a, a, size * sizeof(int), cudaMemcpyHostToDevice);
	cudaMemcpy(dev_b, b, size * sizeof(int), cudaMemcpyHostToDevice);

	// Launch a kernel on the GPU with one thread for each element
	//addKernel <<<1, size >>> (dev_c, dev_a, dev_b);

	//	copy output vector from GPU buffer to host memory
	cudaMemcpy(c, dev_c, size * sizeof(int), cudaMemcpyDeviceToHost);


}