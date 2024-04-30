/******************************************************************************
**
**  This file is part of Met.3D -- a research environment for the
**  three-dimensional visual exploration of numerical ensemble weather
**  prediction data.
**
**  Copyright 2020-2021 Marc Rautenhaus
**  Copyright 2023 Christoph Fischer
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
#include "GeometryHandling.h"
#include "Log.h"
#include "Utility.h"

// standard library imports
#include <cmath>
#include <iostream>
#include <regex>

using namespace std;

namespace Met3D
{

/******************************************************************************
***                     CONSTRUCTOR / DESTRUCTOR                            ***
*******************************************************************************/

GeometryHandling::GeometryHandling()
    : pjSrcDstTransformation(nullptr), pjDstSrcTransformation(nullptr), srcUseScaleFactorForProjection(true),
      dstUseScaleFactorForProjection(true), srcConnectionFunc(nullptr), dstConnectionFunc(nullptr),
      rotatedPole(PointF(0., 90.))
{
}

GeometryHandling::~GeometryHandling()
{
    destroyProjProjection();
}

/******************************************************************************
***                            PUBLIC METHODS                               ***
*******************************************************************************/

std::vector<std::vector<PointF>> GeometryHandling::generate2DGraticuleGeometry(std::vector<float> &longitudes,
                                                                               std::vector<float> &latitudes,
                                                                               Vector2D lonLatVertexSpacing)
{
    std::vector<std::vector<PointF>> graticuleGeometry;

    // Non-empty lists of lons and lats are required.
    if (longitudes.empty() || latitudes.empty())
    {
        return graticuleGeometry;
    }
    // Positive vertex spacing is required.
    if (lonLatVertexSpacing.x <= 0.)
        lonLatVertexSpacing.x = 1.;
    if (lonLatVertexSpacing.y <= 0.)
        lonLatVertexSpacing.y = 1.;

    // List of lons and lats needs to be in ascending order.
    std::sort(longitudes.begin(), longitudes.end());
    std::sort(latitudes.begin(), latitudes.end());

    // Generate meridians (lines of constant longitude).
    if (latitudes.size() > 1)
    {
        for (float lon : longitudes)
        {
            std::vector<PointF> meridian;
            for (float lat = latitudes.front(); lat <= latitudes.back(); lat += lonLatVertexSpacing.y)
            {
                meridian.emplace_back(PointF(lon, lat));
            }
            graticuleGeometry.emplace_back(meridian);
        }
    }

    // Generate parallels (lines of constant latitude).
    if (longitudes.size() > 1)
    {
        for (float lat : latitudes)
        {
            std::vector<PointF> parallel;
            for (float lon = longitudes.front(); lon <= longitudes.back(); lon += lonLatVertexSpacing.x)
            {
                parallel.emplace_back(PointF(lon, lat));
            }
            graticuleGeometry.emplace_back(parallel);
        }
    }

    return graticuleGeometry;
}

std::vector<std::vector<PointF>> GeometryHandling::read2DGeometryFromShapefile(std::string fname, RectF bbox)
{
    LOG_DEBUG("Loading shapefile geometry from file {}...", fname);

    std::vector<std::vector<PointF>> polygons;

    // Open Shapefile.
    auto *gdalDataSet = (GDALDataset *)GDALOpenEx(fname.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);

    if (gdalDataSet == NULL)
    {
        LOG_ERROR("ERROR: cannot open shapefile {}.", fname);
        return polygons;
    }

    // NaturalEarth shapefiles only contain one layer. (Do shapefiles in
    // general contain only one layer?)
    OGRLayer *layer;
    layer = gdalDataSet->GetLayer(0);

    // Filter the layer on-load: Only load those geometries that intersect
    // with the bounding box.
    OGRPolygon *bboxPolygon = convertQRectToOGRPolygon(bbox);
    layer->SetSpatialFilter(bboxPolygon);

    // Loop over all features contained in the layer, add all OGRLineStrings
    // to our list of polygons.
    layer->ResetReading();
    OGRFeature *feature;
    while ((feature = layer->GetNextFeature()) != NULL)
    {
        std::vector<OGRLineString *> lineStrings;
        // Get the geometry associated with the current feature.
        appendOGRLineStringsFromOGRGeometry(&lineStrings, feature->GetGeometryRef());

        // Create QPolygons from the OGRLineStrings.
        for (OGRLineString *lineString : lineStrings)
        {
            polygons.emplace_back(convertOGRLineStringToQPolygon(lineString));
        }

        OGRFeature::DestroyFeature(feature);
    }

    // Clean up.
    OGRGeometryFactory::destroyGeometry(bboxPolygon);
    GDALClose(gdalDataSet);

    LOG_DEBUG("Geometry from shapefile {} has been loaded", fname);

    return polygons;
}

