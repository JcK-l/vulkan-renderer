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

mat4 modelMatrix = GET_DATA(model, MODEL_INDEX).modelMatrix;
mat4 viewMatrix = GET_DATA(camera, CAMERA_INDEX).viewMatrix;
vec3 offsetDirection = GET_DATA(data, DATA_INDEX).offsetDirection;

layout(points) in;
layout(line_strip, max_vertices = 128) out;

layout(location = 0) in VStoGSWorld
{
    vec3 worldPos;
}
Input[];

void main()
{
    const vec3 prevPos = Input[0].worldPos;
    const vec3 nextPos = prevPos + offsetDirection;

    gl_Position = viewMatrix * modelMatrix * vec4(prevPos, 1);
    EmitVertex();

    gl_Position = viewMatrix * modelMatrix * vec4(nextPos, 1);
    EmitVertex();
}

/*****************************************************************************
 ***                          FRAGMENT SHADER
 *****************************************************************************/
// shader::fragment
#version 450
#extension GL_EXT_nonuniform_qualifier : enable

vec4 geometryColor = GET_DATA(data, DATA_INDEX).geometryColor;

layout(location = 0) out vec4 fragColor;

void main()
{
    fragColor = vec4(geometryColor);
}