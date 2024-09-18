// OpenCvTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <opencv2/opencv.hpp>
#include <opencv2/objdetect/aruco_detector.hpp>
#include <opencv2/imgcodecs.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>  // C++17 standard
namespace fs = std::filesystem;
void generateImage(std::string imgName = "marker23.png")
{
    cv::Mat markerImage;
    cv::aruco::Dictionary dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
    cv::aruco::generateImageMarker(dictionary, 23, 200, markerImage, 1);
    cv::imwrite(imgName, markerImage);
};

cv::Mat detectOneImage(cv::Mat inputImage)
{
    std::vector<int> markerIds;
    std::vector<std::vector<cv::Point2f>> markerCorners, rejectedCandidates;
    cv::aruco::DetectorParameters detectorParams = cv::aruco::DetectorParameters();
    cv::aruco::Dictionary dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
    cv::aruco::ArucoDetector detector(dictionary, detectorParams);
    detector.detectMarkers(inputImage, markerCorners, markerIds, rejectedCandidates);
    cv::Mat outputImage = inputImage.clone();
    cv::aruco::drawDetectedMarkers(outputImage, markerCorners, markerIds);
    cv::imshow("test", outputImage);
    cv::waitKey(0);
    return outputImage;
};

int detectFolder()
{
    //cv::Mat inputImage = cv::imread("L:\\Picture1.jpg");

    std::string folderPath = "L:\\LabelDetector";
    std::string destinationFolder = folderPath + "\\Result";
    if (!fs::exists(destinationFolder)) {
        fs::create_directory(destinationFolder);
    }

    for (const auto& entry : fs::directory_iterator(folderPath)) {
        std::string filePath = entry.path().string();
        std::string fileName = entry.path().filename().string();
        std::string saveFilePath = destinationFolder + "/" + fileName;
        cv::Mat image = cv::imread(filePath);
        // Check if the image was loaded successfully
        if (image.empty()) {
            std::cout << "Could not open or find the image: " << filePath << std::endl;
            continue;
        }
        cv::Mat result = detectOneImage(image);
        // Save the image to the destination folder
        bool isSaved = cv::imwrite(saveFilePath, result);
    }
    return 0;
}

int main()
{
    generateImage
    //detectFolder();
    return 0;
};