void GeometryHandling::initProjProjection(std::string projString)
{

    // Set the environment variable PROJ_LIB to the directory containing the proj.db file.
    //    std::string pathToProjDB = std::string(PROJECT_BUILD_DIR) + "/vcpkg_installed/x64-windows/share/proj";
    // #ifdef _WIN32
    //    _putenv(("PROJ_LIB=" + pathToProjDB).c_str());
    // #else
    //    setenv("PROJ_LIB", pathToProjDB.c_str(), 1);
    // #endif
    //    LOG_INFO("PROJ_LIB set to {}", pathToProjDB)

    // Initialize proj-Projection.
    // See https://proj.org/development/migration.html
    destroyProjProjection();

    std::string srcProjString = "+proj=latlong +ellps=WGS84";
    pjSrcDstTransformation = proj_create_crs_to_crs(PJ_DEFAULT_CTX, srcProjString.c_str(), projString.c_str(), NULL);

    if (proj_errno(pjSrcDstTransformation))
    {
        LOG_ERROR("ERROR: cannot initialize proj with definition {} (dst); error is {}.", projString,
                  proj_context_errno_string(PJ_DEFAULT_CTX, proj_errno(pjSrcDstTransformation)));
    }

    pjDstSrcTransformation = proj_create_crs_to_crs(PJ_DEFAULT_CTX, projString.c_str(), srcProjString.c_str(), NULL);

    if (proj_errno(pjDstSrcTransformation)) // Check for non-zero error code
    {
        LOG_ERROR("ERROR: cannot initialize proj with definition {} (dst); error is {}.", projString,
                  proj_context_errno_string(PJ_DEFAULT_CTX, proj_errno(pjDstSrcTransformation)))
    }

    for (int i = 0; i < 2; i++)
    {
        // Check if projections are one of the supported ones for line checking
        // (whether a line string's projection crosses the the domain boundaries)
        // do this for source and destination projections
        std::string curProjString = i ? projString : srcProjString;
        auto &connectionCheckFunc = i ? dstConnectionFunc : srcConnectionFunc;
        auto &useScaleFactorForProjection = i ? dstUseScaleFactorForProjection : srcUseScaleFactorForProjection;

        if (curProjString.find("+proj=lcc") != std::string::npos)
        {
            // Lambert Conformal Conic
            // Need lon_0 value in proj string to deduct boundary
            float lon_0;
            std::regex lon0_exp("lon_0=([^\\s]+)");
            std::smatch matchValue;
            auto isMatch = std::regex_search(curProjString, matchValue, lon0_exp);

            if (isMatch)
            {
                std::string cap = matchValue.str(1);
                lon_0 = std::stof(cap);
            }
            else
            {
                lon_0 = 0.0; // default
            }

            // callback function for checking this projection given a line between two points
            connectionCheckFunc = [this, lon_0](PointF f1, PointF f2) {
                return this->canConnectPointPairInProjectionLcc(f1, f2, lon_0);
            };
            useScaleFactorForProjection = true;
        }
        else if (curProjString.find("+proj=latlong") != std::string::npos ||
                 curProjString.find("+proj=longlat") != std::string::npos)
        {
            connectionCheckFunc = [this](PointF f1, PointF f2) {
                return this->canConnectPointPairInProjectionLatlong(f1, f2);
            };
            useScaleFactorForProjection = false; // is inherently in degrees, don't need scaling
        }
        else if (curProjString.find("+proj=stere") != std::string::npos)
        {
            connectionCheckFunc = [this](PointF f1, PointF f2) {
                return this->canConnectPointPairInProjectionStereographic(f1, f2);
            };
            useScaleFactorForProjection = true;
        }
        else
        {
            useScaleFactorForProjection = true;
            LOG_ERROR("Unsupported projection type provided. Quality of graticules might suffer.")
        }
    }
}

void GeometryHandling::destroyProjProjection()
{
    if (pjSrcDstTransformation)
        proj_destroy(pjSrcDstTransformation);
    if (pjDstSrcTransformation)
        proj_destroy(pjDstSrcTransformation);
}

