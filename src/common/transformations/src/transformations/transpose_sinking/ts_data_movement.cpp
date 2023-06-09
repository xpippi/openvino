// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "transformations/transpose_sinking/ts_data_movement.hpp"

#include "itt.hpp"
#include "openvino/op/util/op_types.hpp"
#include "openvino/opsets/opset10.hpp"
#include "openvino/pass/pattern/op/wrap_type.hpp"
#include "openvino/util/common_util.hpp"
#include "transformations/rt_info/transpose_sinking_attr.hpp"
#include "transformations/transpose_sinking/ts_utils.hpp"

using namespace ov;
using namespace ov::opset10;
using namespace ov::pass::pattern;
using namespace ov::pass::transpose_sinking;
using namespace ov::pass::transpose_sinking::utils;

namespace {

std::vector<size_t> get_indices_by_op_type(const std::shared_ptr<Node>& main_node) {
    if (as_type_ptr<Pad>(main_node)) {
        return {1, 2};
    } else if (as_type_ptr<BatchToSpace>(main_node) || as_type_ptr<SpaceToBatch>(main_node)) {
        return {1, 2, 3};
    } else {
        return {};
    }
}

}  // namespace

TSDataMovementForward::TSDataMovementForward() {
    MATCHER_SCOPE(TSDataMovementForward);
    auto const_label = wrap_type<Constant>();
    auto transpose_label = wrap_type<Transpose>({any_input(), const_label});
    auto main_node_label = wrap_type<Pad, BatchToSpace, SpaceToBatch, ReverseSequence>(
        {transpose_label, any_input(), any_input(), any_input()});

    matcher_pass_callback matcher_pass_callback = [=](Matcher& m) {
        const auto& pattern_to_node = m.get_pattern_map();

        auto& main_node = pattern_to_node.at(main_node_label);
        if (transformation_callback(main_node)) {
            return false;
        }

        auto transpose = std::dynamic_pointer_cast<Transpose>(pattern_to_node.at(transpose_label));
        if (!transpose) {
            return false;
        }

        auto transpose_const = as_type_ptr<Constant>(pattern_to_node.at(const_label));
        if (!transpose_const) {
            return false;
        }

        // remove Transpose on 1st input:
        auto transpose_parent = main_node->input_value(0).get_node()->input_value(0);
        main_node->input(0).replace_source_output(transpose_parent);

        const auto transpose_axis_order = transpose_const->get_axis_vector_val();
        const auto reversed_transpose_order = ReverseTransposeOrder(transpose_axis_order);
        auto axis = std::make_shared<Constant>(element::i32, Shape{}, 0);

        const auto& indices = get_indices_by_op_type(main_node);
        for (const auto& idx : indices) {
            main_node->input(idx).replace_source_output(
                ChangeValuesOrder(main_node->input_value(idx), reversed_transpose_order, axis));
        }

        if (auto reverse_seq = as_type_ptr<ReverseSequence>(main_node)) {
            reverse_seq->set_batch_axis(transpose_axis_order[reverse_seq->get_batch_axis()]);
            reverse_seq->set_sequence_axis(transpose_axis_order[reverse_seq->get_sequence_axis()]);
        }
        main_node->validate_and_infer_types();
        TransposeInputsInfo transpose_input_info = {transpose, transpose_const, 0};
        for (auto& new_node : sink_forward::InsertOutputTransposes(main_node, transpose_input_info)) {
            register_new_node(new_node);
            UpdateForwardSinkingAbility(new_node);
        }
        return true;
    };

    auto m = std::make_shared<Matcher>(main_node_label, matcher_name);
    register_matcher(m, matcher_pass_callback);
}

TSDataMovementBackward::TSDataMovementBackward() {
    MATCHER_SCOPE(TSDataMovementBackward);

    auto main_node_label =
        wrap_type<Pad, BatchToSpace, SpaceToBatch, ReverseSequence>([](const Output<Node>& output) -> bool {
            return has_static_rank()(output) && HasSameOutputTransposeNodes(output);
        });

    auto transpose_const_label = wrap_type<Constant>();

    auto transpose_label =
        wrap_type<Transpose>({main_node_label, transpose_const_label}, [](const Output<Node>& output) -> bool {
            return has_static_rank()(output) && is_sinking_node(output);
        });

    matcher_pass_callback matcher_pass_callback = [=](Matcher& m) {
        const auto& pattern_to_output = m.get_pattern_value_map();
        auto transpose_const = as_type_ptr<Constant>(pattern_to_output.at(transpose_const_label).get_node_shared_ptr());
        auto transpose = pattern_to_output.at(transpose_label).get_node_shared_ptr();
        auto main_node = pattern_to_output.at(main_node_label).get_node_shared_ptr();
        if (transformation_callback(main_node)) {
            return false;
        }

        for (auto& new_node : sink_backward::InsertTransposeBeforeNode(main_node,
                                                                       transpose_const,
                                                                       /* input_indexes= */ {0})) {
            register_new_node(new_node);
        }

        // remove output transposes
        RemoveSingleOutputConsumers(main_node);
        SwapNames(main_node, transpose);
        const auto transpose_axis_order = transpose_const->get_axis_vector_val();
        const auto reversed_transpose_order = ReverseTransposeOrder(transpose_axis_order);
        auto axis = std::make_shared<Constant>(element::i32, Shape{}, 0);
        const auto& indices = get_indices_by_op_type(main_node);
        for (const auto& idx : indices) {
            main_node->input(idx).replace_source_output(
                ChangeValuesOrder(main_node->input_value(idx), transpose_axis_order, axis));
        }

        if (auto reverse_seq = as_type_ptr<ReverseSequence>(main_node)) {
            reverse_seq->set_batch_axis(reversed_transpose_order[reverse_seq->get_batch_axis()]);
            reverse_seq->set_sequence_axis(reversed_transpose_order[reverse_seq->get_sequence_axis()]);
        }
        main_node->validate_and_infer_types();
        return true;
    };

    auto m = std::make_shared<Matcher>(transpose_label, matcher_name);
    register_matcher(m, matcher_pass_callback);
}
