// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "op/is_finite.hpp"

#include "openvino/opsets/opset10.hpp"

using namespace ov::op;

OPENVINO_SUPPRESS_DEPRECATED_START
namespace ngraph {
namespace onnx_import {
namespace op {
namespace set_1 {

ov::OutputVector is_finite(const Node& node) {
    const auto data = node.get_ng_inputs().at(0);
    return {std::make_shared<v10::IsFinite>(data)};
}

}  // namespace set_1
}  // namespace op
}  // namespace onnx_import
}  // namespace ngraph
OPENVINO_SUPPRESS_DEPRECATED_END