PointF GeometryHandling::geographicalToProjectedCoordinates(PointF point, bool inverse)
{
    // For tests:
    // https://mygeodata.cloud/cs2cs/

    if (!pjSrcDstTransformation || !pjDstSrcTransformation)
    {
        LOG_ERROR("ERROR: proj library not initialized, cannot project geographical coordinates.")
        return {};
    }

    int errorCode;
    double lon_x = 0.;
    double lat_y = 0.;
    PJ_COORD c, c_out;

    if (inverse)
    {
        // Inverse projection: projected to geographical coordinates.
        lon_x = point.x;
        lat_y = point.y;
        if (srcUseScaleFactorForProjection)
        {
            lon_x *= MetConstants::scaleFactorToFitProjectedCoordsTo360Range;
            lat_y *= MetConstants::scaleFactorToFitProjectedCoordsTo360Range;
        }
        // See https://proj.org/development/migration.html
        c.lpzt.z = 0.0;
        c.lpzt.t = HUGE_VAL;
        c.lpzt.lam = lon_x;
        c.lpzt.phi = lat_y;
        c_out = proj_trans(pjDstSrcTransformation, PJ_FWD, c);
        errorCode = proj_errno(pjDstSrcTransformation);
        lon_x = c_out.xy.x;
        lat_y = c_out.xy.y;
    }
    else
    {
        // Geographical to projected coordinates.
        lon_x = point.x;
        lat_y = point.y;
        // clamp to -180..180 / -90..90
        lon_x = clamp(lon_x, -180.0, 180.0);
        lat_y = clamp(lat_y, -90.0, 90.0);

        c.lpzt.z = 0.0;
        c.lpzt.t = HUGE_VAL;
        c.lpzt.lam = lon_x;
        c.lpzt.phi = lat_y;
        c_out = proj_trans(pjSrcDstTransformation, PJ_FWD, c);
        errorCode = proj_errno(pjSrcDstTransformation);
        lon_x = c_out.xy.x;
        lat_y = c_out.xy.y;
        if (dstUseScaleFactorForProjection)
        {
            lon_x /= MetConstants::scaleFactorToFitProjectedCoordsTo360Range;
            lat_y /= MetConstants::scaleFactorToFitProjectedCoordsTo360Range;
        }
    }

    if (errorCode)
    {
        LOG_ERROR("ERROR: proj transformation of point ({}, {}) failed with error '{}'. Returning (NaN., NaN.).",
                  point.x, point.y, proj_context_errno_string(PJ_DEFAULT_CTX, errorCode))
        if (inverse) // Reset error in object
        {
            proj_errno_reset(pjDstSrcTransformation);
        }
        else
        {
            proj_errno_reset(pjSrcDstTransformation);
        }
        return {NAN, NAN};
    }

    return {static_cast<float>(lon_x), static_cast<float>(lat_y)};
}

bool GeometryHandling::canConnectPointPairInProjectionLcc(PointF p1, PointF p2, float lon_0)
{
    // Meridian is on opposite side
    float meridian_lon = fmod(lon_0 + 180.0, 360.0);
    meridian_lon = meridian_lon < 0 ? meridian_lon + 360.0 : meridian_lon;

    // make sure we are all in the same range
    float p1_lon = (p1.x < 0 && meridian_lon > 90.0) ? p1.x + 360.0 : p1.x;
    float p2_lon = (p2.x < 0 && meridian_lon > 90.0) ? p2.x + 360.0 : p2.x;

    if (p1_lon == p2_lon)
        return true;

    // invalid if meridian is between both points
    if (p1_lon <= meridian_lon && meridian_lon <= p2_lon && p2_lon - p1_lon < 180.0)
        return false;
    if (p1_lon >= meridian_lon && meridian_lon >= p2_lon && p1_lon - p2_lon < 180.0)
        return false;

    return true;
}

bool GeometryHandling::canConnectPointPairInProjectionLatlong(PointF p1, PointF p2)
{
    // Don't need to consider bounding box, this is done separately.
    // So only check for antimeridian crossing.
    // Since this projection only goes from -180..180, just check the lon difference.

    return (abs(p1.x - p2.x) < 180.0);
}

bool GeometryHandling::canConnectPointPairInProjectionStereographic(PointF p1, PointF p2)
{
    // Just a singularity at the opposing pole, and it is circular.
    return true;
}

std::vector<std::vector<PointF>> GeometryHandling::geographicalToProjectedCoordinates(
    const std::vector<std::vector<PointF>> &polygons, bool inverse)
{
    std::vector<std::vector<PointF>> projectedPolygons;

    auto lineStringConnectionValidFunc = inverse ? srcConnectionFunc : dstConnectionFunc;

    // for each polygon, for each pair of points check if they should be connected
    for (const std::vector<PointF> &polygon : polygons)
    {
        std::vector<PointF> projectedPolygon;

        PointF lastLLVertex(NAN, NAN); // remember last latlon vertex
        for (PointF vertex : polygon)
        {
            PointF currentVertex = geographicalToProjectedCoordinates(vertex, inverse);

            if (isnan(lastLLVertex.x)) // first vertex of input polygon, keep
            {
                projectedPolygon.emplace_back(currentVertex);
                lastLLVertex = vertex;
            }
            else if (!lineStringConnectionValidFunc || lineStringConnectionValidFunc(lastLLVertex, vertex))
            {
                // last line segment does not interfere with projection bounds
                projectedPolygon.emplace_back(currentVertex);
                lastLLVertex = vertex;
            }
            else
            {
                // interferes, split polygon at this point and create new one
                projectedPolygons.emplace_back(projectedPolygon);
                projectedPolygon = std::vector<PointF>();
                projectedPolygon.emplace_back(currentVertex);
                lastLLVertex = vertex;
            }
        }
        projectedPolygons.emplace_back(projectedPolygon);
    }

    return projectedPolygons;
}

