#include "imagecom.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <algorithm>
#include <vector>

namespace ImageCompressor {

// --- SINGLE IMAGE COMPRESSION (JPEG Lossy) ---
// Default quality is set to 75 (assuming you want to keep that change)
bool compressAndSaveSingleImage(const std::string& inputPath, const std::string& outputPath, int quality) {
    cv::Mat image = cv::imread(inputPath, cv::IMREAD_UNCHANGED);

    if (image.empty()) {
        std::cerr << "ERROR [ImageCom]: Could not open or find the image at " << inputPath << std::endl;
        return false;
    }

    std::vector<int> compression_params;
    compression_params.push_back(cv::IMWRITE_JPEG_QUALITY);
    // Ensure quality is within [0, 100]
    quality = std::max(0, std::min(100, quality));
    compression_params.push_back(quality);

    bool success = cv::imwrite(outputPath, image, compression_params);

    if (!success) {
        std::cerr << "ERROR [ImageCom]: Failed to write the compressed image to disk." << std::endl;
    } else {
        std::cout << "SUCCESS [ImageCom]: Single image compressed and saved to " << outputPath << ". Quality: " << quality << "." << std::endl;
    }

    return success;
}


// --- VIDEO COMPRESSION (Optimized XVID Codec) ---
// Note: JPEG quality parameter is ignored for video codec control.
bool compressAndSaveVideo(const std::string& inputPath, const std::string& outputPath, int quality) {
    cv::VideoCapture inputVideo(inputPath);
    if (!inputVideo.isOpened()) {
        std::cerr << "ERROR [VideoCom]: Could not open or find the input video: " << inputPath << std::endl;
        return false;
    }

    // Checking for .avi extension (best container for XVID)
    if (outputPath.find(".avi") == std::string::npos && outputPath.find(".AVI") == std::string::npos) {
        std::cerr << "WARNING [VideoCom]: XVID is used. Output path should end with '.avi' for compatibility." << std::endl;
    }

    int frame_width = (int)inputVideo.get(cv::CAP_PROP_FRAME_WIDTH);
    int frame_height = (int)inputVideo.get(cv::CAP_PROP_FRAME_HEIGHT);
    double fps = inputVideo.get(cv::CAP_PROP_FPS);
    if (fps <= 0) fps = 30.0;

    // Use XVID codec (MPEG-4) for reliable AVI writing
    int fourcc = cv::VideoWriter::fourcc('X', 'V', 'I', 'D');

    cv::VideoWriter outputVideo;
    // Attempt to open the writer
    if (!outputVideo.open(outputPath, fourcc, fps, cv::Size(frame_width, frame_height), true)) {
        std::cerr << "ERROR [VideoCom]: Could not open the output video writer for " << outputPath << std::endl;
        inputVideo.release();
        return false;
    }

    cv::Mat frame;
    long frameCount = 0;

    // Read and Write Loop: Rely entirely on the XVID codec for compression.
    while (inputVideo.read(frame)) {
        frameCount++;
        // NOTE: outputVideo.write() returns void and cannot be checked directly with 'if (!...)'
        outputVideo.write(frame);
    }

    inputVideo.release();
    outputVideo.release();

    if (frameCount > 0) {
        std::cout << "SUCCESS [VideoCom]: Video processed using XVID codec. Total frames: " << frameCount << "." << std::endl;
        return true;
    } else {
        std::cerr << "ERROR [VideoCom]: Video file was empty or could not be fully read." << std::endl;
        return false;
    }
}

} // namespace ImageCompressor
