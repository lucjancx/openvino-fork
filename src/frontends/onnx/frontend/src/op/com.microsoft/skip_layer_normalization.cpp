// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "op/com.microsoft/skip_layer_normalization.hpp"

#include "openvino/frontend/exception.hpp"
#include "openvino/op/add.hpp"
#include "openvino/op/constant.hpp"
#include "openvino/op/multiply.hpp"
#include "openvino/op/mvn.hpp"

using namespace ov::op;

namespace ngraph {
namespace onnx_import {
namespace op {
namespace set_1 {
ov::OutputVector skip_layer_normalization(const Node& node) {
    auto nodes = node.get_ng_inputs();
    auto num_nodes = nodes.size();
    FRONT_END_GENERAL_CHECK(num_nodes >= 3 && num_nodes <= 5,
                            "SkipLayerNormalization takes 3, 4 or 5 inputs. Provided " + std::to_string(num_nodes));

    // input + skip
    std::shared_ptr<ov::Node> input = std::make_shared<v1::Add>(nodes[0], nodes[1]);
    // add bias if available
    if (num_nodes == 5) {
        input = std::make_shared<v1::Add>(input, nodes[4]);
    }
    float eps = node.get_attribute_value<float>("epsilon");
    // reduce over hidden_size
    int hidden_size_dim = 2;
    const auto reduction_axes = v0::Constant::create(ov::element::i32, Shape{1}, {hidden_size_dim});
    std::shared_ptr<ov::Node> result =
        std::make_shared<v6::MVN>(input, reduction_axes, true, eps, ov::op::MVNEpsMode::INSIDE_SQRT);
    // multiply by gamma
    result = std::make_shared<v1::Multiply>(result, nodes[2]);
    // add beta if available
    if (num_nodes > 3) {
        result = std::make_shared<v1::Add>(result, nodes[3]);
    }
    // spec mentions three outputs (output, mean, inv_std_var) while we support only first one, but:
    // - onnxruntime also doesn't support the last two
    // - we'd have to unroll MVN to have them
    return result->outputs();
}
}  // namespace set_1
}  // namespace op
}  // namespace onnx_import
}  // namespace ngraph