void GeometryHandling::initRotatedLonLatProjection(PointF rotatedPoleLonLat)
{
    rotatedPole = rotatedPoleLonLat;
}

static const double DEG2RAD = M_PI / 180.0;
static const double RAD2DEG = 180.0 / M_PI;

// Parts of the following method have been ported from the C implementation of
// the methods 'lam_to_lamrot' and 'phi_to_phirot'. The original code has been
// published under GNU GENERAL PUBLIC LICENSE Version 2, June 1991.
// source: https://code.zmaw.de/projects/cdo/files  [Version 1.8.1]

// Original code:

// static
// double lam_to_lamrot(double phi, double rla, double polphi, double pollam)
// {
//   /*
//     Umrechnung von rla (geo. System) auf rlas (rot. System)

//     phi    : Breite im geographischen System (N>0)
//     rla    : Laenge im geographischen System (E>0)
//     polphi : Geographische Breite des Nordpols des rot. Systems
//     pollam : Geographische Laenge des Nordpols des rot. Systems

//     result : Rotierte Laenge
//   */
//   double zsinpol = sin(DEG2RAD*polphi);
//   double zcospol = cos(DEG2RAD*polphi);
//   double zlampol =     DEG2RAD*pollam;

//   if ( rla > 180.0 ) rla -= 360.0;

//   double zrla = DEG2RAD*rla;
//   double zphi = DEG2RAD*phi;

//   double zarg1  = - sin(zrla-zlampol)*cos(zphi);
//   double zarg2  = - zsinpol*cos(zphi)*cos(zrla-zlampol)+zcospol*sin(zphi);

//   if ( fabs(zarg2) < 1.0e-20 ) zarg2 = 1.0e-20;

//   return RAD2DEG*atan2(zarg1,zarg2);
// }

// static
// double phi_to_phirot(double phi, double rla, double polphi, double pollam)
// {
//   /*
//     Umrechnung von phi (geo. System) auf phis (rot. System)

//     phi    : Breite im geographischen System (N>0)
//     rla    : Laenge im geographischen System (E>0)
//     polphi : Geographische Breite des Nordpols des rot. Systems
//     pollam : Geographische Laenge des Nordpols des rot. Systems

//     result : Rotierte Breite
//   */
//   double zsinpol = sin(DEG2RAD*polphi);
//   double zcospol = cos(DEG2RAD*polphi);
//   double zlampol =     DEG2RAD*pollam;

//   double zphi = DEG2RAD*phi;
//   if ( rla > 180.0 ) rla -= 360.0;
//   double zrla = DEG2RAD*rla;

//   double zarg = zcospol*cos(zphi)*cos(zrla-zlampol) + zsinpol*sin(zphi);

//   return RAD2DEG*asin(zarg);
// }

PointF GeometryHandling::geographicalToRotatedCoordinates(PointF point)
{
    // Early break for rotation values with no effect.
    double poleLon = rotatedPole.x;
    double poleLat = rotatedPole.y;
    if ((poleLon == -180. || poleLon == 180.) && poleLat == 90.)
    {
        return {};
    }

    // Get longitude and latitude from point.
    double lon = point.x;
    double lat = point.y;

    if (lon > 180.0)
    {
        lon -= 360.0;
    }

    // Convert degrees to radians.
    double poleLatRad = DEG2RAD * poleLat;
    double poleLonRad = DEG2RAD * poleLon;
    double lonRad = DEG2RAD * lon;
    double latRad = DEG2RAD * lat;

    // Compute sinus and cosinus of some coordinates since they are needed more
    // often later on.
    double sinPoleLat = sin(poleLatRad);
    double cosPoleLat = cos(poleLatRad);

    // Apply the transformation (conversation to Cartesian coordinates and  two
    // rotations; difference to original code: no use of pollam).

    double x = ((-sinPoleLat) * cos(latRad) * cos(lonRad - poleLonRad)) + (cosPoleLat * sin(latRad));
    double y = (-sin(lonRad - poleLonRad)) * cos(latRad);
    double z = (cosPoleLat * cos(latRad) * cos(lonRad - poleLonRad)) + (sinPoleLat * sin(latRad));

    // Avoid invalid values for z (Might occure due to inaccuracies in
    // computations).
    z = max(-1., min(1., z));

    // Too small values can lead to numerical problems in method atans2.
    if (std::abs(x) < 1.0e-20)
    {
        x = 1.0e-20;
    }

    // Compute spherical coordinates from Cartesian coordinates and convert
    // radians to degrees.
    return {static_cast<float>(RAD2DEG * (atan2(y, x))), static_cast<float>(RAD2DEG * (asin(z)))};
}

