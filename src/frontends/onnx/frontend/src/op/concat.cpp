// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "op/concat.hpp"

#include "openvino/op/concat.hpp"
#include "utils/common.hpp"

using namespace ov::op;

OPENVINO_SUPPRESS_DEPRECATED_START
namespace ngraph {
namespace onnx_import {
namespace op {
namespace set_1 {
ov::OutputVector concat(const Node& node) {
    ov::OutputVector inputs{node.get_ng_inputs()};
    std::int64_t axis = node.get_attribute_value<std::int64_t>("axis");
    ov::OutputVector valid_inputs;
    std::copy_if(inputs.begin(), inputs.end(), std::back_inserter(valid_inputs), [](ov::Output<ov::Node>& in) -> bool {
        return !common::is_failsafe_node(in.get_node_shared_ptr());
    });
    return {std::make_shared<v0::Concat>(valid_inputs, axis)};
}

}  // namespace set_1

}  // namespace op

}  // namespace onnx_import

}  // namespace ngraph
OPENVINO_SUPPRESS_DEPRECATED_END
