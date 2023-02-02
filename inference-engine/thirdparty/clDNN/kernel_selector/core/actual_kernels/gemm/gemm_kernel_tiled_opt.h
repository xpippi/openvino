// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include "gemm_kernel_base.h"
#include <vector>

namespace kernel_selector {
class GemmKernelTiledOpt : public GemmKernelBase {
public:
    using Parent = GemmKernelBase;

    mutable struct GemmTuningData {
        size_t simd_size = 8;
        size_t tile_m_size = 1;
        size_t tile_k_size = 1;
        size_t tile_n_size = 1;
    } tuning_data;

    GemmKernelTiledOpt() : GemmKernelBase("gemm_tiled_opt") {}

    KernelsData GetKernelsData(const Params& params, const optional_params& options) const override;
    KernelsPriority GetKernelsPriority(const Params& params, const optional_params& options) const override;
    ParamsKey GetSupportedKey() const override;

protected:
    std::vector<FusedOpType> GetSupportedFusedOps() const override {
        return { FusedOpType::QUANTIZE,
                 FusedOpType::ACTIVATION,
                 FusedOpType::SCALE,
                 FusedOpType::ELTWISE };
    }
    bool Validate(const Params& params, const optional_params& options) const override;
    DispatchData SetDefault(const gemm_params& params) const override;
    JitConstants GetJitConstants(const gemm_params& params) const override;
    GemmTuningData SetTuningParams(const gemm_params& params) const;
};
}  // namespace kernel_selector
