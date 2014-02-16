/************************************************************************************************/
/*										OpenCLProject1											*/			
/*		Function: addition of two arrays filled with random float using CPU/GPU OpenCL			*/
/*																								*/
/*																								*/
/*																								*/
/************************************************************************************************/

#include "test.h"

using namespace std;

// method to fill array with random float
template<typename T>
int test::fillRandom(T *arrayPtr, const int width, const T rangeMin, const T rangeMax, unsigned int seed)
{
	if(!arrayPtr){
		cerr << "ERROR: Cannot fill array." << endl;
		exit(EXIT_FAILURE);
	}
	if(!seed)
	{
		seed = (unsigned int)time(NULL);
	}
	srand(seed);
	float range = float(rangeMax - rangeMin)+1.00000f;
	for(int j = 0; j < width; j++)
	{
		arrayPtr[j] = rangeMin + T(range*rand()/(RAND_MAX + 1.00000f));
		// cout << arrayPtr[j] << endl;
	}

	return CL_SUCCESS;
}

// error handling
inline void test::checkErr(cl_int err, const char * name){
	if(err != CL_SUCCESS)
	{
		cerr << "ERROR: " << name << " (" << err << ")" << endl;
		exit(EXIT_FAILURE);
	}
}

// initialize and list platform and devices
// TO-DO: Add support for more platforms (AMD, NVIDIA)
int test::initialize(){

	errNum = clGetPlatformIDs(0, NULL, &numPlatforms);
	checkErr((errNum != CL_SUCCESS) ? errNum : 
		(numPlatforms <= 0 ? -1 : CL_SUCCESS),
			"clGetPlatformIDs");
	platformIDs = (cl_platform_id*)alloca(sizeof(cl_platform_id)*numPlatforms);
	errNum = clGetPlatformIDs(numPlatforms, platformIDs, NULL);
	checkErr(
		(errNum != CL_SUCCESS) ? errNum : 
		(numPlatforms <= 0 ? -1 : CL_SUCCESS), "clGetPlatformIDs");
	deviceIDs = NULL;

	cout << "Number of Platforms: " << numPlatforms << endl << endl;
	
	if(numPlatforms == 0){
		cerr << "No availabile OpenCL platform exists.";
		exit(EXIT_FAILURE);
	}

	// List all devices informations

	clGetDeviceIDs(platformIDs[0], CL_DEVICE_TYPE_ALL, 0, NULL, &numDevices);
	deviceIDs = (cl_device_id*)alloca(sizeof(cl_device_id)*numDevices);
	clGetDeviceIDs(platformIDs[0], CL_DEVICE_TYPE_ALL, numDevices, deviceIDs, NULL);

	cout << "Number of Devices: " << numDevices << " Platform #: " << platformIDs[0] << endl << endl;   // debug

	if(numDevices > 0){
		for(cl_uint j = 0; j < numDevices; j++){
			cout << "Device # " << j << endl;

			// device name
			clGetDeviceInfo(deviceIDs[j], CL_DEVICE_NAME, 0, NULL, &valueSize);
			value = (cl_char*)alloca(valueSize);
			clGetDeviceInfo(deviceIDs[j], CL_DEVICE_NAME, valueSize, value, NULL);
			cout << " Device: " << value << endl;

			// hardware vendor
			clGetDeviceInfo(deviceIDs[j], CL_DEVICE_VENDOR, 0, NULL, &valueSize);
			value = (cl_char*)alloca(valueSize);
			clGetDeviceInfo(deviceIDs[j], CL_DEVICE_VENDOR, valueSize, value, NULL);
			cout << " Vendor: " << value << endl;

			// hardware device version
			clGetDeviceInfo(deviceIDs[j], CL_DEVICE_VERSION, 0, NULL, &valueSize);
			value = (cl_char*)alloca(valueSize);
			clGetDeviceInfo(deviceIDs[j], CL_DEVICE_VERSION, valueSize, value, NULL);
			cout << " Hardware version: " << value << endl;
			
			// software driver version
			clGetDeviceInfo(deviceIDs[j], CL_DRIVER_VERSION, 0, NULL, &valueSize);
			value = (cl_char*)alloca(valueSize);
			clGetDeviceInfo(deviceIDs[j], CL_DRIVER_VERSION, valueSize, value, NULL);
			cout << " Driver Version: " << value << endl;

			// OPENCL version
			clGetDeviceInfo(deviceIDs[j], CL_DEVICE_OPENCL_C_VERSION, 0, NULL, &valueSize);
			value = (cl_char*)alloca(valueSize);
			clGetDeviceInfo(deviceIDs[j], CL_DEVICE_OPENCL_C_VERSION, valueSize, value, NULL);
			cout << " OpenCL C Version: " << value << endl;

			// parallel computer units
			clGetDeviceInfo(deviceIDs[j], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(maxComputeUnits), &maxComputeUnits, NULL);
			cout << " Parallel Compute Units: " << maxComputeUnits << endl << endl;
		}
	}
	selectDevice();
	return CL_SUCCESS;
}

