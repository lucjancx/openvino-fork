// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "shared_test_classes/single_layer/topk.hpp"

namespace LayerTestsDefinitions {
    std::string TopKLayerTest::getTestCaseName(const testing::TestParamInfo<TopKParams>& obj) {
    InferenceEngine::Precision netPrecision;
    InferenceEngine::Precision inPrc, outPrc;
    InferenceEngine::Layout inLayout;
    InferenceEngine::SizeVector inputShape;
    std::string targetDevice;
    int64_t keepK, axis;
    ov::op::v3::TopK::Mode mode;
    ov::op::v3::TopK::SortType sort;
    std::tie(keepK, axis, mode, sort, netPrecision, inPrc, outPrc, inLayout, inputShape, targetDevice) = obj.param;
    std::ostringstream result;
    result << "IS=" << ov::test::utils::vec2str(inputShape) << "_";
    result << "k=" << keepK << "_";
    result << "axis=" << axis << "_";
    result << "mode=" << mode << "_";
    result << "sort=" << sort << "_";
    result << "netPRC=" << netPrecision.name() << "_";
    result << "inPRC=" << inPrc.name() << "_";
    result << "outPRC=" << outPrc.name() << "_";
    result << "inL=" << inLayout << "_";
    result << "trgDev=" << targetDevice;
    return result.str();
}

void TopKLayerTest::SetUp() {
    InferenceEngine::SizeVector inputShape;
    InferenceEngine::Precision netPrecision;
    int64_t keepK, axis;
    ov::op::v3::TopK::Mode mode;
    ov::op::v3::TopK::SortType sort;
    std::tie(keepK, axis, mode, sort, netPrecision, inPrc, outPrc, inLayout, inputShape, targetDevice) = this->GetParam();

    auto ngPrc = FuncTestUtils::PrecisionUtils::convertIE2nGraphPrc(netPrecision);
    ov::ParameterVector params{std::make_shared<ov::op::v0::Parameter>(ngPrc, ov::Shape(inputShape))};

    auto k = std::make_shared<ov::op::v0::Constant>(ov::element::Type_t::i64, ov::Shape{}, &keepK);
    auto topk = std::dynamic_pointer_cast<ov::op::v3::TopK>(
            std::make_shared<ov::op::v3::TopK>(params[0], k, axis, mode, sort));

    ov::ResultVector results;
    for (size_t i = 0; i < topk->get_output_size(); i++) {
        results.push_back(std::make_shared<ov::op::v0::Result>(topk->output(i)));
    }
    function = std::make_shared<ov::Model>(results, params, "TopK");
}

InferenceEngine::Blob::Ptr TopKLayerTest::GenerateInput(const InferenceEngine::InputInfo &info) const {
    IE_ASSERT(InferenceEngine::Precision::FP32 == info.getTensorDesc().getPrecision()
           || InferenceEngine::Precision::BF16 == info.getTensorDesc().getPrecision()
           || InferenceEngine::Precision::FP16 == info.getTensorDesc().getPrecision());

    InferenceEngine::Blob::Ptr blob = make_blob_with_precision(info.getTensorDesc());
    blob->allocate();

    size_t size = blob->size();
    int start = - static_cast<int>(size / 2);
    std::vector<int> data(size);
    std::iota(data.begin(), data.end(), start);
    std::mt19937 gen(0);
    std::shuffle(data.begin(), data.end(), gen);

    float divisor = size / 10.0;
    if (InferenceEngine::Precision::FP32 == info.getTensorDesc().getPrecision()) {
        auto *rawBlobDataPtr = blob->buffer().as<float *>();
        for (size_t i = 0; i < size; i++) {
            rawBlobDataPtr[i] = static_cast<float>(data[i] / divisor);
        }
    } else if (InferenceEngine::Precision::BF16 == info.getTensorDesc().getPrecision()) {
        auto *rawBlobDataPtr = blob->buffer().as<ov::bfloat16 *>();
        for (size_t i = 0; i < size; i++) {
            rawBlobDataPtr[i] = static_cast<ov::bfloat16>(data[i] / divisor);
        }
    } else if (InferenceEngine::Precision::FP16 == info.getTensorDesc().getPrecision()) {
        auto* rawBlobDataPtr = blob->buffer().as<ov::float16*>();
        for (size_t i = 0; i < size; i++) {
            rawBlobDataPtr[i] = static_cast<ov::float16>(data[i] / divisor);
        }
    }

    return blob;
}
}  // namespace LayerTestsDefinitions
