#include <cuda_runtime.h>

/**
 * The first sample file.
 */
__global__ void add(float* out, const float* in) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    // A very useful comment!
    out[idx] = in[idx] + 1.0f;
}
