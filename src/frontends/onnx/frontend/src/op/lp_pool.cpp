// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "op/lp_pool.hpp"

#include "exceptions.hpp"
#include "openvino/frontend/exception.hpp"
#include "openvino/op/concat.hpp"
#include "openvino/op/constant.hpp"
#include "openvino/op/reshape.hpp"
#include "ov_models/ov_builders/norm.hpp"
#include "ov_models/ov_builders/split.hpp"
#include "utils/common.hpp"

using namespace ov::op;

OPENVINO_SUPPRESS_DEPRECATED_START
namespace ngraph {
namespace onnx_import {
namespace op {
namespace set_1 {
ov::OutputVector global_lp_pool(const Node& node) {
    const ov::Output<ov::Node> data{node.get_ng_inputs().at(0)};
    const std::size_t channel_axis{1};

    const auto data_shape = data.get_partial_shape();
    FRONT_END_GENERAL_CHECK(data_shape.rank().is_static(), "Rank of input data must be static");
    FRONT_END_GENERAL_CHECK(data_shape.rank().get_length() >= 2, "Rank of input data must be greater or equal to 2");
    FRONT_END_GENERAL_CHECK(data_shape[0].is_static(), "First dimension of input data must be static");
    FRONT_END_GENERAL_CHECK(data_shape[channel_axis].is_static(), "Channel dimension of intput data must be static");

    const std::size_t channels_count = data_shape[channel_axis].get_length();
    const std::int64_t p_norm{node.get_attribute_value<std::int64_t>("p", 2)};

    CHECK_VALID_NODE(node, p_norm >= 0, "Only positive (including zero) values are supported for 'p' attribute.");

    ov::OutputVector slices = ov::op::util::split(data, channels_count, channel_axis);

    for (auto& slice : slices) {
        // all dimensions except spatial/feature
        const auto reduction_axes = common::get_monotonic_range_along_node_rank(data, 2);

        slice = ov::op::util::lp_norm(slice, reduction_axes, static_cast<std::size_t>(p_norm));

        // output shape is all ones except N channel
        Shape output_shape(data_shape.rank().get_length(), 1);
        output_shape.at(0) = data_shape[0].get_length();

        const auto reshape_pattern = v0::Constant::create(ov::element::i64, Shape{output_shape.size()}, output_shape);

        slice = std::make_shared<v1::Reshape>(slice, reshape_pattern, false);
    }

    return {std::make_shared<v0::Concat>(slices, channel_axis)};
}

}  // namespace set_1

}  // namespace op

}  // namespace onnx_import

}  // namespace ngraph
OPENVINO_SUPPRESS_DEPRECATED_END
