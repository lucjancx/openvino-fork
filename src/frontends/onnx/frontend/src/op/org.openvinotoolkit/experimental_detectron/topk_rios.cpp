// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "op/org.openvinotoolkit/experimental_detectron/topk_rios.hpp"

#include "onnx_import/core/node.hpp"
#include "openvino/op/experimental_detectron_topkrois.hpp"

using namespace ov::op;

namespace ngraph {
namespace onnx_import {
namespace op {
namespace set_1 {
ov::OutputVector experimental_detectron_topk_rois(const Node& node) {
    using TopKROIs = v6::ExperimentalDetectronTopKROIs;

    auto inputs = node.get_ng_inputs();
    auto input_rois = inputs[0];
    auto rois_probs = inputs[1];
    auto max_rois = static_cast<std::size_t>(node.get_attribute_value<std::int64_t>("max_rois", 1000));

    return {std::make_shared<TopKROIs>(input_rois, rois_probs, max_rois)};
}

}  // namespace set_1

}  // namespace op

}  // namespace onnx_import

}  // namespace ngraph
