#include "depthai_ros_driver/pipeline/base_types.hpp"

#include "depthai/device/Device.hpp"
#include "depthai/pipeline/Pipeline.hpp"
#include "depthai_ros_driver/dai_nodes/base_node.hpp"
#include "depthai_ros_driver/dai_nodes/nn/nn_helpers.hpp"
#include "depthai_ros_driver/dai_nodes/nn/nn_wrapper.hpp"
#include "depthai_ros_driver/dai_nodes/nn/spatial_nn_wrapper.hpp"
#include "depthai_ros_driver/dai_nodes/sensors/imu.hpp"
#include "depthai_ros_driver/dai_nodes/sensors/sensor_helpers.hpp"
#include "depthai_ros_driver/dai_nodes/sensors/sensor_wrapper.hpp"
#include "depthai_ros_driver/dai_nodes/stereo.hpp"
#include "depthai_ros_driver/pipeline/base_pipeline.hpp"
#include "depthai_ros_driver/utils.hpp"
#include "rclcpp/node.hpp"

namespace depthai_ros_driver {
namespace pipeline_gen {

std::vector<std::unique_ptr<dai_nodes::BaseNode>> RGB::createPipeline(rclcpp::Node* node,
                                                                      std::shared_ptr<dai::Device> device,
                                                                      std::shared_ptr<dai::Pipeline> pipeline,
                                                                      const std::string& nnType) {
    std::string nTypeUpCase = utils::getUpperCaseStr(nnType);
    auto nType = utils::getValFromMap(nTypeUpCase, nnTypeMap);

    std::vector<std::unique_ptr<dai_nodes::BaseNode>> daiNodes;
    auto rgb = std::make_unique<dai_nodes::SensorWrapper>("rgb", node, pipeline, device, dai::CameraBoardSocket::CAM_A);
    switch(nType) {
        case NNType::None:
            break;
        case NNType::RGB: {
            auto nn = createNN(node, pipeline, *rgb);
            daiNodes.push_back(std::move(nn));
            break;
        }
        case NNType::Spatial: {
            RCLCPP_WARN(node->get_logger(), "Spatial NN selected, but configuration is RGB. Please change camera.i_nn_type parameter to RGB.");
        }
        default:
            break;
    }
    daiNodes.push_back(std::move(rgb));
    return daiNodes;
}
std::vector<std::unique_ptr<dai_nodes::BaseNode>> RGBD::createPipeline(rclcpp::Node* node,
                                                                       std::shared_ptr<dai::Device> device,
                                                                       std::shared_ptr<dai::Pipeline> pipeline,
                                                                       const std::string& nnType) {
    std::string nTypeUpCase = utils::getUpperCaseStr(nnType);
    auto nType = utils::getValFromMap(nTypeUpCase, nnTypeMap);

    std::vector<std::unique_ptr<dai_nodes::BaseNode>> daiNodes;
    auto rgb = std::make_unique<dai_nodes::SensorWrapper>("rgb", node, pipeline, device, dai::CameraBoardSocket::CAM_A);
    auto stereo = std::make_unique<dai_nodes::Stereo>("stereo", node, pipeline, device);
    switch(nType) {
        case NNType::None:
            break;
        case NNType::RGB: {
            auto nn = createNN(node, pipeline, *rgb);
            daiNodes.push_back(std::move(nn));
            break;
        }
        case NNType::Spatial: {
            auto nn = createSpatialNN(node, pipeline, *rgb, *stereo);
            daiNodes.push_back(std::move(nn));
            break;
        }
        default:
            break;
    }
    daiNodes.push_back(std::move(rgb));
    daiNodes.push_back(std::move(stereo));
    return daiNodes;
}
std::vector<std::unique_ptr<dai_nodes::BaseNode>> RGBStereo::createPipeline(rclcpp::Node* node,
                                                                            std::shared_ptr<dai::Device> device,
                                                                            std::shared_ptr<dai::Pipeline> pipeline,
                                                                            const std::string& nnType) {
    std::string nTypeUpCase = utils::getUpperCaseStr(nnType);
    auto nType = utils::getValFromMap(nTypeUpCase, nnTypeMap);

    std::vector<std::unique_ptr<dai_nodes::BaseNode>> daiNodes;
    auto rgb = std::make_unique<dai_nodes::SensorWrapper>("rgb", node, pipeline, device, dai::CameraBoardSocket::CAM_A);
    auto left = std::make_unique<dai_nodes::SensorWrapper>("left", node, pipeline, device, dai::CameraBoardSocket::CAM_B);
    auto right = std::make_unique<dai_nodes::SensorWrapper>("right", node, pipeline, device, dai::CameraBoardSocket::CAM_C);
    switch(nType) {
        case NNType::None:
            break;
        case NNType::RGB: {
            auto nn = createNN(node, pipeline, *rgb);
            daiNodes.push_back(std::move(nn));
            break;
        }
        case NNType::Spatial: {
            RCLCPP_WARN(node->get_logger(), "Spatial NN selected, but configuration is RGBStereo. Please change camera.i_nn_type parameter to RGB.");
        }
        default:
            break;
    }
    daiNodes.push_back(std::move(rgb));
    daiNodes.push_back(std::move(left));
    daiNodes.push_back(std::move(right));
    return daiNodes;
}
std::vector<std::unique_ptr<dai_nodes::BaseNode>> Stereo::createPipeline(rclcpp::Node* node,
                                                                         std::shared_ptr<dai::Device> device,
                                                                         std::shared_ptr<dai::Pipeline> pipeline,
                                                                         const std::string& /*nnType*/) {
    std::vector<std::unique_ptr<dai_nodes::BaseNode>> daiNodes;
    auto left = std::make_unique<dai_nodes::SensorWrapper>("left", node, pipeline, device, dai::CameraBoardSocket::CAM_B);
    auto right = std::make_unique<dai_nodes::SensorWrapper>("right", node, pipeline, device, dai::CameraBoardSocket::CAM_C);
    daiNodes.push_back(std::move(left));
    daiNodes.push_back(std::move(right));
    return daiNodes;
}
std::vector<std::unique_ptr<dai_nodes::BaseNode>> Depth::createPipeline(rclcpp::Node* node,
                                                                        std::shared_ptr<dai::Device> device,
                                                                        std::shared_ptr<dai::Pipeline> pipeline,
                                                                        const std::string& /*nnType*/) {
    std::vector<std::unique_ptr<dai_nodes::BaseNode>> daiNodes;
    auto stereo = std::make_unique<dai_nodes::Stereo>("stereo", node, pipeline, device);
    daiNodes.push_back(std::move(stereo));
    return daiNodes;
}
std::vector<std::unique_ptr<dai_nodes::BaseNode>> CamArray::createPipeline(rclcpp::Node* node,
                                                                           std::shared_ptr<dai::Device> device,
                                                                           std::shared_ptr<dai::Pipeline> pipeline,
                                                                           const std::string& /*nnType*/) {
    std::vector<std::unique_ptr<dai_nodes::BaseNode>> daiNodes;

    for(auto& feature : device->getConnectedCameraFeatures()) {
        auto name = utils::getSocketName(feature.socket);
        auto daiNode = std::make_unique<dai_nodes::SensorWrapper>(name, node, pipeline, device, feature.socket);
        daiNodes.push_back(std::move(daiNode));
    };
    return daiNodes;
}
std::vector<std::unique_ptr<dai_nodes::BaseNode>> Rae::createPipeline(rclcpp::Node* node,
                                                                      std::shared_ptr<dai::Device> device,
                                                                      std::shared_ptr<dai::Pipeline> pipeline,
                                                                      const std::string& nnType) {
    std::vector<std::unique_ptr<dai_nodes::BaseNode>> daiNodes;
    auto stereo_front =
        std::make_unique<dai_nodes::Stereo>("stereo_front", node, pipeline, device, dai::CameraBoardSocket::CAM_B, dai::CameraBoardSocket::CAM_C);
    auto stereo_back =
        std::make_unique<dai_nodes::Stereo>("stereo_back", node, pipeline, device, dai::CameraBoardSocket::CAM_D, dai::CameraBoardSocket::CAM_E);

    std::string nTypeUpCase = utils::getUpperCaseStr(nnType);
    auto nType = utils::getValFromMap(nTypeUpCase, nnTypeMap);
    switch(nType) {
        case NNType::None:
            break;
        case NNType::RGB: {
            auto nn = std::make_unique<dai_nodes::SpatialNNWrapper>("nn", node, pipeline);
            stereo_front->link(nn->getInput(static_cast<int>(dai_nodes::nn_helpers::link_types::SpatialNNLinkType::input)),
                               static_cast<int>(dai_nodes::link_types::StereoLinkType::left_preview));
            daiNodes.push_back(std::move(nn));
            break;
        }
        case NNType::Spatial: {
            auto nn = std::make_unique<dai_nodes::SpatialNNWrapper>("nn", node, pipeline);
            stereo_front->link(nn->getInput(static_cast<int>(dai_nodes::nn_helpers::link_types::SpatialNNLinkType::input)),
                               static_cast<int>(dai_nodes::link_types::StereoLinkType::left_preview));
            stereo_front->link(nn->getInput(static_cast<int>(dai_nodes::nn_helpers::link_types::SpatialNNLinkType::inputDepth)), 0);
            daiNodes.push_back(std::move(nn));
            break;
        }
        default:
            break;
    }
    daiNodes.push_back(std::move(stereo_front));
    daiNodes.push_back(std::move(stereo_back));
    return daiNodes;
}
}  // namespace pipeline_gen
}  // namespace depthai_ros_driver

#include <pluginlib/class_list_macros.hpp>

PLUGINLIB_EXPORT_CLASS(depthai_ros_driver::pipeline_gen::RGB, depthai_ros_driver::pipeline_gen::BasePipeline)
PLUGINLIB_EXPORT_CLASS(depthai_ros_driver::pipeline_gen::RGBD, depthai_ros_driver::pipeline_gen::BasePipeline)
PLUGINLIB_EXPORT_CLASS(depthai_ros_driver::pipeline_gen::RGBStereo, depthai_ros_driver::pipeline_gen::BasePipeline)
PLUGINLIB_EXPORT_CLASS(depthai_ros_driver::pipeline_gen::Stereo, depthai_ros_driver::pipeline_gen::BasePipeline)
PLUGINLIB_EXPORT_CLASS(depthai_ros_driver::pipeline_gen::Depth, depthai_ros_driver::pipeline_gen::BasePipeline)
PLUGINLIB_EXPORT_CLASS(depthai_ros_driver::pipeline_gen::CamArray, depthai_ros_driver::pipeline_gen::BasePipeline)
PLUGINLIB_EXPORT_CLASS(depthai_ros_driver::pipeline_gen::Rae, depthai_ros_driver::pipeline_gen::BasePipeline)