std::vector<std::vector<PointF>> GeometryHandling::geographicalToRotatedCoordinates(
    const std::vector<std::vector<PointF>> &polygons)
{
    std::vector<std::vector<PointF>> projectedPolygons;

    for (const std::vector<PointF> &polygon : polygons)
    {
        std::vector<PointF> projectedPolygon;
        for (PointF vertex : polygon)
        {
            projectedPolygon.emplace_back(geographicalToRotatedCoordinates(vertex));
        }
        projectedPolygons.emplace_back(projectedPolygon);
    }

    return projectedPolygons;
}

// Parts of the following method have been ported from the C implementation of
// the methods 'lamrot_to_lam' and 'phirot_to_phi'. The original code has been
// published under GNU GENERAL PUBLIC LICENSE Version 2, June 1991.
// source: https://code.zmaw.de/projects/cdo/files  [Version 1.8.1]
// Necessary code duplicate in basemap.fx.glsl .

// Original code:

// double lamrot_to_lam(double phirot, double lamrot, double polphi, double pollam, double polgam)
// {
//   /*
//     This function converts lambda from one rotated system to lambda in another system.
//     If the optional argument polgam is present, the other system can also be a rotated one,
//     where polgam is the angle between the two north poles.
//     If polgam is not present, the other system is the real geographical system.

//     phirot : latitude in the rotated system
//     lamrot : longitude in the rotated system (E>0)
//     polphi : latitude of the rotated north pole
//     pollam : longitude of the rotated north pole

//     result : longitude in the geographical system
//   */
//   double zarg1, zarg2;
//   double zgam;
//   double result = 0;

//   double zsinpol = sin(DEG2RAD*polphi);
//   double zcospol = cos(DEG2RAD*polphi);

//   double zlampol = DEG2RAD*pollam;
//   double zphirot = DEG2RAD*phirot;
//   if ( lamrot > 180.0 ) lamrot -= 360.0;
//   double zlamrot = DEG2RAD*lamrot;

//   if ( fabs(polgam) > 0 )
//     {
//       zgam  = -DEG2RAD*polgam;
//       zarg1 = sin(zlampol) *
//  	     (- zsinpol*cos(zphirot) * (cos(zlamrot)*cos(zgam) - sin(zlamrot)*sin(zgam))
//  	      + zcospol*sin(zphirot))
// 	 - cos(zlampol)*cos(zphirot) * (sin(zlamrot)*cos(zgam) + cos(zlamrot)*sin(zgam));

//       zarg2 = cos(zlampol) *
//  	     (- zsinpol*cos(zphirot) * (cos(zlamrot)*cos(zgam) - sin(zlamrot)*sin(zgam))
//	      + zcospol*sin(zphirot))
//	 + sin(zlampol)*cos(zphirot) * (sin(zlamrot)*cos(zgam) + cos(zlamrot)*sin(zgam));
//      }
//   else
//     {
//       zarg1 = sin(zlampol)*(- zsinpol*cos(zlamrot)*cos(zphirot)  +
//      		               zcospol*             sin(zphirot)) -
//	       cos(zlampol)*           sin(zlamrot)*cos(zphirot);
//       zarg2 = cos(zlampol)*(- zsinpol*cos(zlamrot)*cos(zphirot)  +
//                               zcospol*             sin(zphirot)) +
//               sin(zlampol)*           sin(zlamrot)*cos(zphirot);
//     }

//   if ( fabs(zarg2) > 0 ) result = RAD2DEG*atan2(zarg1, zarg2);
//   if ( fabs(result) < 9.e-14 ) result = 0;

//   return result;
// }

// double phirot_to_phi(double phirot, double lamrot, double polphi, double polgam)
// {
//   /*
//     This function converts phi from one rotated system to phi in another
//     system. If the optional argument polgam is present, the other system
//     can also be a rotated one, where polgam is the angle between the two
//     north poles.
//     If polgam is not present, the other system is the real geographical
//     system.

//     phirot : latitude in the rotated system
//     lamrot : longitude in the rotated system (E>0)
//     polphi : latitude of the rotated north pole
//     polgam : angle between the north poles of the systems

//     result : latitude in the geographical system
//   */
//   double zarg;
//   double zgam;

//   double zsinpol = sin(DEG2RAD*polphi);
//   double zcospol = cos(DEG2RAD*polphi);

//   double zphirot   = DEG2RAD*phirot;
//   if ( lamrot > 180.0 ) lamrot -= 360.0;
//   double zlamrot   = DEG2RAD*lamrot;

//   if ( fabs(polgam) > 0 )
//     {
//       zgam = -DEG2RAD*polgam;
//       zarg = zsinpol*sin(zphirot) +
//              zcospol*cos(zphirot)*(cos(zlamrot)*cos(zgam) - sin(zgam)*sin(zlamrot));
//     }
//   else
//     zarg   = zcospol*cos(zphirot)*cos(zlamrot) + zsinpol*sin(zphirot);

