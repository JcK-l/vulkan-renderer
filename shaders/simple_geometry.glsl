// shader::global
#define BINDLESS 1

#define DESCRIPTOR_SET 0

#define CAMERA_INDEX 0
#define COLOR_INDEX 1
#define MODEL_INDEX 2

#define UNIFORM_BINDING 0
#define STORAGE_BINDING 1
#define TEXTURE_BINDING 2

#define MAX_PUSH_CONSTANTS 32

#define INIT_PUSH_CONSTANTS                                                                                            \
    layout(push_constant) uniform PushConstants                                                                        \
    {                                                                                                                  \
        uint indices[MAX_PUSH_CONSTANTS];                                                                              \
    }                                                                                                                  \
    pushConstants

#define NEW_UNIFORM_BUFFER(name, data)                                                                                 \
    layout(set = DESCRIPTOR_SET, binding = UNIFORM_BINDING) uniform name##Buffer data name[]

#define NEW_STORAGE_BUFFER(bufferLayout, bufferAccess, name, data)                                                     \
    layout(bufferLayout, set = DESCRIPTOR_SET, binding = UNIFORM_BINDING) bufferAccess buffer name##Buffer data name[]

#define NEW_TEXTURE(name) layout(set = DESCRIPTOR_SET, binding = TEXTURE_BINDING) uniform sampler2D name[]

#define GET_DATA(name, index) name[pushConstants.indices[index]]

INIT_PUSH_CONSTANTS;

NEW_UNIFORM_BUFFER(model, { mat4 modelMatrix; });

NEW_UNIFORM_BUFFER(camera, { mat4 viewMatrix; });

NEW_UNIFORM_BUFFER(colors, { vec4 color; });

/*****************************************************************************
 ***                           VERTEX SHADER
 *****************************************************************************/
// shader::vertex
#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) in vec2 positions;

mat4 modelMatrix = GET_DATA(model, MODEL_INDEX).modelMatrix;
mat4 viewMatrix = GET_DATA(camera, CAMERA_INDEX).viewMatrix;

void main()
{
    gl_Position = viewMatrix * modelMatrix * vec4(positions.x, 0.3, -positions.y, 1.0);
}

/*****************************************************************************
 ***                          FRAGMENT SHADER
 *****************************************************************************/
// shader::fragment
#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) out vec4 outColor;

vec4 color = GET_DATA(colors, COLOR_INDEX).color;

void main()
{
    outColor = vec4(color);
}