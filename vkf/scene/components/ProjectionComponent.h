/// \file
/// \brief

//
// Created by Joshua Lowe on 1/22/2024.
// The license and distribution terms for this file may be found in the file LICENSE in this distribution
//

#pragma once

namespace vkf::scene
{

enum class ProjectionType
{
    CYLINDRICAL,
    PROJ_LIBRARY,
    ROTATEDLATLON
};

enum ProjectionFlags
{
    CYLINDRICAL_ENABLE = 1 << 0,
    PROJ_LIBRARY_ENABLE = 1 << 1,
    ROTATEDLATLON_ENABLE = 1 << 2
};

struct ProjectionComponent
{

    void updateGui();

    ProjectionType mapProjection{ProjectionType::CYLINDRICAL};

    std::string projLibraryString = "+proj=stere +lat_0=90 +lon_0=0 +x_0=0 +y_0=0 +datum=WGS84 +units=m +no_defs";
    //    std::string projLibraryString =
    //        "+proj=lcc +lat_1=33 +lat_2=45 +lat_0=39 +lon_0=-96 +x_0=0 +y_0=0 +datum=WGS84 +units=m +no_defs";

    float rotatedNorthPoleLongitude{179.9};
    float rotatedNorthPoleLatitude{89.9};

    int projectionFlags{CYLINDRICAL_ENABLE | PROJ_LIBRARY_ENABLE | ROTATEDLATLON_ENABLE};
};

} // namespace vkf::scene