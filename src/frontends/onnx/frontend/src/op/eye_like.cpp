// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "op/eye_like.hpp"

#include "exceptions.hpp"
#include "openvino/op/constant.hpp"
#include "openvino/op/eye.hpp"
#include "openvino/op/gather.hpp"
#include "openvino/op/shape_of.hpp"
#include "utils/common.hpp"

using namespace ov::op;

OPENVINO_SUPPRESS_DEPRECATED_START
namespace ngraph {
namespace onnx_import {
namespace op {
namespace detail {
namespace {

/// \brief Split a shape returned by a ShapeOf operation into two outputs: width and height.
ov::OutputVector get_shape_width_and_height(const ov::Output<ov::Node>& shape) {
    const auto axis = v0::Constant::create(ov::element::i64, {1}, {0});
    const auto height = std::make_shared<v8::Gather>(shape, v0::Constant::create(ov::element::i64, {1}, {0}), axis);
    const auto width = std::make_shared<v8::Gather>(shape, v0::Constant::create(ov::element::i64, {1}, {1}), axis);

    return {width, height};
}
}  // namespace
}  // namespace detail

namespace set_1 {

ov::OutputVector eye_like(const Node& node) {
    const auto input = node.get_ng_inputs().at(0);

    const auto& input_rank = input.get_partial_shape().rank();
    CHECK_VALID_NODE(node,
                     input_rank.compatible(ov::Rank(2)),
                     "The provided shape rank: ",
                     input_rank.get_length(),
                     " is unsupported, only 2D shapes are supported");

    ov::element::Type target_type;
    if (node.has_attribute("dtype")) {
        std::int64_t dtype = node.get_attribute_value<std::int64_t>("dtype");
        target_type = common::get_ov_element_type(dtype);
    } else {
        target_type = input.get_element_type();
    }

    const auto input_shape = std::make_shared<ov::op::v0::ShapeOf>(input);
    const auto dims = detail::get_shape_width_and_height(input_shape);
    const auto width = dims.at(0);
    const auto height = dims.at(1);
    const auto k =
        ov::op::v0::Constant::create(ov::element::i64, {1}, {node.get_attribute_value<std::int64_t>("k", 0)});

    const auto output = std::make_shared<v9::Eye>(height, width, k, target_type);

    return {output};
}

}  // namespace set_1
}  // namespace op
}  // namespace onnx_import
}  // namespace ngraph
OPENVINO_SUPPRESS_DEPRECATED_END
