// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "op/transpose.hpp"

#include <memory>
#include <vector>

#include "ov_models/ov_builders/reshape.hpp"

OPENVINO_SUPPRESS_DEPRECATED_START
namespace ngraph {
namespace onnx_import {
namespace op {
namespace set_1 {
ov::OutputVector transpose(const Node& node) {
    ov::Output<ov::Node> data = node.get_ng_inputs().at(0);

    auto permute_axes = node.get_attribute_value<std::vector<std::size_t>>("perm", {});

    return {(permute_axes.empty()) ? ov::op::util::transpose(data) : ov::op::util::reorder_axes(data, permute_axes)};
}

}  // namespace set_1

}  // namespace op

}  // namespace onnx_import

}  // namespace ngraph
OPENVINO_SUPPRESS_DEPRECATED_END
