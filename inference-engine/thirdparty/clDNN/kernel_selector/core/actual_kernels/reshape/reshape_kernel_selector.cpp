﻿// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "reshape_kernel_selector.h"
#include "reshape_kernel_ref.h"

namespace kernel_selector {

reshape_kernel_selector::reshape_kernel_selector() { Attach<ReshapeKernelRef>(); }

KernelsData reshape_kernel_selector::GetBestKernels(const Params& params, const optional_params& options) const {
    return GetNaiveBestKernel(params, options, KernelType::RESHAPE);
}
}  // namespace kernel_selector