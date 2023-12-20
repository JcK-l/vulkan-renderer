/// \file
/// \brief

//
// Created by Joshua Lowe on 12/10/2023.
// The license and distribution terms for this file may be found in the file LICENSE in this distribution
//

#ifndef VULKANRENDERER_SHADER_H
#define VULKANRENDERER_SHADER_H

#include <shaderc/shaderc.hpp>
// #include <spirv_cross/spirv_cross.hpp>

namespace vkf::core
{

// Forward declarations
class Device;

class Shader
{
  public:
    enum class Type
    {
        Vertex,
        Fragment,
        Geometry,
        Unknown
        // Add more shader types as needed
    };

    explicit Shader(const std::string &filePath);
    Shader(const Shader &) = delete;            // Deleted copy constructor
    Shader(Shader &&) noexcept = default;       // Default move constructor
    Shader &operator=(const Shader &) = delete; // Deleted copy assignment operator
    Shader &operator=(Shader &&) = delete;      // Deleted move assignment operator
    ~Shader() = default;                        // Default destructor

    [[nodiscard]] const std::unordered_map<Type, std::string> &getCodes() const;

    std::vector<vk::PipelineShaderStageCreateInfo> createShaderStages(const Device &device);

    void printTest(std::vector<uint32_t> code);

  private:
    vk::raii::ShaderModule compileShader(const Device &device, Type type);

    Type stringToType(const std::string &typeString) const;
    shaderc_shader_kind typeToShadercKind(Type type) const;
    std::string readFile(const std::string &filePath) const;
    void parseShader(std::string &shaderString);

    std::unordered_map<Type, std::string> codes;
    std::vector<vk::raii::ShaderModule> shaderModules;

    static const std::unordered_map<std::string, Type> typeMap;
};

} // namespace vkf::core

#endif // VULKANRENDERER_SHADER_H