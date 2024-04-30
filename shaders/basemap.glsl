/******************************************************************************
**
**  This file is part of Met.3D -- a research environment for the
**  three-dimensional visual exploration of numerical ensemble weather
**  prediction data.
**
**  Copyright 2015-2017 Marc Rautenhaus
**  Copyright 2015-2017 Michael Kern
**  Copyright 2015-2017 Bianca Tost
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
#define DATA_INDEX 1
#define TEXTURE_INDEX 2

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

NEW_UNIFORM_BUFFER(data, {
    vec4 cornersData;
    float poleLat;
    float poleLon;
    float colorIntensity; // 0..1 with 1 = rgb texture used, 0 = grey scales
    float padding;
});

NEW_UNIFORM_BUFFER(camera, { mat4 viewMatrix; });

NEW_TEXTURE(mapTexture);

/*****************************************************************************
 ***                           VERTEX SHADER
 *****************************************************************************/
// shader::vertex
#version 450
#extension GL_EXT_nonuniform_qualifier : enable

mat4 mvpMatrix = GET_DATA(camera, CAMERA_INDEX).viewMatrix;
vec4 cornersData = GET_DATA(data, DATA_INDEX).cornersData;

layout(location = 0) in vec2 position;

layout(location = 0) out smooth vec2 texCoord2D;

// shader VSmain(in vec4 vertex0 : 0, in vec2 texCoord2D0 : 1, out VStoFS Output)
void main()
{
    // calculate the texture coordinates on the fly
    float texCoordX = (position.x - cornersData.x) / (cornersData.z - cornersData.x);
    float texCoordY = (cornersData.w - position.y) / (cornersData.w - cornersData.y);

    gl_Position = mvpMatrix * vec4(position.x, 0, -position.y, 1);
    texCoord2D = vec2(texCoordX, texCoordY);
}

/*****************************************************************************
 ***                          FRAGMENT SHADER
 *****************************************************************************/
// shader::fragment
#version 450
#extension GL_EXT_nonuniform_qualifier : enable

float colorIntensity = GET_DATA(data, DATA_INDEX).colorIntensity; // 0..1 with 1 = rgb texture used, 0 = grey scales

layout(location = 0) in smooth vec2 texCoord2D;

layout(location = 0) out vec4 fragColour;

// Changes the intensity of the color value given in rgbaColour according to
// the scale given in colorIntensity [0 -> gray, 1 -> unchanged saturation].
vec4 adaptColourIntensity(vec4 rgbaColour);

void main()
{
    vec4 rgbaColour = vec4(texture(GET_DATA(mapTexture, TEXTURE_INDEX), texCoord2D).rgb, 1);

    fragColour = adaptColourIntensity(rgbaColour);
}

vec4 adaptColourIntensity(vec4 rgbaColour)
{
    // If colorIntensity < 1 turn RGB texture into grey scales. See
    // http://stackoverflow.com/questions/687261/converting-rgb-to-grayscale-intensity
    vec3 rgbColour = rgbaColour.rgb;
    vec3 grey = vec3(0.2989, 0.5870, 0.1140);
    vec3 greyColour = vec3(dot(rgbColour, grey));
    return vec4(mix(greyColour, rgbColour, colorIntensity), rgbaColour.a);
}