//   return RAD2DEG*asin(zarg);
// }

PointF GeometryHandling::rotatedToGeographicalCoordinates(PointF point)
{
    // Early break for rotation values with no effect.
    double poleLon = rotatedPole.x;
    double poleLat = rotatedPole.y;
    if ((poleLon == -180. || poleLon == 180.) && poleLat == 90.)
    {
        return {};
    }

    double resultLon = 0.;

    // Get longitude and latitude from point.
    double rotLon = point.x;
    double rotLat = point.y;

    if (rotLon > 180.0)
    {
        rotLon -= 360.0;
    }

    // Convert degrees to radians.
    double poleLatRad = DEG2RAD * poleLat;
    double poleLonRad = DEG2RAD * poleLon;
    double rotLonRad = DEG2RAD * rotLon;

    // Compute sinus and cosinus of some coordinates since they are needed more
    // often later on.
    double sinPoleLat = sin(poleLatRad);
    double cosPoleLat = cos(poleLatRad);
    double sinRotLatRad = sin(DEG2RAD * rotLat);
    double cosRotLatRad = cos(DEG2RAD * rotLat);
    double cosRotLonRad = cos(DEG2RAD * rotLon);

    // Apply the transformation (conversation to Cartesian coordinates and  two
    // rotations; difference to original code: no use of polgam).

    double x = (cos(poleLonRad) * (((-sinPoleLat) * cosRotLonRad * cosRotLatRad) + (cosPoleLat * sinRotLatRad))) +
               (sin(poleLonRad) * sin(rotLonRad) * cosRotLatRad);
    double y = (sin(poleLonRad) * (((-sinPoleLat) * cosRotLonRad * cosRotLatRad) + (cosPoleLat * sinRotLatRad))) -
               (cos(poleLonRad) * sin(rotLonRad) * cosRotLatRad);
    double z = cosPoleLat * cosRotLatRad * cosRotLonRad + sinPoleLat * sinRotLatRad;

    // Avoid invalid values for z (Might occure due to inaccuracies in
    // computations).
    z = max(-1., min(1., z));

    // Compute spherical coordinates from Cartesian coordinates and convert
    // radians to degrees.

    if (std::abs(x) > 0)
    {
        resultLon = RAD2DEG * atan2(y, x);
    }
    if (std::abs(resultLon) < 9.e-14)
    {
        resultLon = 0.;
    }

    return {static_cast<float>(resultLon), static_cast<float>(RAD2DEG * (asin(z)))};
}

std::vector<std::vector<PointF>> GeometryHandling::splitLineSegmentsLongerThanThreshold(
    const std::vector<std::vector<PointF>> &polygons, double thresholdDistance)
{
    std::vector<std::vector<PointF>> revisedPolygons;

    for (const std::vector<PointF> &polygon : polygons)
    {
        std::vector<PointF> revisedPolygon;
        for (PointF vertex : polygon)
        {
            // The revised polygon is empty? Add this vertex.
            if (revisedPolygon.empty())
            {
                revisedPolygon.emplace_back(vertex);
            }
            // Otherwise, check if the distance between this vertex and the
            // previous one is smaller than the threshold. If yes, add this
            // vertex.
            else if (Vector2D(revisedPolygon.back() - vertex).length() < thresholdDistance)
            {
                revisedPolygon.emplace_back(vertex);
            }
            // If the distance is larger than the threshold split into two
            // polygons.
            else
            {
                revisedPolygons.emplace_back(revisedPolygon);
                revisedPolygon = std::vector<PointF>();
                revisedPolygon.emplace_back(vertex);
            }
        }
        revisedPolygons.emplace_back(revisedPolygon);
    }

    return revisedPolygons;
}

std::vector<std::vector<PointF>> GeometryHandling::enlargeGeometryToBBoxIfNecessary(
    std::vector<std::vector<PointF>> polygons, RectF bbox)
{
    // The geometry in "polygons" is assumed to be located in the range
    // -180..180 degrees.
    // First, determine how many times the geometry needs to be repeated to
    // fill the provdied bounding box "bbox".
    // E.g., if the western edge of the bbox is at -270. degrees, repeat
    // one time to the west; if the eastern edge is 600. degrees, repeat twice
    // to the east.
    int globeRepetitionsWestward = 0;
    if (bbox.left < -180.)
    {
        globeRepetitionsWestward = int(floor((bbox.left + 180.) / 360.));
        LOG_DEBUG("Western edge of BBOX is west of -180. degrees, copying global geometry {} times westward.",
                  globeRepetitionsWestward)
    }

    int globeRepetitionsEastward = 0;
    if (bbox.right > 180.)
    {
        globeRepetitionsEastward = int(ceil((bbox.right - 180.) / 360.));
        LOG_DEBUG("Eastern edge of BBOX is east of 180. degrees, copying global geometry {} times eastward.",
                  globeRepetitionsEastward)
    }

    if (globeRepetitionsWestward == 0 && globeRepetitionsEastward == 0)
    {
        // Bounding box is in range -180..180 degrees, polygons can remain
        // as they are.
        return polygons;
    }

    std::vector<std::vector<PointF>> enlargedPolygonGeometry;

    for (int globeOffset = globeRepetitionsWestward; globeOffset <= globeRepetitionsEastward; globeOffset++)
    {
        double lonOffset = globeOffset * 360.;
        for (std::vector<PointF> &polygon : polygons)
        {
            if (globeOffset == 0)
            {
                enlargedPolygonGeometry.emplace_back(polygon);
            }
            else
            {
                translate(polygon, lonOffset, 0.);
                enlargedPolygonGeometry.emplace_back(polygon);
            }
        }
    }

    return enlargedPolygonGeometry;
}

