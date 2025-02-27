// Copyright (C) 2019-2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "shared_test_classes/single_layer/low_precision.hpp"
#include "ov_models/builders.hpp"
#include "common_test_utils/node_builders/constant.hpp"

namespace LowPrecisionTestDefinitions {

std::string LowPrecisionTest::getTestCaseName(const testing::TestParamInfo<lowPrecisionTestParamsSet>& obj) {
    InferenceEngine::Precision netPrecision;
    std::string targetDevice;
    std::pair<std::string, std::map<std::string, std::string>> config;
    std::tie(netPrecision, targetDevice, config) = obj.param;

    std::ostringstream result;
    result << "netPRC=" << netPrecision.name() << "_";
    result << "trgDev=" << targetDevice;
    if (!config.first.empty()) {
        result << "_targetConfig=" << config.first;
    }
    return result.str();
}

void LowPrecisionTest::SetUp() {
    InferenceEngine::Precision netPrecision;
    std::pair<std::string, std::map<std::string, std::string>> config;
    std::tie(netPrecision, targetDevice, config) = this->GetParam();
    auto ngPrc = FuncTestUtils::PrecisionUtils::convertIE2nGraphPrc(netPrecision);
    auto inputShape = ov::Shape{ 1, 16 };
    auto weights1Shape = ov::Shape{ 16, 16 };
    auto weights2Shape = ov::Shape{ 128, 32 };

    // fully connected 1
    auto input = std::make_shared<ov::op::v0::Parameter>(ngPrc, inputShape);
    std::vector<float> weights1Data(ov::shape_size(weights1Shape), 0.0f);

    for (size_t i = 0; i < 16; i++) {
        weights1Data[i * 17] = 10.0f + i;
    }

    auto weights1 = ov::test::utils::deprecated::make_constant<float>(ngPrc, weights1Shape, weights1Data);
    auto fc1 = std::make_shared<ov::op::v0::MatMul>(input, weights1);
    fc1->set_friendly_name("FullyConnected_1");

    // bias 1
    std::vector<float> bias1Data(ov::shape_size(inputShape), 0.0f);
    auto bias1 = ov::test::utils::deprecated::make_constant<float>(ngPrc, inputShape, bias1Data);
    auto add1 = std::make_shared<ov::op::v1::Add>(fc1, bias1);
    add1->set_friendly_name("Add_1");
#if 0
    // ReLU 1
    auto relu1 = std::make_shared<ov::op::v0::Relu>(add1);
    relu1->set_friendly_name("Relu_1");

    //// fully connected 2
    std::vector<float> weights2Data(ov::shape_size(weights2Shape), 0.0f);
    std::fill(weights2Data.begin(), weights2Data.end(), 0.0001f);
    auto weights2 = ov::test::utils::deprecated::make_constant<float>(ngPrc, weights2Shape, weights2Data);
    auto fc2 = std::make_shared<ov::op::v0::MatMul>(relu1, weights2);
    fc2->set_friendly_name("FullyConnected_2");

    //// bias 2
    std::vector<float> bias2Data(ov::shape_size(weights2Shape), 0.0f);
    auto bias2 = ov::test::utils::deprecated::make_constant<float>(ngPrc, weights2Shape, bias2Data);
    auto add2 = std::make_shared<ov::op::v1::Add>(fc2, bias2);
    add2->set_friendly_name("Add_2");

    //// ReLU 2
    auto relu2 = std::make_shared<ov::op::v0::Relu>(add2);
    relu2->set_friendly_name("Relu_2");
#endif
    configuration = config.second;
    function = std::make_shared<ov::Model>(ov::ResultVector{std::make_shared<ov::op::v0::Result>(add1)},
                                                  ov::ParameterVector{input},
                                                  "LowPrecisionTest");
}

}  // namespace LowPrecisionTestDefinitions
