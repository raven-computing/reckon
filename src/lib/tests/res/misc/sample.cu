#include <cuda_runtime.h>

/**
 * A Sample file.
 */

__global__ void add(float* out, const float* in) {
    // A comment
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    out[idx] = in[idx] + 1.0f;
}