std::vector<std::vector<PointF>> GeometryHandling::clipPolygons(const std::vector<std::vector<PointF>> &polygons,
                                                                RectF bbox)
{
    std::vector<std::vector<PointF>> clippedPolygons;

    for (const std::vector<PointF> &polygon : polygons)
    {
        std::vector<PointF> clippedPolygon;

        // Loop over each line segment in current polygon.
        for (int i = 0; i < polygon.size() - 1; i++)
        {
            // Obtain the two points that make up the segment.
            PointF p1 = polygon.at(i);
            PointF p2 = polygon.at(i + 1);

            // Clip segment against bbox. If (at least a part of the) segment
            // is maintained, add to the clipped polygon.
            if (cohenSutherlandClip(&p1, &p2, bbox))
            {
                if (clippedPolygon.empty())
                {
                    // If this is the first segment in the clipped polygon,
                    // add both points, unless they're equal.
                    if (p1 == p2)
                    {
                        clippedPolygon.emplace_back(p2);
                    }
                    else
                    {
                        clippedPolygon.emplace_back(p1);
                        clippedPolygon.emplace_back(p2);
                    }
                }
                else if (p1 == clippedPolygon.back())
                {
                    // Last point in clippedPolygon equal to first point in
                    // current line segment? If yes append only last point
                    // current segment. If not, start a new polygon.
                    if (p2 != clippedPolygon.back())
                    {
                        clippedPolygon.emplace_back(p2);
                    }
                }
                else
                {
                    clippedPolygons.emplace_back(clippedPolygon);
                    clippedPolygon = std::vector<PointF>();
                    clippedPolygon.emplace_back(p1);
                    clippedPolygon.emplace_back(p2);
                }
            }
        }

        if (!clippedPolygon.empty())
        {
            clippedPolygons.emplace_back(clippedPolygon);
        }
    }

    return clippedPolygons;
}

void GeometryHandling::flattenPolygonsToVertexList(const std::vector<std::vector<PointF>> &polygons,
                                                   std::vector<float> *vertexList,
                                                   std::vector<int> *polygonStartIndices,
                                                   std::vector<int> *polygonVertexCounts)
{
    // Clear the output vectors.
    vertexList->clear();
    polygonStartIndices->clear();
    polygonVertexCounts->clear();

    // Loop over all polygons.
    for (const std::vector<PointF> &polygon : polygons)
    {
        // Store the start index of the current polygon.
        polygonStartIndices->emplace_back(vertexList->size() / 2);

        // Loop over all points in the current polygon.
        for (const PointF &point : polygon)
        {
            // Add the x and y components of the point to the vertex list.
            vertexList->emplace_back(point.x);
            vertexList->emplace_back(point.y);
        }

        // Store the vertex count of the current polygon.
        polygonVertexCounts->emplace_back(polygon.size());
    }
}

/******************************************************************************
***                           PRIVATE METHODS                               ***
*******************************************************************************/

// The following implementation of the Cohen-Sutherland Clipping algorithm
// is based on code from this website:
// https://www.geeksforgeeks.org/line-clipping-set-1-cohen-sutherland-algorithm/

// Region codes for Cohen-Sutherland clipping.
const int INSIDE = 0; // 0000
const int LEFT = 1;   // 0001
const int RIGHT = 2;  // 0010
const int BOTTOM = 4; // 0100
const int TOP = 8;    // 1000

int GeometryHandling::cohenSutherlandCode(PointF &point, RectF &bbox) const
{
    // Initialize as being inside.
    int code = INSIDE;

    if (point.x < bbox.left) // to the left of rectangle
    {
        code |= LEFT;
    }
    else if (point.x > bbox.right) // to the right of rectangle
    {
        code |= RIGHT;
    }
    if (point.y < bbox.top) // below the rectangle
    {
        code |= BOTTOM;
    }
    else if (point.y > bbox.bottom) // above the rectangle
    {
        code |= TOP;
    }

    return code;
}

