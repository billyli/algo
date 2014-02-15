__kernel void float_kernel(__global const float *input1, 
							__global const float *input2, 
							__global float *out1)
{
	int tid = get_global_id(0);
	out1[tid] = sqrt(pow(input1[tid],2) + pow(input2[tid],2));
	return;
}

__kernel void bitwise_kernel(__global const int *input3, 
							__global const int *input4,
							__global int *out2)
{
	int tid2 = get_global_id(0);
	out2[tid2] = (input3[tid2] << 6) ^ (input4[tid2] << 26);
	return;
}
