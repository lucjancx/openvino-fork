// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "shared_test_classes/single_layer/nonzero.hpp"

namespace LayerTestsDefinitions {

std::string NonZeroLayerTest::getTestCaseName(const testing::TestParamInfo<NonZeroLayerTestParamsSet>& obj) {
    std::vector<size_t> inputShape;
    InferenceEngine::Precision inputPrecision;
    std::string targetDevice;
    ConfigMap additionalConfig;
    std::tie(inputShape, inputPrecision, targetDevice, additionalConfig) = obj.param;

    std::ostringstream result;
    result << "IS=" << ov::test::utils::vec2str(inputShape) << "_";
    result << "inPRC=" << inputPrecision.name() << "_";
    result << "targetDevice=" << targetDevice;
    return result.str();
}

void NonZeroLayerTest::SetUp() {
    auto inputShape     = std::vector<std::size_t>{};
    auto inputPrecision = InferenceEngine::Precision::UNSPECIFIED;
    ConfigMap additionalConfig;
    std::tie(inputShape, inputPrecision, targetDevice, additionalConfig) = GetParam();

    configuration.insert(additionalConfig.cbegin(), additionalConfig.cend());

    const auto& precision = FuncTestUtils::PrecisionUtils::convertIE2nGraphPrc(inputPrecision);
    const auto& paramNode = std::make_shared<ov::op::v0::Parameter>(precision, ov::Shape(inputShape));

    auto nonZeroOp = std::make_shared<ov::op::v3::NonZero>(paramNode->output(0));

    ov::ResultVector results{std::make_shared<ov::op::v0::Result>(nonZeroOp)};
    function = std::make_shared<ov::Model>(results, ov::ParameterVector{paramNode}, "non_zero");
}
}  // namespace LayerTestsDefinitions
