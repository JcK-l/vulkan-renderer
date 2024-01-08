// shader:vertex
#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) in vec2 positions;
layout(location = 1) in vec2 texCoords;

layout (location = 0) out vec2 outTexCoords;

const int MAX_SIZE = 32;

layout(push_constant) uniform PushConstants {
    uint indices[MAX_SIZE];
} push;

layout(set = 0, binding = 0) uniform ModelMatrix {
    mat4 modelMatrix;
} model[];

layout(set = 0, binding = 0) uniform ViewMatrix {
    mat4 viewMatrix;
} view[];

mat4 modelMatrix = model[push.indices[1]].modelMatrix;
mat4 viewMatrix = view[push.indices[0]].viewMatrix;

void main() {
    gl_Position = viewMatrix * modelMatrix * vec4(positions, 0.0, 1.0);
    outTexCoords = texCoords;
}

// shader:fragment
#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable

const int MAX_SIZE = 32;

layout(push_constant) uniform PushConstants {
    uint indices[MAX_SIZE];
} push;

layout(set = 0, binding = 2) uniform sampler2D textures[];

layout(location = 0) in vec2 inTexCoords;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = texture(textures[push.indices[2]], inTexCoords);
}
