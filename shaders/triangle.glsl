// shader:vertex
#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable

out gl_PerVertex {
    vec4 gl_Position;
};


vec2 positions[3] = vec2[](
vec2(0.0, -0.5),
vec2(0.5, 0.5),
vec2(-0.5, 0.5)
);

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

uint modelIndex = push.indices[2];
uint viewIndex = push.indices[0];

void main() {
    gl_Position = view[viewIndex].viewMatrix * model[modelIndex].modelMatrix * vec4(positions[gl_VertexIndex], 0.0, 1.0);
}

// shader:fragment
#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable

const int MAX_SIZE = 32;

layout(push_constant) uniform PushConstants {
    uint indices[MAX_SIZE];
} push;

uint colorIndex = push.indices[1];

layout(set = 0, binding = 0) uniform UniformBufferObject {
    vec4 color;
} ubo[];

layout(location = 0) out vec4 outColor;

void main() {
    outColor = ubo[colorIndex].color;
}
