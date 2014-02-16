__kernel void float_kernel(__global const float *input1, 
							__global const float *input2, 
							__global float *out1)
{
	int tid = get_global_id(0);
	out1[tid] = sqrt(pow(input1[tid],2) + pow(input2[tid],2));
	return;
}
