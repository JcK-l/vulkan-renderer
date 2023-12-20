/// \file
/// \brief

//
// Created by Joshua Lowe on 12/10/2023.
// The license and distribution terms for this file may be found in the file LICENSE in this distribution
//

#include "Shader.h"
#include "../common/Log.h"
#include "Device.h"
#include <utility>

namespace vkf::core
{

Shader::Shader(const std::string &filePath)
{
    std::string shaderString = readFile(filePath);
    parseShader(shaderString);
}

std::vector<vk::PipelineShaderStageCreateInfo> Shader::createShaderStages(const Device &device)
{
    std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;

    for (const auto &entry : codes)
    {
        shaderModules.emplace_back(std::move(compileShader(device, entry.first)));

        vk::ShaderStageFlagBits stage;
        switch (entry.first)
        {
        case Type::Vertex:
            stage = vk::ShaderStageFlagBits::eVertex;
            break;
        case Type::Fragment:
            stage = vk::ShaderStageFlagBits::eFragment;
            break;
        // Add more cases as needed
        default:
            throw std::runtime_error("Invalid shader type");
        }

        shaderStages.push_back(
            vk::PipelineShaderStageCreateInfo{.stage = stage, .module = *shaderModules.back(), .pName = "main"});
    }

    return shaderStages;
}

const std::unordered_map<Shader::Type, std::string> &Shader::getCodes() const
{
    return codes;
}

vk::raii::ShaderModule Shader::compileShader(const Device &device, Type shaderType)
{
    // Check if the shader type exists in the codes map
    auto it = codes.find(shaderType);
    if (it == codes.end())
    {
        std::cerr << "Shader type not found in codes map";
        return vk::raii::ShaderModule(device.getHandle(), vk::ShaderModuleCreateInfo{});
    }

    // Get the shader source code from the codes map
    const std::string &shaderSource = it->second;
    shaderc_shader_kind shadercType = typeToShadercKind(shaderType);

    shaderc::Compiler compiler;
    shaderc::CompileOptions options;
    options.SetOptimizationLevel(shaderc_optimization_level_performance);

    shaderc::SpvCompilationResult shaderModule =
        compiler.CompileGlslToSpv(shaderSource, shadercType, "shader", options);
    if (shaderModule.GetCompilationStatus() != shaderc_compilation_status_success)
    {
        std::cerr << shaderModule.GetErrorMessage();
        return vk::raii::ShaderModule(device.getHandle(), vk::ShaderModuleCreateInfo{});
    }

    auto shaderCode = std::vector<uint32_t>{shaderModule.cbegin(), shaderModule.cend()};
    //    printTest(shaderCode);
    auto shaderSize = std::distance(shaderCode.begin(), shaderCode.end());
    auto shaderCreateInfo =
        vk::ShaderModuleCreateInfo{.codeSize = shaderSize * sizeof(uint32_t), .pCode = shaderCode.data()};

    return {device.getHandle(), shaderCreateInfo};
}

void Shader::printTest(std::vector<uint32_t> code)
{
    //    compiler = std::make_unique<spirv_cross::Compiler>(std::move(code));
    //    spirv_cross::ShaderResources resources = compiler->get_shader_resources();

    //    // Iterate over the push constant buffers
    //    for (const spirv_cross::Resource &resource : resources.push_constant_buffers)
    //    {
    //        // Get the range of the push constant buffer
    //        spirv_cross::SPIRType::BaseType baseType = reflection.get_type(resource.base_type_id).basetype;
    //        spirv_cross::SPIRType::BaseType type = reflection.get_type(resource.type_id).basetype;
    //
    //        // Print the push constant range
    //        std::cout << "Push constant range: " << baseType << ", " << type << std::endl;
    //    }
}

std::string Shader::readFile(const std::string &filePath) const
{
    std::ifstream shaderFile(filePath);
    if (!shaderFile)
    {
        throw std::runtime_error("Failed to open shader file: " + filePath);
    }

    std::stringstream shaderStream;
    shaderStream << shaderFile.rdbuf();
    return shaderStream.str();
}

void Shader::parseShader(std::string &shaderString)
{
    std::string delimiter = "// shader:";
    size_t pos = 0;
    size_t start = 0;
    std::string token;
    while ((pos = shaderString.find(delimiter, start)) != std::string::npos)
    {
        if (start != 0) // Ignore the first token
        {
            token = shaderString.substr(start, pos - start);
            size_t endOfLine = token.find("\n");
            std::string typeString = token.substr(0, endOfLine);
            std::string code = token.substr(endOfLine + 1);

            Type shaderType = stringToType(typeString);
            if (shaderType == Type::Unknown)
            {
                throw std::runtime_error("Invalid shader type: " + typeString);
            }
            codes[shaderType] = code;
        }
        start = pos + delimiter.length();
    }

    // Handle the last shader code
    if (start < shaderString.size())
    {
        token = shaderString.substr(start);
        size_t endOfLine = token.find("\n");
        std::string typeString = token.substr(0, endOfLine);
        std::string code = token.substr(endOfLine + 1);

        Type shaderType = stringToType(typeString);
        if (shaderType == Type::Unknown)
        {
            throw std::runtime_error("Invalid shader type: " + typeString);
        }
        codes[shaderType] = code;
    }
}

Shader::Type Shader::stringToType(const std::string &typeString) const
{
    std::string prefix = "// shader:";
    std::string typeWithoutPrefix;

    // Check if the typeString starts with the prefix
    if (typeString.substr(0, prefix.size()) == prefix)
    {
        // Remove the prefix
        typeWithoutPrefix = typeString.substr(prefix.size());
    }
    else
    {
        typeWithoutPrefix = typeString;
    }

    auto it = typeMap.find(typeWithoutPrefix);
    if (it != typeMap.end())
        return it->second;
    else
        return Type::Unknown;
}

shaderc_shader_kind Shader::typeToShadercKind(Type type) const
{
    switch (type)
    {
    case Type::Vertex:
        return shaderc_glsl_vertex_shader;
    case Type::Fragment:
        return shaderc_glsl_fragment_shader;
    case Type::Geometry:
        return shaderc_glsl_geometry_shader;
    default:
        return shaderc_glsl_infer_from_source;
    }
}

const std::unordered_map<std::string, Shader::Type> Shader::typeMap = {
    {"vertex", Type::Vertex}, {"fragment", Type::Fragment}, {"geometry", Type::Geometry}};
// Add more shader types as needed

} // namespace vkf::core