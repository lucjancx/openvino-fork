// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <gtest/gtest.h>

#include <functional>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <tuple>
#include <vector>

#include "common_test_utils/common_utils.hpp"
#include "functional_test_utils/blob_utils.hpp"
#include "ie_core.hpp"
#include "ie_precision.hpp"

#include "functional_test_utils/blob_utils.hpp"
#include "shared_test_classes/base/layer_test_utils.hpp"
#include "common_test_utils/common_utils.hpp"

#include "ov_models/utils/ov_helpers.hpp"
#include "ov_models/builders.hpp"
#include "ov_models/utils/ov_helpers.hpp"
#include "shared_test_classes/base/layer_test_utils.hpp"

namespace LayerTestsDefinitions {

typedef std::tuple<
        std::vector<float>,  // widths
        std::vector<float>,  // heights
        bool,                // clip
        float,               // step_width
        float,               // step_height
        float,               // step
        float,               // offset
        std::vector<float>> priorBoxClusteredSpecificParams;

typedef std::tuple<
        priorBoxClusteredSpecificParams,
        InferenceEngine::Precision,   // net precision
        InferenceEngine::Precision,   // Input precision
        InferenceEngine::Precision,   // Output precision
        InferenceEngine::Layout,      // Input layout
        InferenceEngine::Layout,      // Output layout
        InferenceEngine::SizeVector,  // input shape
        InferenceEngine::SizeVector,  // image shape
        std::string> priorBoxClusteredLayerParams;

class PriorBoxClusteredLayerTest
    : public testing::WithParamInterface<priorBoxClusteredLayerParams>,
      virtual public LayerTestsUtils::LayerTestsCommon {
public:
    static std::string getTestCaseName(const testing::TestParamInfo<priorBoxClusteredLayerParams>& obj);

protected:
    InferenceEngine::SizeVector inputShapes;
    InferenceEngine::SizeVector imageShapes;
    InferenceEngine::Precision netPrecision;
    std::vector<float> widths;
    std::vector<float> heights;
    std::vector<float> variances;
    float step_width;
    float step_height;
    float step;
    float offset;
    bool clip;

    void SetUp() override;
};

}  // namespace LayerTestsDefinitions
