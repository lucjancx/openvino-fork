// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

// Disabled in CMakeList
// Update to higher opset required

#pragma once

#include "openvino/core/deprecated.hpp"
OPENVINO_SUPPRESS_DEPRECATED_START

#include "onnx_import/core/node.hpp"

namespace ngraph {
namespace onnx_import {
namespace op {
namespace set_1 {
/// \brief Performs ONNX QLinearConv operation.
///
/// \param node   The ONNX node object representing this operation.
///
/// \return The vector containing OV nodes producing output of ONNX quantizied
///         convolution operation.
ov::OutputVector qlinear_conv(const Node& node);

}  // namespace set_1
}  // namespace op
}  // namespace onnx_import

}  // namespace ngraph
OPENVINO_SUPPRESS_DEPRECATED_END
