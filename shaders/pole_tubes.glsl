/******************************************************************************
**
**  This file is part of Met.3D -- a research environment for the
**  three-dimensional visual exploration of numerical ensemble weather
**  prediction data.
**
**  Copyright 2015 Marc Rautenhaus
**  Copyright 2015 Michael Kern
**
**  Computer Graphics and Visualization Group
**  Technische Universitaet Muenchen, Garching, Germany
**
**  Met.3D is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  Met.3D is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with Met.3D.  If not, see <http://www.gnu.org/licenses/>.
**
******************************************************************************/

// shader::global
#define BINDLESS 1

#define DESCRIPTOR_SET 0

#define CAMERA_INDEX 0
#define MODEL_INDEX 1
#define DATA_INDEX 2

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

NEW_UNIFORM_BUFFER(data, {
    vec4 geometryColor;
    vec2 pToWorldZParams;
    vec3 cameraPosition;
    vec3 offsetDirection;
    float tubeRadius;
    float endSegmentOffset;
});

struct TubeGeometryInfo
{
    vec3 pos;      // current world position
    vec3 normal;   // normal to the line segment tangent
    vec3 binormal; // binormal, perpendicular to the normal and tangent
    vec3 tangent;  // tangent of segment
    float radius;  // tube radius
    float value;   // [optional value]
};

const int NUM_TUBESEGMENTS = 8;

/*****************************************************************************
 ***                           VERTEX SHADER
 *****************************************************************************/
// shader::vertex
#version 450
#extension GL_EXT_nonuniform_qualifier : enable

vec2 pToWorldZParams = GET_DATA(data, DATA_INDEX).pToWorldZParams;

layout(location = 0) in vec3 pos;

layout(location = 0) out VStoGSWorld
{
    vec3 worldPos;
}
Output;

void main()
{
    float worldZ = (log(pos.z) - pToWorldZParams.x) * pToWorldZParams.y;
    Output.worldPos = vec3(pos.x, worldZ, -pos.y);
}

/*****************************************************************************
 ***                          GEOMETRY SHADER
 *****************************************************************************/
// shader::geometry
#version 450
#extension GL_EXT_nonuniform_qualifier : enable

layout(lines) in;
layout(triangle_strip, max_vertices = 128) out;

float tubeRadius = GET_DATA(data, DATA_INDEX).tubeRadius;
float endSegmentOffset = GET_DATA(data, DATA_INDEX).endSegmentOffset;
mat4 modelMatrix = GET_DATA(model, MODEL_INDEX).modelMatrix;
mat4 viewMatrix = GET_DATA(camera, CAMERA_INDEX).viewMatrix;

layout(location = 0) in VStoGSWorld
{
    vec3 worldPos;
}
Input[];

layout(location = 0) out GStoFSSimple
{
    smooth vec3 worldPos;
    smooth vec3 normal;
}
Output;

void calculateRayBasis(in vec3 prevPos, in vec3 nextPos, out vec3 normal, out vec3 binormal);

void generateTube(in TubeGeometryInfo prev, in TubeGeometryInfo next, out vec3 tubeWorlds[(NUM_TUBESEGMENTS + 1) * 2],
                  out vec3 tubeNormals[(NUM_TUBESEGMENTS + 1) * 2], out float tubeValues[(NUM_TUBESEGMENTS + 1) * 2]);

void generateTubeEnd(in TubeGeometryInfo end, in float endOffset, out vec3 tubeWorlds[(NUM_TUBESEGMENTS + 1) * 3],
                     out vec3 tubeNormals[(NUM_TUBESEGMENTS + 1) * 3],
                     out float tubeValues[(NUM_TUBESEGMENTS + 1) * 3]);