bool GeometryHandling::cohenSutherlandClip(PointF *p1, PointF *p2, RectF &bbox)
{
    // Compute region codes for P1, P2
    int code1 = cohenSutherlandCode(*p1, bbox);
    int code2 = cohenSutherlandCode(*p2, bbox);

    // Initialize line as outside the rectangular window
    bool accept = false;

    while (true)
    {
        if ((code1 == 0) && (code2 == 0))
        {
            // If both endpoints lie within rectangle
            accept = true;
            break;
        }
        else if (code1 & code2)
        {
            // If both endpoints are outside rectangle,
            // in same region
            break;
        }
        else
        {
            // Some segment of line lies within the
            // rectangle
            int codeOut;
            double x, y;

            // At least one endpoint is outside the
            // rectangle, pick it.
            if (code1 != 0)
            {
                codeOut = code1;
            }
            else
            {
                codeOut = code2;
            }

            // Find intersection point;
            // using formulas y = y1 + slope * (x - x1),
            // x = x1 + (1 / slope) * (y - y1)
            if (codeOut & TOP)
            {
                // point is above the clip rectangle
                // NOTE: RectF has reversed y-axis, hence top here is
                // bbox.bottom()..
                x = p1->x + (p2->x - p1->x) * (bbox.bottom - p1->y) / (p2->y - p1->y);
                y = bbox.bottom;
            }
            else if (codeOut & BOTTOM)
            {
                // point is below the rectangle
                // NOTE: RectF has reversed y-axis, hence bottom here is
                // bbox.top()..
                x = p1->x + (p2->x - p1->x) * (bbox.top - p1->y) / (p2->y - p1->y);
                y = bbox.top;
            }
            else if (codeOut & RIGHT)
            {
                // point is to the right of rectangle
                y = p1->y + (p2->y - p1->y) * (bbox.right - p1->x) / (p2->x - p1->x);
                x = bbox.right;
            }
            else if (codeOut & LEFT)
            {
                // point is to the left of rectangle
                y = p1->y + (p2->y - p1->y) * (bbox.left - p1->x) / (p2->x - p1->x);
                x = bbox.left;
            }

            // Now intersection point x, y is found
            // We replace point outside rectangle
            // by intersection point
            if (codeOut == code1)
            {
                p1->x = x; // x1 = x;
                p1->y = y; // y1 = y;
                code1 = cohenSutherlandCode(*p1, bbox);
            }
            else
            {
                p2->x = x; // x2 = x;
                p2->y = y; // y2 = y;
                code2 = cohenSutherlandCode(*p2, bbox);
            }
        }
    }

    return accept;
}

OGRPolygon *GeometryHandling::convertQRectToOGRPolygon(RectF &rect)
{
    float leftlon = rect.left;
    float lowerlat = rect.top;
    float rightlon = rect.right;
    float upperlat = rect.bottom;

    OGRLinearRing bboxRing;
    bboxRing.addPoint(leftlon, lowerlat);
    bboxRing.addPoint(rightlon, lowerlat);
    bboxRing.addPoint(rightlon, upperlat);
    bboxRing.addPoint(leftlon, upperlat);
    bboxRing.addPoint(leftlon, lowerlat);
    // OGRPolygon *bboxPolygon = new OGRPolygon(); causes problems on windows
    auto *bboxPolygon = dynamic_cast<OGRPolygon *>(OGRGeometryFactory::createGeometry(OGRwkbGeometryType::wkbPolygon));
    bboxPolygon->addRing(&bboxRing);
    return bboxPolygon;
}

std::vector<PointF> GeometryHandling::convertOGRLineStringToQPolygon(OGRLineString *lineString)
{
    int numLinePoints = lineString->getNumPoints();
    auto *v = new OGRRawPoint[numLinePoints];
    lineString->getPoints(v);
    std::vector<PointF> polygon;
    for (int i = 0; i < numLinePoints; i++)
    {
        polygon.emplace_back(PointF(v[i].x, v[i].y)); // Use push_back() to add PointF to the vector
    }
    delete[] v;
    return polygon;
}

void GeometryHandling::appendOGRLineStringsFromOGRGeometry(std::vector<OGRLineString *> *lineStrings,
                                                           OGRGeometry *geometry)
{
    if (geometry != NULL)
    {
        OGRwkbGeometryType gType = wkbFlatten(geometry->getGeometryType());

        if (gType == wkbLineString)
        {
            lineStrings->emplace_back((OGRLineString *)geometry);
        }

        else if (gType == wkbMultiLineString)
        {
            OGRGeometryCollection *gc = (OGRGeometryCollection *)geometry;
            for (int g = 0; g < gc->getNumGeometries(); g++)
            {
                lineStrings->emplace_back((OGRLineString *)gc->getGeometryRef(g));
            }
        }
    }
}

} // namespace Met3D