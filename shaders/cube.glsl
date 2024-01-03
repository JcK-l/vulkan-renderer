// shader:vertex
#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable


layout(location = 0) out vec3 fragPos;
layout(location = 1) out vec3 normal;

vec3 positions[36] = vec3[](
// Front face
vec3(-0.25, -0.25, 0.25), vec3(0.25, -0.25, 0.25), vec3(0.25, 0.25, 0.25),
vec3(-0.25, -0.25, 0.25), vec3(0.25, 0.25, 0.25), vec3(-0.25, 0.25, 0.25),

// Back face
vec3(-0.25, -0.25, -0.25), vec3(0.25, -0.25, -0.25), vec3(0.25, 0.25, -0.25),
vec3(-0.25, -0.25, -0.25), vec3(0.25, 0.25, -0.25), vec3(-0.25, 0.25, -0.25),

// Top face
vec3(-0.25, 0.25, -0.25), vec3(0.25, 0.25, -0.25), vec3(0.25, 0.25, 0.25),
vec3(-0.25, 0.25, -0.25), vec3(0.25, 0.25, 0.25), vec3(-0.25, 0.25, 0.25),

// Bottom face
vec3(-0.25, -0.25, -0.25), vec3(0.25, -0.25, -0.25), vec3(0.25, -0.25, 0.25),
vec3(-0.25, -0.25, -0.25), vec3(0.25, -0.25, 0.25), vec3(-0.25, -0.25, 0.25),

// Right face
vec3(0.25, -0.25, -0.25), vec3(0.25, -0.25, 0.25), vec3(0.25, 0.25, 0.25),
vec3(0.25, -0.25, -0.25), vec3(0.25, 0.25, 0.25), vec3(0.25, 0.25, -0.25),

// Left face
vec3(-0.25, -0.25, -0.25), vec3(-0.25, -0.25, 0.25), vec3(-0.25, 0.25, 0.25),
vec3(-0.25, -0.25, -0.25), vec3(-0.25, 0.25, 0.25), vec3(-0.25, 0.25, -0.25)
);

vec3 normals[36] = vec3[](
// Front face
vec3(0.0, 0.0, 1.0), vec3(0.0, 0.0, 1.0), vec3(0.0, 0.0, 1.0),
vec3(0.0, 0.0, 1.0), vec3(0.0, 0.0, 1.0), vec3(0.0, 0.0, 1.0),

// Back face
vec3(0.0, 0.0, -1.0), vec3(0.0, 0.0, -1.0), vec3(0.0, 0.0, -1.0),
vec3(0.0, 0.0, -1.0), vec3(0.0, 0.0, -1.0), vec3(0.0, 0.0, -1.0),

// Top face
vec3(0.0, 1.0, 0.0), vec3(0.0, 1.0, 0.0), vec3(0.0, 1.0, 0.0),
vec3(0.0, 1.0, 0.0), vec3(0.0, 1.0, 0.0), vec3(0.0, 1.0, 0.0),

// Bottom face
vec3(0.0, -1.0, 0.0), vec3(0.0, -1.0, 0.0), vec3(0.0, -1.0, 0.0),
vec3(0.0, -1.0, 0.0), vec3(0.0, -1.0, 0.0), vec3(0.0, -1.0, 0.0),

// Right face
vec3(1.0, 0.0, 0.0), vec3(1.0, 0.0, 0.0), vec3(1.0, 0.0, 0.0),
vec3(1.0, 0.0, 0.0), vec3(1.0, 0.0, 0.0), vec3(1.0, 0.0, 0.0),

// Left face
vec3(-1.0, 0.0, 0.0), vec3(-1.0, 0.0, 0.0), vec3(-1.0, 0.0, 0.0),
vec3(-1.0, 0.0, 0.0), vec3(-1.0, 0.0, 0.0), vec3(-1.0, 0.0, 0.0)
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

mat4 modelMatrix = model[push.indices[2]].modelMatrix;
mat4 viewMatrix = view[push.indices[0]].viewMatrix;

void main() {
    mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
    vec3 norm = normalize(normalMatrix * normals[gl_VertexIndex]);

    gl_Position = viewMatrix * modelMatrix * vec4(positions[gl_VertexIndex], 1.0);
    fragPos = vec3(modelMatrix * vec4(positions[gl_VertexIndex], 1.0));
    normal = norm;
}

// shader:fragment
#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable

const int MAX_SIZE = 32;

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 normal;

layout(push_constant) uniform PushConstants {
    uint indices[MAX_SIZE];
} push;

layout(set = 0, binding = 0) uniform Color {
    vec4 color;
} colors[];

layout(set = 0, binding = 0) uniform ViewMatrix {
    mat4 viewMatrix;
} view[];

vec4 color = colors[push.indices[1]].color;
mat4 viewMatrix = view[push.indices[0]].viewMatrix;

layout(location = 0) out vec4 outColor;

void main() {
    vec3 result = vec3(0.0, 0.0, 0.0);// Initialize final color to black
    // Ambient
    vec3 ambient = 0.2 * vec3(1.0, 1.0, 1.0);

    vec3 lightDir = normalize(vec3(1.0, -1.0, 1.0));

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
