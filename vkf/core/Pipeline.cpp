////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file Pipeline.cpp
/// \brief This file implements the Pipeline class which is used for managing Vulkan pipelines.
///
/// The Pipeline class is part of the vkf::core namespace. It provides functionality to get the handle to the pipeline.
/// Currently, this class only creates a trivial pipeline, but it is planned to be expanded in the future.
///
/// \author Joshua Lowe
/// \date 12/3/2023
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Pipeline.h"
#include "Device.h"
#include "RenderPass.h"
#include <shaderc/shaderc.hpp>

namespace vkf::core
{

Pipeline::Pipeline(const Device &device, const RenderPass &renderPass)
{
    // const char kShaderSource[]
    std::string kShaderSource = R"vertexshader(
            #version 450
            #extension GL_ARB_separate_shader_objects : enable

            out gl_PerVertex {
                vec4 gl_Position;
            };

            layout(location = 0) out vec3 fragColor;

            vec2 positions[3] = vec2[](
                vec2(0.0, -0.5),
                vec2(0.5, 0.5),
                vec2(-0.5, 0.5)
            );

            vec3 colors[3] = vec3[](
                vec3(1.0, 1.0, 1.0),
                vec3(1.0, 1.0, 1.0),
                vec3(1.0, 1.0, 1.0)
            );

            void main() {
                gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
                fragColor = colors[gl_VertexIndex];
            }
            )vertexshader";

    std::string fragmentShader = R"fragmentShader(
            #version 450
            #extension GL_ARB_separate_shader_objects : enable

            layout(location = 0) in vec3 fragColor;

            layout(location = 0) out vec4 outColor;

            void main() {
                outColor = vec4(fragColor, 1.0);
            }
            )fragmentShader";

    shaderc::Compiler compiler;
    shaderc::CompileOptions options;
    options.SetOptimizationLevel(shaderc_optimization_level_performance);

    shaderc::SpvCompilationResult vertShaderModule =
        compiler.CompileGlslToSpv(kShaderSource, shaderc_glsl_vertex_shader, "vertex shader", options);
    if (vertShaderModule.GetCompilationStatus() != shaderc_compilation_status_success)
    {
        std::cerr << vertShaderModule.GetErrorMessage();
    }
    auto vertShaderCode = std::vector<uint32_t>{vertShaderModule.cbegin(), vertShaderModule.cend()};
    auto vertSize = std::distance(vertShaderCode.begin(), vertShaderCode.end());
    auto vertShaderCreateInfo =
        vk::ShaderModuleCreateInfo{.codeSize = vertSize * sizeof(uint32_t), .pCode = vertShaderCode.data()};

    shaderc::SpvCompilationResult fragShaderModule =
        compiler.CompileGlslToSpv(fragmentShader, shaderc_glsl_fragment_shader, "fragment shader", options);
    if (fragShaderModule.GetCompilationStatus() != shaderc_compilation_status_success)
    {
        std::cerr << fragShaderModule.GetErrorMessage();
    }
    auto fragShaderCode = std::vector<uint32_t>{fragShaderModule.cbegin(), fragShaderModule.cend()};
    auto fragSize = std::distance(fragShaderCode.begin(), fragShaderCode.end());
    auto fragShaderCreateInfo =
        vk::ShaderModuleCreateInfo{.codeSize = fragSize * sizeof(uint32_t), .pCode = fragShaderCode.data()};

    auto vertexShaderModule = vk::raii::ShaderModule{device.getHandle(), vertShaderCreateInfo};
    auto fragmentShaderModule = vk::raii::ShaderModule{device.getHandle(), fragShaderCreateInfo};

    auto vertShaderStageInfo = vk::PipelineShaderStageCreateInfo{
        .stage = vk::ShaderStageFlagBits::eVertex, .module = *vertexShaderModule, .pName = "main"};

    auto fragShaderStageInfo = vk::PipelineShaderStageCreateInfo{
        .stage = vk::ShaderStageFlagBits::eFragment, .module = *fragmentShaderModule, .pName = "main"};

    auto pipelineShaderStages =
        std::vector<vk::PipelineShaderStageCreateInfo>{vertShaderStageInfo, fragShaderStageInfo};

    auto vertexInputCreateInfo = vk::PipelineVertexInputStateCreateInfo{};
    auto inputAssemblyCreateInfo =
        vk::PipelineInputAssemblyStateCreateInfo{.topology = vk::PrimitiveTopology::eTriangleList};

    std::vector<vk::DynamicState> dynamicStates = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};

    vk::PipelineDynamicStateCreateInfo dynamicStateCreateInfo{
        .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()), .pDynamicStates = dynamicStates.data()};

    vk::PipelineViewportStateCreateInfo pipelineViewportStateCreateInfo{
        .viewportCount = 1, .pViewports = nullptr, .scissorCount = 1, .pScissors = nullptr};

    auto rasterizerCreateInfo = vk::PipelineRasterizationStateCreateInfo{
        .polygonMode = vk::PolygonMode::eFill, .frontFace = vk::FrontFace::eCounterClockwise, .lineWidth = 1.0f};

    auto multisamplingCreateInfo =
        vk::PipelineMultisampleStateCreateInfo{.rasterizationSamples = vk::SampleCountFlagBits::e1};

    auto colorBlendAttachment = vk::PipelineColorBlendAttachmentState{
        .colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                          vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA};

    auto colorBlendingCreateInfo =
        vk::PipelineColorBlendStateCreateInfo{.attachmentCount = 1, .pAttachments = &colorBlendAttachment};

    pipelineLayout = vk::raii::PipelineLayout{device.getHandle(), vk::PipelineLayoutCreateInfo{}};

    auto pipelineCreateInfo = vk::GraphicsPipelineCreateInfo{.stageCount = 2,
                                                             .pStages = pipelineShaderStages.data(),
                                                             .pVertexInputState = &vertexInputCreateInfo,
                                                             .pInputAssemblyState = &inputAssemblyCreateInfo,
                                                             .pViewportState = &pipelineViewportStateCreateInfo,
                                                             .pRasterizationState = &rasterizerCreateInfo,
                                                             .pMultisampleState = &multisamplingCreateInfo,
                                                             .pColorBlendState = &colorBlendingCreateInfo,
                                                             .pDynamicState = &dynamicStateCreateInfo,
                                                             .layout = *pipelineLayout,
                                                             .renderPass = *renderPass.getHandle()};

    handle = vk::raii::Pipeline{device.getHandle(), nullptr, pipelineCreateInfo};
}

const vk::raii::Pipeline &Pipeline::getHandle() const
{
    return handle;
}

} // namespace vkf::core