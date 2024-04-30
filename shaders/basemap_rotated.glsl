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

layout(location = 0) out smooth vec2 position2D;

// Vertex shader for base maps on rotated grid. Uses coordinates of bounding box
// in rotated coordinates to get potential render area. Does not compute texture
// coordinates since we first need to trasform the rotated coordinates back to
// real geographical coordinates before sampling in texture (done in fragment
// shader).
void main()
{
    // Store position in rotated coordinates but not projected to be able to
    // sample the map in the fragment shader.
    position2D = vec2(position.x, position.y);
    gl_Position = mvpMatrix * vec4(position.x, 0, -position.y, 1);
}

/*****************************************************************************
 ***                          FRAGMENT SHADER
 *****************************************************************************/
// shader::fragment
#version 450
#extension GL_EXT_nonuniform_qualifier : enable

#define M_PI 3.1415926535897932384626433832795
#define DEG2RAD M_PI / 180.0
#define RAD2DEG 180.0 / M_PI

float colorIntensity = GET_DATA(data, DATA_INDEX).colorIntensity; // 0..1 with 1 = rgb texture used, 0 = grey scales
vec4 cornersData = GET_DATA(data, DATA_INDEX).cornersData;
float poleLat = GET_DATA(data, DATA_INDEX).poleLat;
float poleLon = GET_DATA(data, DATA_INDEX).poleLon;

layout(location = 0) in smooth vec2 position2D;

layout(location = 0) out vec4 fragColour;

// Changes the intensity of the color value given in rgbaColour according to
// the scale given in colorIntensity [0 -> gray, 1 -> unchanged saturation].
vec4 adaptColourIntensity(vec4 rgbaColour);

// Parts of the following method have been ported from the C implementation of
// the methods 'lamrot_to_lam' and 'phirot_to_phi'. The original code has been
// published under GNU GENERAL PUBLIC LICENSE Version 2, June 1991.
// source: https://code.zmaw.de/projects/cdo/files  [Version 1.8.1]
// Necessary code duplicate in naturalearthdataloader.cpp . (Contains copy of
// original code.)
vec2 rotatedToGeograhpicalCoords(vec2 position);

// Fragment shader for base map on rotated grids but with bounding box
// coordinates given by the user treated as rotated coordinates.
void main()
{
    // Get position in real geographical coordinates.
    vec2 position = rotatedToGeograhpicalCoords(position2D);

    // Get texture coordinates of position.
    float texCoordX = (position.x - cornersData.x) / (cornersData.z - cornersData.x);
    float texCoordY = (cornersData.w - position.y) / (cornersData.w - cornersData.y);
    vec4 rgbaColour = vec4(texture(GET_DATA(mapTexture, TEXTURE_INDEX), vec2(texCoordX, texCoordY)).rgb, 1);

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

vec2 rotatedToGeograhpicalCoords(vec2 position)
{
    // Early break for rotation values with no effect.
    if ((poleLon == -180. || poleLon == 180.) && poleLat == 90.)
    {
        return position;
    }

    float result = 0.0f;

    // Get longitude and latitude from position.
    float rotLon = position.x;
    float rotLat = position.y;

    if (rotLon > 180.0f)
    {
        rotLon -= 360.0f;
    }

    // Convert degrees to radians.
    float poleLatRad = DEG2RAD * poleLat;
    float poleLonRad = DEG2RAD * poleLon;
    float rotLonRad = DEG2RAD * rotLon;

    // Compute sinus and cosinus of some coordinates since they are needed more
    // often later on.
    float sinPoleLat = sin(poleLatRad);
    float cosPoleLat = cos(poleLatRad);
    float sinRotLatRad = sin(DEG2RAD * rotLat);
    float cosRotLatRad = cos(DEG2RAD * rotLat);
    float cosRotLonRad = cos(DEG2RAD * rotLon);

    // Apply the transformation (conversation to Cartesian coordinates and  two
    // rotations; difference to original code: no use of polgam).

    float x = (cos(poleLonRad) * (((-sinPoleLat) * cosRotLonRad * cosRotLatRad) + (cosPoleLat * sinRotLatRad))) +
              (sin(poleLonRad) * sin(rotLonRad) * cosRotLatRad);
    float y = (sin(poleLonRad) * (((-sinPoleLat) * cosRotLonRad * cosRotLatRad) + (cosPoleLat * sinRotLatRad))) -
              (cos(poleLonRad) * sin(rotLonRad) * cosRotLatRad);
    float z = cosPoleLat * cosRotLatRad * cosRotLonRad + sinPoleLat * sinRotLatRad;

    // Avoid invalid values for z (Might occure due to inaccuracies in
    // computations).
    z = max(-1., min(1., z));

    // Compute spherical coordinates from Cartesian coordinates and convert
    // radians to degrees.

    if (abs(x) > 0.f)
    {
        result = RAD2DEG * atan(y, x);
    }
    if (abs(result) < 9.e-14)
    {
        result = 0.f;
    }

    position.x = result;
    position.y = RAD2DEG * (asin(z));

    return position;
}