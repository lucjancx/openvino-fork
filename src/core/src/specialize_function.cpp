// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "ngraph/specialize_function.hpp"

#include "itt.hpp"
#include "ngraph/op/util/op_types.hpp"
#include "openvino/op/constant.hpp"
#include "openvino/op/parameter.hpp"

using namespace ngraph;
NGRAPH_SUPPRESS_DEPRECATED_START;

using ov::op::v0::Constant;

std::shared_ptr<ov::Model> ngraph::specialize_function(std::shared_ptr<ov::Model> f,
                                                       const std::vector<ov::element::Type>& parameter_element_types,
                                                       const std::vector<ov::PartialShape>& parameter_shapes,
                                                       const std::vector<void*>& parameter_values)

{
    OV_ITT_SCOPED_TASK(ov::itt::domains::core, "specialize_function");

    OPENVINO_ASSERT(f->get_parameters().size() == parameter_shapes.size());
    OPENVINO_ASSERT(f->get_parameters().size() == parameter_element_types.size());
    OPENVINO_ASSERT(f->get_parameters().size() == parameter_values.size());

    NodeMap m;

    for (size_t i = 0; i < parameter_shapes.size(); i++) {
        OPENVINO_ASSERT(f->get_parameters()[i]->get_element_type().is_dynamic() ||
                        parameter_element_types[i] == f->get_parameters()[i]->get_element_type());

        if (parameter_values[i] != nullptr && parameter_shapes[i].is_static() &&
            parameter_element_types[i].is_static()) {
            m[f->get_parameters()[i].get()] = std::make_shared<Constant>(parameter_element_types[i],
                                                                         parameter_shapes[i].to_shape(),
                                                                         parameter_values[i]);
        } else {
            m[f->get_parameters()[i].get()] =
                std::make_shared<ov::op::v0::Parameter>(parameter_element_types[i], parameter_shapes[i]);
        }
        auto rt_info = f->get_parameters()[i]->get_rt_info();
        m[f->get_parameters()[i].get()]->get_rt_info() = rt_info;
    }

    for (auto old_node : f->get_ordered_ops()) {
        if (op::is_parameter(old_node)) {
            continue;
        }

        ov::OutputVector new_args;
        for (auto input : old_node->inputs()) {
            auto output = input.get_source_output();
            new_args.push_back(output.for_node(m[output.get_node()]));
        }

        ov::NodeVector cloned_dependencies;
        for (auto& dependency : old_node->get_control_dependencies()) {
            std::shared_ptr<Node> dependent = m.at(dependency.get());
            if (find(cloned_dependencies.begin(), cloned_dependencies.end(), dependent) == cloned_dependencies.end()) {
                cloned_dependencies.push_back(dependent);
            }
        }
        m[old_node.get()] = old_node->copy_with_new_inputs(new_args, cloned_dependencies);

        auto rt_info = old_node->get_rt_info();
        m[old_node.get()]->get_rt_info() = rt_info;

        m[old_node.get()]->set_friendly_name(old_node->get_friendly_name());
    }

    ov::ParameterVector new_parameters = f->get_parameters();
    for (size_t i = 0; i < new_parameters.size(); i++) {
        auto name = new_parameters[i]->get_friendly_name();
        new_parameters[i] = ov::as_type_ptr<ov::op::v0::Parameter>(m[new_parameters[i].get()]);

        // If the replacement for a Parameter is not itself a Parameter, we must have replaced it
        // with a constant. We will insert a dead Parameter into the clone's parameters, in order
        // to maintain the arity of the original function.
        if (new_parameters[i] == nullptr) {
            new_parameters[i] =
                std::make_shared<ov::op::v0::Parameter>(parameter_element_types[i], parameter_shapes[i]);
        }
        new_parameters[i]->set_friendly_name(name);
    }

    ov::ResultVector new_results = f->get_results();
    for (size_t i = 0; i < new_results.size(); i++) {
        auto name = new_results[i]->get_friendly_name();
        new_results[i] = std::static_pointer_cast<ov::op::v0::Result>(m[new_results[i].get()]);
        new_results[i]->set_friendly_name(name);
    }
    auto new_sinks = f->get_sinks();
    for (size_t i = 0; i < new_sinks.size(); i++) {
        new_sinks[i] = std::static_pointer_cast<ov::op::Sink>(m[new_sinks[i].get()]);
    }

    return std::make_shared<ov::Model>(new_results, new_sinks, new_parameters);
}