void main()
{
    vec3 prevPos = Input[0].worldPos;
    vec3 nextPos = Input[1].worldPos;

    vec3 tangent = normalize(nextPos - prevPos);
    vec3 normal = vec3(0);
    vec3 binormal = vec3(0);
    calculateRayBasis(prevPos, nextPos, normal, binormal);

    const int numVertices = (NUM_TUBESEGMENTS + 1) * 2;
    vec3 tubeWorlds[numVertices];
    vec3 tubeNormals[numVertices];
    float tubeValues[numVertices];

    TubeGeometryInfo prevInfo = {prevPos, normal, binormal, tangent, tubeRadius, 0};
    TubeGeometryInfo nextInfo = {nextPos, normal, binormal, tangent, tubeRadius, 0};

    generateTube(prevInfo, nextInfo, tubeWorlds, tubeNormals, tubeValues);

    int numTubeSegments = NUM_TUBESEGMENTS;
    float anglePerSegment = 360. / float(numTubeSegments);

    for (int t = 0; t < numVertices; ++t)
    {
        gl_Position = viewMatrix * modelMatrix * vec4(tubeWorlds[t], 1);
        Output.normal = tubeNormals[t];
        Output.worldPos = tubeWorlds[t];
        EmitVertex();
    }

    EndPrimitive();

    const int numVerticesEnd = (NUM_TUBESEGMENTS + 1) * 3;

    vec3 tubeWorldsEnd[numVerticesEnd];
    vec3 tubeNormalsEnd[numVerticesEnd];
    float tubeValuesEnd[numVerticesEnd];

    tangent = normalize(prevPos - nextPos);

    TubeGeometryInfo endInfo = TubeGeometryInfo(prevPos, normal, binormal, tangent, tubeRadius, 0);

    // Generate the ends of each tube
    generateTubeEnd(endInfo, endSegmentOffset, tubeWorldsEnd, tubeNormalsEnd, tubeValuesEnd);

    for (int t = 0; t < numVerticesEnd; ++t)
    {
        gl_Position = viewMatrix * modelMatrix * vec4(tubeWorldsEnd[t], 1);
        Output.normal = tubeNormalsEnd[t];
        Output.worldPos = tubeWorldsEnd[t];
        EmitVertex();
    }

    tangent = normalize(nextPos - prevPos);
    endInfo.pos = nextPos;
    endInfo.tangent = tangent;

    // Generate the ends of each tube
    generateTubeEnd(endInfo, endSegmentOffset, tubeWorldsEnd, tubeNormalsEnd, tubeValuesEnd);

    for (int t = 0; t < numVerticesEnd; ++t)
    {
        gl_Position = viewMatrix * modelMatrix * vec4(tubeWorldsEnd[t], 1);
        Output.normal = tubeNormalsEnd[t];
        Output.worldPos = tubeWorldsEnd[t];
        EmitVertex();
    }

    EndPrimitive();
}

// Calculate normal and binormal from a given direction
void calculateRayBasis(in vec3 prevPos, in vec3 nextPos, out vec3 normal, out vec3 binormal)
{
    const vec3 tangent = nextPos - prevPos;

    if (length(normal) <= 0.01)
    {
        normal = cross(tangent, vec3(0, 0, 1));

        if (length(normal) <= 0.01)
        {
            normal = cross(tangent, vec3(1, 0, 0));

            if (length(normal) <= 0.01)
            {
                normal = cross(tangent, vec3(0, 0, 1));
            }
        }
    }

    normal = normalize(normal);

    binormal = cross(tangent, normal);
    binormal = normalize(binormal);

    normal = cross(binormal, tangent);
    normal = normalize(normal);
}

void generateTube(in TubeGeometryInfo prev, in TubeGeometryInfo next, out vec3 tubeWorlds[(NUM_TUBESEGMENTS + 1) * 2],
                  out vec3 tubeNormals[(NUM_TUBESEGMENTS + 1) * 2], out float tubeValues[(NUM_TUBESEGMENTS + 1) * 2])
{
    // Start tube generation.
    int numTubeSegments = NUM_TUBESEGMENTS;
    float anglePerSegment = 360. / float(numTubeSegments);

    int cc = 0;

#pragma unroll
    for (int t = 0; t <= numTubeSegments; ++t)
    {
        // Get the next angle in radians
        float angle = radians(anglePerSegment * t);
        // Compute the sine and cosine of the circle
        float cosi = cos(angle);
        float sini = sin(angle);

        // Compute the next vertices along the circle for start and end position
        vec3 startWorldPos = prev.pos + (cosi * prev.normal + sini * prev.binormal) * prev.radius;
        vec3 endWorldPos = next.pos + (cosi * next.normal + sini * next.binormal) * next.radius;

        // Emit two new vertices to create the tube
        tubeValues[cc] = prev.value;
        tubeNormals[cc] = normalize(startWorldPos - prev.pos);
        tubeWorlds[cc] = startWorldPos;

        cc++;

        tubeValues[cc] = next.value;
        tubeNormals[cc] = normalize(endWorldPos - next.pos);
        tubeWorlds[cc] = endWorldPos;

        cc++;
    }
}

