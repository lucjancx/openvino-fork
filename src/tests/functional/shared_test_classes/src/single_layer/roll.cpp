// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "shared_test_classes/single_layer/roll.hpp"

namespace LayerTestsDefinitions {

std::string RollLayerTest::getTestCaseName(const testing::TestParamInfo<rollParams>& obj) {
    InferenceEngine::SizeVector inputShapes;
    InferenceEngine::Precision inputPrecision;
    std::vector<int64_t> shift;
    std::vector<int64_t> axes;
    std::string targetDevice;
    std::tie(inputShapes, inputPrecision, shift, axes, targetDevice) = obj.param;

    std::ostringstream result;
    result << "IS=" << ov::test::utils::vec2str(inputShapes) << "_";
    result << "Precision=" << inputPrecision.name() << "_";
    result << "Shift=" << ov::test::utils::vec2str(shift) << "_";
    result << "Axes=" << ov::test::utils::vec2str(axes) << "_";
    result << "TargetDevice=" << targetDevice;
    return result.str();
}

void RollLayerTest::SetUp() {
    InferenceEngine::SizeVector inputShapes;
    InferenceEngine::Precision inputPrecision;
    std::vector<int64_t> shift;
    std::vector<int64_t> axes;
    std::tie(inputShapes, inputPrecision, shift, axes, targetDevice) = this->GetParam();
    auto inType = FuncTestUtils::PrecisionUtils::convertIE2nGraphPrc(inputPrecision);
    ov::ParameterVector paramVector;
    auto paramData = std::make_shared<ov::op::v0::Parameter>(inType, ov::Shape(inputShapes));
    paramVector.push_back(paramData);

    auto shiftNode = std::make_shared<ov::op::v0::Constant>(ov::element::Type_t::i64, ov::Shape{shift.size()}, shift)->output(0);
    auto axesNode = std::make_shared<ov::op::v0::Constant>(ov::element::Type_t::i64, ov::Shape{axes.size()}, axes)->output(0);

    auto roll = std::make_shared<ov::op::v7::Roll>(paramVector[0], shiftNode, axesNode);

    ov::ResultVector results{std::make_shared<ov::op::v0::Result>(roll)};
    function = std::make_shared<ov::Model>(results, paramVector, "roll");
}
}  // namespace LayerTestsDefinitions
