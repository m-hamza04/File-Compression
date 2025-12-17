#ifndef IMAGECOM_H
#define IMAGECOM_H

#include <string>

namespace ImageCompressor {

/**
 * @brief Compresses a single image using JPEG encoding and saves it to disk.
 * @param inputPath Path to the source image file (e.g., .png, .bmp).
 * @param outputPath Path where the compressed image will be saved (must be .jpg).
 * @param quality JPEG compression quality (0-100).
 * @return true if successful, false otherwise.
 */
bool compressAndSaveSingleImage(const std::string& inputPath, const std::string& outputPath, int quality = 75);

/**
 * @brief Loads a video, compresses it frame-by-frame using JPEG encoding, and saves the new video.
 * @param inputPath Path to the source video file (e.g., .mp4, .avi).
 * @param outputPath Path where the compressed video will be saved (e.g., _compressed.mp4).
 * @param quality JPEG compression quality (0-100) applied to each frame.
 * @return true if successful, false otherwise.
 */
bool compressAndSaveVideo(const std::string& inputPath, const std::string& outputPath, int quality = 75);

} // namespace ImageCompressor

#endif // IMAGECOM_H
