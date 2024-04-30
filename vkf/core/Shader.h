////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file Shader.h
/// \brief This file declares the Shader class which is used for managing Vulkan shaders.
///
/// The Shader class is part of the vkf::core namespace. It provides functionality to compile shaders.
///
/// \author Joshua Lowe
/// \date 12/10/2023
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <shaderc/shaderc.hpp>
// #include <spirv_cross/spirv_cross.hpp>

// Forward declarations
#include "CoreFwd.h"

namespace vkf::core
{

/// \class Shader
/// \brief Class for managing Vulkan shaders.
///
/// This class provides functionality to compile shaders.
///
class Shader
{
  public:
    /// \enum Type
    /// \brief Enum for shader types.
    ///
    /// This enum is used to specify the type of a shader.
    ///
    enum class Type
    {
        Unknown,  ///< Unknown shader type
        Vertex,   ///< Vertex shader
        Fragment, ///< Fragment shader
        Geometry, ///< Geometry shader
        Global    ///< Global data
        // Add more shader types as needed
    };

    ///
    /// \brief Constructor that takes a file path as parameter.
    ///
    /// This constructor takes a file path as parameter and compiles the shader.
    ///
    /// \param filePath The file path of the shader.
    ///
    explicit Shader(const std::string &filePath);

    Shader(const Shader &) = delete;            ///< Deleted copy constructor
    Shader(Shader &&) noexcept = default;       ///< Default move constructor
    Shader &operator=(const Shader &) = delete; ///< Deleted copy assignment operator
    Shader &operator=(Shader &&) = delete;      ///< Deleted move assignment operator
    ~Shader() = default;                        ///< Default destructor

    /// \brief Method to get shader codes.
    ///
    /// This method returns a map of shader types and their corresponding codes.
    [[nodiscard]] const std::unordered_map<Type, std::string> &getCodes() const;

    /// \brief Method to create shader stages.
    ///
    /// This method creates and returns a vector of pipeline shader stage create info objects.
    std::vector<vk::PipelineShaderStageCreateInfo> createShaderStages(const Device &device);

  private:
    vk::raii::ShaderModule compileShader(const Device &device, Type type);

    Type stringToType(const std::string &typeString) const;
    shaderc_shader_kind typeToShadercKind(Type type) const;
    std::string readFile(const std::string &filePath) const;
    void parseShader(std::string &shaderString);

    std::unordered_map<Type, std::string> codes; ///< Map of shader types and their corresponding codes.
    std::vector<vk::raii::ShaderModule> shaderModules;

    static const std::unordered_map<std::string, Type>
        typeMap; ///< Map of string representations of shader types and their corresponding enum values.
};

} // namespace vkf::core