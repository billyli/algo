#define NUM_KERNELS 2
#define GROUP_SIZE 256

#ifndef TEST_H
#define TEST_H

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <fstream>
#include <ostream>
#include <sstream>
#include <CL/opencl.h>
#include <ctime>
#include <cstdlib>
#include <ctime>

class test
{

public:
	cl_int errNum;
	cl_int myDevice;
	cl_device_type myType;

	clock_t start;
	clock_t end;
	cl_uint length;
	cl_float *inputA;
	cl_float *inputB;
	cl_float *outputA;




	cl_uint numPlatforms;
	cl_uint numDevices;
	cl_device_id *deviceIDs;
	cl_platform_id *platformIDs;
	size_t valueSize;
	cl_char * value;
	cl_uint maxComputeUnits;

	cl_context context;
	cl_command_queue commandQueue;

	cl_mem inputABuffer;
	cl_mem inputBBuffer;
	cl_mem outputABuffer;
	cl_mem inputCBuffer;
	cl_mem inputDBuffer;
	cl_mem outputBBuffer;

	cl_program program;
	cl_kernel kernel[NUM_KERNELS];
	size_t globalThreads[2];
	size_t localThreads[2];

	/*
	size_t maxWorkGroupSize;
	size_t kernelWorkGroupSize;
	cl_uint maxDimensions;
	size_t * maxWorkItemSizes;
	cl_ulong totalLocalMemory;
	cl_ulong usedLocalMemory;
	cl_ulong availableLocalMemory;
	cl_ulong neededLocalMemory;
	*/
	test(){
		inputA = NULL;
		inputB = NULL;
		length = 1024;
		context = NULL;
	}

	int initialize();

	int createContext();

	int selectDevice();
	
	int setupCQ();

	int runCLKernels();

	int setupArray();

	inline void checkErr(cl_int, const char *);

	template<typename T>
	int fillRandom(T*, const int, const T, const T, unsigned int);

};
#endif