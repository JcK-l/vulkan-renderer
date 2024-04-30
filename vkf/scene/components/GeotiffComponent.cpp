/// \file
/// \brief

//
// Created by Joshua Lowe on 1/9/2024.
// The license and distribution terms for this file may be found in the file LICENSE in this distribution
//

#include "GeotiffComponent.h"
#include "../../common/Log.h"
#include "../../core/Buffer.h"
#include "../../core/Image.h"
#include <gdal.h>
#include <gdal_priv.h>
#include <imgui.h>

namespace vkf::scene
{

GeotiffComponent::GeotiffComponent(const vkf::core::Device &device) : device{device}
{
    data.poleLat = 90.0f;
    data.poleLon = 180.0f;
    data.cornersData = glm::vec4{-180.0f, -90.0f, 180.0f, 90.0f};
    data.colorIntensity = 0.5f;

    GDALAllRegister();
};

core::Image GeotiffComponent::createImage()
{
    this->path =
        (this->path.empty()) ? PROJECT_ROOT_DIR + std::string("/assets/HYP_50M_SR_W/HYP_50M_SR_W.tif") : this->path;

    // MET.3D code
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // open the raster dataset and store it in a GDALDataset object
    auto *tiffData = static_cast<GDALDataset *>(GDALOpen(path.c_str(), GA_ReadOnly));

    // GetDescription() <- filename
    // GetRaster Size() <- dimensions lot/lan
    // GetRasterCount() <- number of raster bands (3 for rgb)
    // GetDriver()->GetDescription() <- data type

    if (tiffData == nullptr)
    {
        LOG_ERROR("Cannot open GeoTIFF file {}", path)
    }

    std::string datasetType = tiffData->GetDriver()->GetDescription();

    if (datasetType != "GTiff")
    {
        LOG_ERROR("Raster dataset {} is not of type GeoTiff.", path)
    }

    // get the geo-spatial translation of the current raster dataset
    std::vector<double> geoData(6);
    // 0 - top left x
    // 1 - w-e pixel resolution
    // 2 - 0
    // 3 - top left y
    // 4 - 0
    // 5 - n-s pixel resolution (negative)

    tiffData->GetGeoTransform(&geoData[0]);

    // calculate lat/lon corners of loaded map and store them in class
    const int32_t longitudeDim = tiffData->GetRasterXSize();
    const int32_t latitudeDim = tiffData->GetRasterYSize();
    const int32_t colorDim = tiffData->GetRasterCount() + 1;

    data.cornersData[0] = static_cast<float>(geoData[0]);                               // llcrnrlon
    data.cornersData[1] = static_cast<float>((geoData[3] + geoData[5] * latitudeDim));  // llcrnrlat
    data.cornersData[2] = static_cast<float>((geoData[0] + geoData[1] * longitudeDim)); // urcrnrlon
    data.cornersData[3] = static_cast<float>(geoData[3]);                               // urcrnrlat

    LOG_DEBUG("Longitude range: {} - {}", data.cornersData[0], data.cornersData[2])
    LOG_DEBUG("Latitude range: {} - {}", data.cornersData[1], data.cornersData[3])

    // read the color data by using bands
    // we need three bands for every color channel

    // GDALGetDataTypeName(band->GetRasterDataType()) <- datatype
    // DALGetColorInterpretationName(band->GetColorInterpretation()) <- color channel

    // image data
    const uint32_t imgSizeX = longitudeDim * colorDim;
    const uint32_t imgSize = imgSizeX * latitudeDim;

    std::vector<uint8_t> tiffImg;
    // changed reserve to resize because of "vector subscript out of range"
    // error on windows
    tiffImg.resize(imgSize);
    // GLbyte* tiffImg = new GLbyte[imgSize * sizeof(GLbyte)];

    // print out some information
    LOG_DEBUG("Map texture size (lon/lat/col): {}x{}x{}", longitudeDim, latitudeDim, colorDim)

    // print out some information
    LOG_DEBUG("Parsing color data...")

    enum ColorChannel
    {
        Red,
        Green,
        Blue
    };

    std::unordered_map<ColorChannel, GDALRasterBand *> bandsMap;
    bandsMap[Red] = tiffData->GetRasterBand(1);
    bandsMap[Green] = tiffData->GetRasterBand(2);
    bandsMap[Blue] = tiffData->GetRasterBand(3);

    // fetch the raster data of all color components
    std::unordered_map<ColorChannel, std::vector<uint8_t>> rasterData;
    rasterData[Red] = std::vector<uint8_t>(longitudeDim * latitudeDim);
    rasterData[Green] = std::vector<uint8_t>(longitudeDim * latitudeDim);
    rasterData[Blue] = std::vector<uint8_t>(longitudeDim * latitudeDim);

    std::string dataType = GDALGetDataTypeName(bandsMap[Red]->GetRasterDataType());

    if (dataType != "Byte")
    {
        LOG_ERROR("Raster dataset {} has no data of type Byte.", path)
    }

    // load whole raster data into buffers
    CPLErr returnCode;
    for (auto &band : bandsMap)
    {
        returnCode = band.second->RasterIO(GF_Read, 0, 0, longitudeDim, latitudeDim, &rasterData[band.first][0],
                                           longitudeDim, latitudeDim, GDT_Byte, 0, 0);
        if (returnCode != CE_None)
        {
            LOG_ERROR("issue when reading basemap image.")
        }
    }

    // copy the color components into the image buffer
    for (int i = 0; i < longitudeDim * latitudeDim; ++i)
    {
        const uint32_t tiffID = i * colorDim;

        tiffImg[tiffID + 0] = rasterData[Red][i];
        tiffImg[tiffID + 1] = rasterData[Green][i];
        tiffImg[tiffID + 2] = rasterData[Blue][i];
        tiffImg[tiffID + 3] = 255;
    }

    // at the end close the tiff file
    GDALClose(tiffData);
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    auto texture = core::Image{
        device,
        vk::ImageCreateInfo{.imageType = vk::ImageType::e2D,
                            .format = vk::Format::eR8G8B8A8Srgb,
                            .extent = vk::Extent3D{.width = static_cast<uint32_t>(longitudeDim),
                                                   .height = static_cast<uint32_t>(latitudeDim),
                                                   .depth = 1},
                            .mipLevels = 1,
                            .arrayLayers = 1,
                            .samples = vk::SampleCountFlagBits::e1,
                            .tiling = vk::ImageTiling::eOptimal,
                            .usage = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
                            .sharingMode = vk::SharingMode::eExclusive,
                            .initialLayout = vk::ImageLayout::eUndefined},
        VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT};

    vk::BufferCreateInfo bufferCreateInfo{.size = imgSize, .usage = vk::BufferUsageFlagBits::eTransferSrc};
    core::Buffer stagingBuffer{device, bufferCreateInfo,
                               VMA_ALLOCATION_CREATE_MAPPED_BIT |
                                   VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT};

    stagingBuffer.updateData(tiffImg.data(), imgSize, 0);

    texture.transitionImageLayout(vk::ImageLayout::eTransferDstOptimal);
    texture.copyBufferToImage(stagingBuffer);
    texture.transitionImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal);

    hasNewTexture = false;

    return texture;
}

void GeotiffComponent::updateGui()
{
    // Maybe use tinyfiledialogs to open a file dialog to select a texture file in the future
    ImGui::Text("Texture:");
    ImGui::Spacing();
    std::string filepathLabel = "File Path##" + std::to_string(reinterpret_cast<uintptr_t>(this));
    ImGui::InputText(filepathLabel.c_str(), &path[0], path.size());
    std::string loadLabel = "Load##" + std::to_string(reinterpret_cast<uintptr_t>(this));
    if (ImGui::Button(loadLabel.c_str()))
    {
        // Load texture
        hasNewTexture = true;
    }

    ImGui::SliderFloat("Color Intensity", &data.colorIntensity, 0.0f, 1.0f);

    ImGui::Spacing();
}

} // namespace vkf::scene