void generateTubeEnd(in TubeGeometryInfo end, in float endOffset, out vec3 tubeWorlds[(NUM_TUBESEGMENTS + 1) * 3],
                     out vec3 tubeNormals[(NUM_TUBESEGMENTS + 1) * 3], out float tubeValues[(NUM_TUBESEGMENTS + 1) * 3])
{
    // Start tube generation.
    const int numTubeSegments = NUM_TUBESEGMENTS;
    const float anglePerSegment = 360. / float(numTubeSegments);

    const vec3 endMiddlePos = end.pos + endOffset * end.tangent;

    int cc = 0;

#pragma unroll
    for (int t = 0; t <= numTubeSegments; ++t)
    {
        // Get the next angle in radians
        float angle = radians(anglePerSegment * t);
        // Compute the sine and cosine of the circle
        float cosi = cos(angle);
        float sini = sin(angle);

        // Compute the next vertices along the circle for start and end position
        vec3 worldPos = end.pos + (cosi * end.normal + sini * end.binormal) * end.radius;
        tubeValues[cc] = end.value;
        tubeNormals[cc] = normalize(worldPos - end.pos);
        tubeWorlds[cc] = worldPos;

        cc++;

        tubeWorlds[cc] = endMiddlePos;
        tubeNormals[cc] = end.tangent;
        tubeValues[cc] = end.value;

        cc++;

        angle = radians(anglePerSegment * (t + 1));
        cosi = cos(angle);
        sini = sin(angle);

        worldPos = end.pos + (cosi * end.normal + sini * end.binormal) * end.radius;

        tubeValues[cc] = end.value;
        tubeNormals[cc] = normalize(worldPos - end.pos);
        tubeWorlds[cc] = worldPos;

        cc++;
    }
}

/*****************************************************************************
 ***                          FRAGMENT SHADER
 *****************************************************************************/
// shader::fragment
#version 450
#extension GL_EXT_nonuniform_qualifier : enable

vec4 geometryColor = GET_DATA(data, DATA_INDEX).geometryColor;
vec3 cameraPosition = GET_DATA(data, DATA_INDEX).cameraPosition;

layout(location = 0) in GStoFSSimple
{
    smooth vec3 worldPos;
    smooth vec3 normal;
}
Input;

layout(location = 0) out vec4 fragColor;

void getBlinnPhongColor(in vec3 worldPos, in vec3 normalDir, in vec3 ambientColor, out vec3 color);

void main()
{
    vec3 surfaceColor = vec3(0);
    vec3 ambientColor = geometryColor.xyz;
    getBlinnPhongColor(Input.worldPos, Input.normal, ambientColor, surfaceColor);

    fragColor = vec4(surfaceColor, 1);
}

// Compute color of blinn-phong shaded surface
void getBlinnPhongColor(in vec3 worldPos, in vec3 normalDir, in vec3 ambientColor, out vec3 color)
{
    const vec3 lightColor = vec3(1, 1, 1);
    vec3 lightDirection = normalize(vec3(3.0, 5.0, 4.0));

    const vec3 kA = 0.3 * ambientColor;
    const vec3 kD = 0.5 * ambientColor;
    const float kS = 0.2;
    const float s = 10;

    const vec3 n = normalize(normalDir);
    const vec3 v = normalize(cameraPosition - worldPos);
    const vec3 l = normalize(-lightDirection); // specialCase
    const vec3 h = normalize(v + l);

    vec3 diffuse = kD * clamp(abs(dot(n, l)), 0.0, 1.0) * lightColor;
    vec3 specular = kS * pow(clamp(abs(dot(n, h)), 0.0, 1.0), s) * lightColor;

    color = kA + diffuse + specular;
}