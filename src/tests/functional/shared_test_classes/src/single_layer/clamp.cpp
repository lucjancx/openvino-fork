// Copyright (C) 2018-2023 Intel Corporation
//
// SPDX-License-Identifier: Apache-2.0
//

#include "shared_test_classes/single_layer/clamp.hpp"

namespace LayerTestsDefinitions {

std::string ClampLayerTest::getTestCaseName(const testing::TestParamInfo<clampParamsTuple>& obj) {
    InferenceEngine::SizeVector inShape;
    std::pair<float, float> interval;
    InferenceEngine::Precision netPrc;
    std::string targetDevice;

    std::tie(inShape, interval, netPrc, targetDevice) = obj.param;

    std::ostringstream result;
    result << "inShape=" << ov::test::utils::vec2str(inShape) << "_";
    result << "min=" << interval.first << "_";
    result << "max=" << interval.second << "_";
    result << "netPrc=" << netPrc.name() << "_";
    result << "trgDev=" << targetDevice;
    return result.str();
}

void ClampLayerTest::SetUp() {
    InferenceEngine::SizeVector inShape;
    std::pair<float, float> interval;
    InferenceEngine::Precision netPrc;

    std::tie(inShape, interval, netPrc, targetDevice) = this->GetParam();

    auto ngNetPrc = FuncTestUtils::PrecisionUtils::convertIE2nGraphPrc(netPrc);
    auto input = std::make_shared<ov::op::v0::Parameter>(ngNetPrc, ov::Shape(inShape));
    auto clamp = std::make_shared<ov::op::v0::Clamp>(input, interval.first, interval.second);
    function = std::make_shared<ov::Model>(std::make_shared<ov::op::v0::Result>(clamp), ov::ParameterVector{input});
}

} // namespace LayerTestsDefinitions