// picks device and initializes
// NOTE: Must be run inside getContext() or it generates a wrong platformIDs[0]
// Need to be fixed in the future
int test::selectDevice(){
	cout << endl;
	cout << "Select a device: ";
	cin >> myDevice;

	errNum = clGetDeviceInfo(deviceIDs[myDevice], CL_DEVICE_TYPE, sizeof(cl_device_type), &myType, NULL);
	checkErr(errNum, "clGetDeviceInfo: After device selection");

	cout << "Using " << ((myType == CL_DEVICE_TYPE_CPU) ? "CPU" : "GPU") << endl;
	cout << "Platform ID: " << platformIDs[0] << endl;

	errNum = clGetDeviceIDs(platformIDs[0], myType, numDevices, &deviceIDs[myDevice], NULL);
	checkErr(errNum, "clGetDeviceIDs: After device selection");
	start = clock();
	createContext();
	end = clock();
	return CL_SUCCESS;
}

// create device context w.r.t. device choice
int test::createContext(){
	size_t size;
	cl_device_id *devices;

	cl_context_properties cprops [] = 
	{
		CL_CONTEXT_PLATFORM,
		(cl_context_properties)platformIDs[0],
		0
	};

	context = clCreateContextFromType(cprops, myType, NULL, NULL, &errNum);
	checkErr(errNum, "clCreateContextFromType: createContext()");

	errNum = clGetContextInfo(context, CL_CONTEXT_DEVICES, 0, NULL, &size);
	checkErr(errNum, "clGetContextInfo: createContext() - 1");
	devices = new cl_device_id[size / sizeof(cl_device_id)];
	errNum = clGetContextInfo(context, CL_CONTEXT_DEVICES, size, devices, NULL);
	checkErr(errNum, "clGetContextInfo: createContext() - 2");
	
	switch(myType){
		case CL_DEVICE_TYPE_CPU:
		{
			cout << "Device #" << myDevice << endl;
			cout << " CL_DEVICE_TYPE_CPU" << endl;
			break;
		}
		case CL_DEVICE_TYPE_GPU:
		{
			cout << "Device #" << myDevice << endl;
			cout << " CL_DEVICE_TYPE_GPU" << endl;
			break;
		}
	}

	// If myDevice == 1, deviceIDs[1] is correct but deviceIDs[0] becomes rubbish
	// Although it does not affect the program, this bug should be fixed in the 
	// future

	*deviceIDs = devices[myDevice]; 
	delete [] devices;   // clear temp
	setupCQ();
	return CL_SUCCESS;
}


// create commandqueue
// TO-DO: Fallback for CPU and GPU if one fails
int test::setupCQ(){
	commandQueue = clCreateCommandQueue(context, deviceIDs[myDevice], 0, &errNum);
	checkErr(errNum, "clCreateCommandQueue: setupCQ()");
	
	runCLKernels();

	return CL_SUCCESS;
}

