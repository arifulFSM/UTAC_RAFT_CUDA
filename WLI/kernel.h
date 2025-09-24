#pragma once

extern "C" {
	void cudaAdd(int* c, const int* a, const int* b, size_t size);
}