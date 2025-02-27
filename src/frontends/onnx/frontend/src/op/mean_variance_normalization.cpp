// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "op/mean_variance_normalization.hpp"

#include "openvino/op/constant.hpp"
#include "openvino/op/mvn.hpp"
#include "validation_util.hpp"

using namespace ov::op;

OPENVINO_SUPPRESS_DEPRECATED_START
namespace ngraph {
namespace onnx_import {
namespace op {
namespace set_1 {
ov::OutputVector mean_variance_normalization(const Node& node) {
    auto data = node.get_ng_inputs().at(0);
    bool across_channels = node.get_attribute_value<std::int64_t>("across_channels", 0);
    bool normalize_variance = node.get_attribute_value<std::int64_t>("normalize_variance", 1);

    return {std::make_shared<v0::MVN>(data, across_channels, normalize_variance)};
}

}  // namespace set_1

namespace set_9 {
ov::OutputVector mean_variance_normalization(const Node& node) {
    auto data = node.get_ng_inputs().at(0);
    auto axes = node.get_attribute_value<std::vector<std::int64_t>>("axes", {0, 2, 3});
    const std::vector<std::size_t> normalized_axes =
        ov::util::normalize_axes(node.get_description(), axes, data.get_partial_shape().rank());
    auto const_axes = v0::Constant::create(ov::element::i64, Shape{normalized_axes.size()}, normalized_axes);
    return {std::make_shared<v6::MVN>(data, const_axes, true, 1e-09f, ov::op::MVNEpsMode::OUTSIDE_SQRT)};
}

}  // namespace set_9

}  // namespace op

}  // namespace onnx_import

}  // namespace ngraph
OPENVINO_SUPPRESS_DEPRECATED_END
