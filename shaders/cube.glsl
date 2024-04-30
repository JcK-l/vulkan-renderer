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

// shader::vertex
#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) in vec3 positions;
layout(location = 1) in vec3 normals;

layout(location = 0) out vec3 fragPos;
layout(location = 1) out vec3 normal;

mat4 modelMatrix = GET_DATA(model, MODEL_INDEX).modelMatrix;
mat4 viewMatrix = GET_DATA(camera, CAMERA_INDEX).viewMatrix;

void main()
{
    mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
    vec3 norm = normalize(normalMatrix * normals);

    gl_Position = viewMatrix * modelMatrix * vec4(positions, 1.0);
    fragPos = vec3(modelMatrix * vec4(positions, 1.0));
    normal = norm;
}

// shader::fragment
#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 normal;

layout(location = 0) out vec4 outColor;

vec4 color = GET_DATA(colors, COLOR_INDEX).color;
mat4 viewMatrix = GET_DATA(camera, CAMERA_INDEX).viewMatrix;

void main()
{
    vec3 result = vec3(0.0, 0.0, 0.0); // Initialize final color to black
    // Ambient
    vec3 ambient = 0.2 * vec3(1.0, 1.0, 1.0);

    vec3 lightDir = normalize(vec3(3.0, 5.0, 4.0));

    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * 1 * vec3(1.0, 1.0, 1.0);

    // Specular
    mat4 viewMatrixInv = inverse(viewMatrix);
    vec3 viewDir = normalize(vec3(viewMatrixInv * vec4(0.0, 0.0, 0.0, 1.0)) - fragPos);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64.0);
    vec3 specular = spec * 0.5 * vec3(1.0, 1.0, 1.0);

    result = ((diffuse + ambient + specular) * color.rgb);
    outColor = vec4(result, color.a);
}