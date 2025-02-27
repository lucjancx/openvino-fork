// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "op/global_max_pool.hpp"

#include "openvino/op/constant.hpp"
#include "openvino/op/range.hpp"
#include "openvino/op/reduce_max.hpp"
#include "openvino/op/shape_of.hpp"
#include "openvino/op/squeeze.hpp"

using namespace ov::op;

OPENVINO_SUPPRESS_DEPRECATED_START
namespace ngraph {
namespace onnx_import {
namespace op {
namespace set_1 {
ov::OutputVector global_max_pool(const Node& node) {
    // Generate axes for reduce operation which contain all spatial dims indexes.
    // Examples:
    // Input shape: [N, C, H, W]
    // Input spatial dimensions are H and W
    // Expected spatial dims indexes: [2, 3]
    //
    // Input shape: [N, C, H, W, D]
    // Input spatial dimensions are H, W and D
    // Expected spatial dims indexes: [2, 3, 4]
    auto data = node.get_ng_inputs()[0];

    const auto zero_node = v0::Constant::create(ov::element::i64, Shape{}, {0});
    const auto one_node = v0::Constant::create(ov::element::i64, Shape{}, {1});
    const auto two_node = v0::Constant::create(ov::element::i64, Shape{}, {2});

    const auto data_shape = std::make_shared<v3::ShapeOf>(data);
    const auto data_rank = std::make_shared<v3::ShapeOf>(data_shape);
    const auto data_rank_as_scalar = std::make_shared<v0::Squeeze>(data_rank);

    const auto reduce_axes = std::make_shared<v4::Range>(two_node, data_rank_as_scalar, one_node, ov::element::i64);

    return {std::make_shared<v1::ReduceMax>(data, reduce_axes, true)};
}

}  // namespace set_1

}  // namespace op

}  // namespace onnx_import

}  // namespace ngraph
OPENVINO_SUPPRESS_DEPRECATED_END
