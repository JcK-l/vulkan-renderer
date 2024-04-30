/******************************************************************************
**
**  This file is part of Met.3D -- a research environment for the
**  three-dimensional visual exploration of numerical ensemble weather
**  prediction data.
**
**  Copyright 2020 Marc Rautenhaus
**
**  Regional Computing Center, Visual Data Analysis Group
**  Universitaet Hamburg, Hamburg, Germany
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
*******************************************************************************/

#pragma once

// standard library imports
#include <functional>

// related third party imports
#include "cpl_error.h"
#include "ogrsf_frmts.h"
#include <proj.h>

// local application imports

namespace Met3D
{

/******************************************************************************
***                               CONSTANTS                                 ***
*******************************************************************************/

// Definition of atmospheric science constants. Reference: e.g. Wallace and
// Hobbs (2006), pp. 467ff.
namespace MetConstants
{
// Universal constants

// Air
const double GAS_CONSTANT_DRY_AIR = 287.058;              // J K^-1 kg^-1
const double SPECIFIC_HEAT_DRYAIR_CONST_PRESSURE = 1004.; // J K^-1 kg^-1

// Water substance

// Earth and sun
const double GRAVITY_ACCELERATION = 9.80665; // m s^-2
const double EARTH_RADIUS_km = 6371.;        // km

// Source: http://hpiers.obspm.fr/eop-pc/models/constants.html
const double EARTHS_ANGULAR_SPEED_OF_ROTATION = 7.292115E-5; // rad/s

// Scaling factor for projections, since Met.3D has originally been designed
// such that horizontal coordinates are in the range (-180..180 and -90..90)
// (lon and lat). For projection coordinates that are given in e.g. meters,
// we need to scale to fit into that range. Example: 1.e6 will scale
// stereographic units in m to 10^3 km.
const double scaleFactorToFitProjectedCoordsTo360Range = 1.e6;
} // namespace MetConstants

struct PointF
{
    float x;
    float y;

    bool operator==(const PointF &other) const
    {
        return x == other.x && y == other.y;
    }

    PointF operator-(const PointF &other) const
    {
        return PointF{x - other.x, y - other.y};
    }
};

struct Vector2D
{
    explicit Vector2D(const PointF &point) : x{point.x}, y{point.y}
    {
    }

    [[nodiscard]] float length() const
    {
        return std::sqrt(x * x + y * y);
    }

    float x;
    float y;
};

struct RectF
{
    float left{0.0f};   // x
    float top{0.0f};    // y
    float right{0.0f};  // x + width
    float bottom{0.0f}; // y + height

    [[nodiscard]] float width() const
    {
        return right - left;
    }
    [[nodiscard]] float height() const
    {
        return top - bottom;
    }
};

/**
  @brief MGeometryHandling provides methods to create, load, and transform 2D
  geometries. Geometries are generally handled as @ref std::vector<std::vector<PointF>>.
  Methods include loading geometry from shapefiles, generation of graticule
  geometry, projection methods for rotated lon-lat and proj-supported projections,
  and clipping to bounding boxes.
 */
class GeometryHandling
{
  public:
    GeometryHandling();
    virtual ~GeometryHandling();

    std::vector<std::vector<PointF>> generate2DGraticuleGeometry(std::vector<float> &longitudes,
                                                                 std::vector<float> &latitudes,
                                                                 Vector2D lonLatVertexSpacing);

    std::vector<std::vector<PointF>> read2DGeometryFromShapefile(std::string fname, RectF bbox);

    void initProjProjection(std::string projString);

    void destroyProjProjection();

    PointF geographicalToProjectedCoordinates(PointF point, bool inverse = false);

    std::vector<std::vector<PointF>> geographicalToProjectedCoordinates(
        const std::vector<std::vector<PointF>> &polygons, bool inverse = false);

    void initRotatedLonLatProjection(PointF rotatedPoleLonLat);

    PointF geographicalToRotatedCoordinates(PointF point);

    std::vector<std::vector<PointF>> geographicalToRotatedCoordinates(const std::vector<std::vector<PointF>> &polygons);

    PointF rotatedToGeographicalCoordinates(PointF point);

    std::vector<std::vector<PointF>> splitLineSegmentsLongerThanThreshold(
        const std::vector<std::vector<PointF>> &polygons, double thresholdDistance);

    std::vector<std::vector<PointF>> enlargeGeometryToBBoxIfNecessary(std::vector<std::vector<PointF>> polygons,
                                                                      RectF bbox);

    std::vector<std::vector<PointF>> clipPolygons(const std::vector<std::vector<PointF>> &polygons, RectF bbox);

    void flattenPolygonsToVertexList(const std::vector<std::vector<PointF>> &polygons, std::vector<float> *vertexList,
                                     std::vector<int> *polygonStartIndices, std::vector<int> *polygonVertexCount);

    /**
     * @brief canConnectPointPairInProjection_
     * @param p1 First point
     * @param p2 Second point
     *
     * Functions to check supported projection whether a line between p1 and p2 crosses the domain boundary. Then, the
     * graticule should not be drawm.
     * @return whether the boundary is crossed
     */
    bool canConnectPointPairInProjectionLcc(PointF p1, PointF p2, float lon_0);
    bool canConnectPointPairInProjectionLatlong(PointF p1, PointF p2);
    bool canConnectPointPairInProjectionStereographic(PointF p1, PointF p2);

  private:
    int cohenSutherlandCode(PointF &point, RectF &bbox) const;

    bool cohenSutherlandClip(PointF *p1, PointF *p2, RectF &bbox);

    OGRPolygon *convertQRectToOGRPolygon(RectF &rect);

    std::vector<PointF> convertOGRLineStringToQPolygon(OGRLineString *lineString);

    void appendOGRLineStringsFromOGRGeometry(std::vector<OGRLineString *> *lineStrings, OGRGeometry *geometry);

    // active transformations
    PJ *pjSrcDstTransformation, *pjDstSrcTransformation;
    // active callbacks to connection checking functions
    std::function<bool(PointF, PointF)> srcConnectionFunc;
    std::function<bool(PointF, PointF)> dstConnectionFunc;
    // whether the magic scaling constant should be used for projections (typically used when projection units resemble
    // meters instead of degrees)
    bool srcUseScaleFactorForProjection, dstUseScaleFactorForProjection;

    PointF rotatedPole;
};

} // namespace Met3D