// load and run kernels
int test::runCLKernels(){
	const char * filename = "test.cl";

	ifstream File(filename, ios::in);
	if(!File.is_open()){
		cerr << "Failed to load kernel: " << filename << endl;
		return NULL;
	}

	//cout << "Kernel loaded." << endl;
	
	ostringstream oss;
	oss << File.rdbuf();

	string srcStdStr = oss.str();
	const char *srcStr = srcStdStr.c_str();
	size_t sourceSize  = srcStdStr.length();
	program = clCreateProgramWithSource(context, 1, &srcStr, &sourceSize, &errNum);
	checkErr(errNum, "clCreateProgramWithSource: runCLKernels()");

	errNum = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
	checkErr(errNum, "clBuildProgram: runCLKernels()");

	kernel[0] = clCreateKernel(program, "float_kernel", NULL);
	if(kernel[0] == NULL){
		cerr << "Failed to create kernel" << endl;
		return 1;
	}

	// setupArray
	cl_uint size1 = length * sizeof(cl_float);
	inputA = (cl_float*)alloca(size1);
	inputB = (cl_float*)alloca(size1);
	outputA = (cl_float*)alloca(size1);

	fillRandom<cl_float>(inputA, length, 0, 20000.00000f, 1);
	fillRandom<cl_float>(inputB, length , 0, 20000.00000f, 2);

	/*   debug
	for(int i = 0; i < length; i++){
		cout <<"inputA: " << inputA[i] << endl;
	}
	*/
	//end of setup Array

	inputABuffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
									sizeof(cl_float) * length, inputA, NULL);
	inputBBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
									sizeof(cl_float) * length, inputB, NULL);
	outputABuffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
									sizeof(cl_float) * length, outputA, NULL);


	if(inputABuffer == NULL || inputBBuffer == NULL || outputABuffer == NULL){
		cerr << "Error creating memory objects" << endl;
		/* insert cleanup() */
		return false;
	}

	errNum = clSetKernelArg(kernel[0], 0, sizeof(cl_mem), &inputABuffer);
	errNum |= clSetKernelArg(kernel[0], 1, sizeof(cl_mem), &inputBBuffer);
	errNum |= clSetKernelArg(kernel[0], 2, sizeof(cl_mem), &outputABuffer);

	if(errNum != CL_SUCCESS){
		cerr << "Error setting kernel[0] arguments" << endl;
		/* insert cleanup() */
		return 1;
	}

	size_t globalWorkSize[1] = {length};
	size_t localWorkSize[1] = {1};

	errNum = clEnqueueNDRangeKernel(commandQueue, kernel[0], 1, NULL, globalWorkSize, localWorkSize, 0, NULL, NULL);
	if(errNum != CL_SUCCESS){
		cerr << "Error queuing kernel for execution" << endl;
		/* insert cleanup() */
		return 1;
	}

	errNum = clEnqueueReadBuffer(commandQueue, outputABuffer, CL_TRUE, 0, length*sizeof(cl_float), outputA, 0, NULL, NULL);
	if(errNum != CL_SUCCESS){
		cerr << "Error reading result buffer" << endl;
		/* insert cleanup() */
		return 1;
	}


	//end = clock();


	/*
	// print result (debug)
	// print after clock has stopped
	for(int i = 0; i < length; i++){
		cout.precision(10);
		cout << "Pythagoras's Theorem - " << i << ": " << inputA[i] << " and " << inputB[i] << ": "<< outputA[i] << endl;
	}
	*/
	return CL_SUCCESS;
}


// good stuff
int main(int argc, char ** argv){
	test CLTest;
	CLTest.initialize();
	cout << endl << endl;
	cout << "Runtime: " << ((double)(CLTest.end-CLTest.start)/1000) << "s" << endl;
	system("pause");

}