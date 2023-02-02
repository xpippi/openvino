// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <transformations_visibility.hpp>

#include <ngraph/op/op.hpp>
#include "load.hpp"

namespace ngraph {
namespace snippets {
namespace op {

/**
 * @interface BlockedLoad
 * @brief Generated by Canonicalization step for blocked data (NCHW<X>c) to be loaded
 * @ingroup snippets
 */
class TRANSFORMATIONS_API BlockedLoad : public Load {
public:
    NGRAPH_RTTI_DECLARATION;

    BlockedLoad(const Output<Node>& x);
    BlockedLoad() = default;

    std::shared_ptr<Node> clone_with_new_inputs(const OutputVector& new_args) const override {
        check_new_args_count(this, new_args);
        return std::make_shared<BlockedLoad>(new_args.at(0));
    }
};

} // namespace op
} // namespace snippets
} // namespace ngraph