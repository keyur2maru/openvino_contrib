// Copyright (C) 2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include "cuda_type_traits.hpp"
#include "error.hpp"

namespace CUDAPlugin {
namespace kernel {

class Swish {
public:
    Swish(Type_t element_type, size_t max_threads_per_block);
    Swish(Swish&&) = default;
    Swish& operator=(Swish&&) = default;

    void operator()(cudaStream_t stream, const void* in, size_t num_elements, void* out, double beta) const;

private:
    Type_t element_type_;
    size_t max_threads_per_block_;
};

}  // namespace kernel
}  // namespace CUDAPlugin