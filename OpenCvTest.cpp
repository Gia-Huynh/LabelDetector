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
cv::Mat generateImage(int id = 0, int markerSize = 200, int markerBorderSize = 1)
{
    cv::Mat markerImage;
    cv::aruco::Dictionary dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
    cv::aruco::generateImageMarker(dictionary, id, markerSize, markerImage, markerBorderSize);
    return markerImage;
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

int generateMarker()
{
    std::string folderPath = "L:\\LabelDetector\\StockLabel";
    cv::Mat marker;
    for (int i = 0; i < 10; i++)
    {
        marker = generateImage(i, 200, 1);
        cv::imwrite(folderPath + "/" + std::to_string(i) + ".png", marker);
    }
    return 0;
};
cv::VideoCapture webcamPrepare()
{
    cv::VideoCapture cap(0, cv::CAP_DSHOW);
    if (!cap.isOpened()) {
        std::cout << "Error: Could not open webcam" << std::endl;
        return cap;
    }
    int frame_width = 4320;
    int frame_height = 7680;
    auto codec = cv::VideoWriter::fourcc('M', 'J', 'P', 'G');
    int fps = 60;
    cap.set(cv::CAP_PROP_FRAME_WIDTH, frame_width);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, frame_height);
    cap.set(cv::CAP_PROP_FPS, fps);
    cap.set(cv::CAP_PROP_FOURCC, codec);

    fps = cap.get(cv::CAP_PROP_FPS);
    frame_width = cap.get(cv::CAP_PROP_FRAME_WIDTH);
    frame_height = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
    std::cout << "\n" << fps << ": webcamfps\n\n";
    std::cout << "\n" << frame_width << ": frame_width\n\n";
    std::cout << "\n" << frame_height << ": frame_height\n\n";
    return cap;
};
int webcamRead()
{
    cv::VideoCapture cap = webcamPrepare();
    if (!cap.isOpened()) {
        std::cout << "Error: Could not open webcam" << std::endl;
        return -1;
    }
    // Create a window to display the video stream
    cv::namedWindow("Webcam Stream", cv::WINDOW_AUTOSIZE);
    cv::Mat frame;
    while (true) {
        cap >> frame;
        if (frame.empty()) {
            std::cout << "Error: Empty frame" << std::endl;
            break;
        }
        cv::imshow("Webcam Stream", frame);
        if (cv::waitKey(25) == 'q') {
            break;
        }
    }
    cap.release();
    cv::destroyAllWindows();
    return 0;
};

cv::Mat generateBlankImage()
{
    int width = 2000;
    int height = static_cast<int>(width / 1.414); 
    cv::Mat blank_image(height, width, CV_8UC3, cv::Scalar(255, 255, 255));

    std::vector <cv::Mat> markerList;
    for (int i = 0; i < 4; i++)
    {
        cv::Mat rgbMarker;
        cvtColor(generateImage(i, 200, 1), rgbMarker, cv::COLOR_GRAY2RGB);
        markerList.push_back(rgbMarker);
    };

    // Top-left corner
    markerList[0].copyTo(blank_image(cv::Rect(0, 0, markerList[0].cols, markerList[0].rows)));
    // Top-right corner
    markerList[1].copyTo(blank_image(cv::Rect(width - markerList[0].cols, 0, markerList[0].cols, markerList[0].rows)));
    // Bottom-left corner
    markerList[2].copyTo(blank_image(cv::Rect(0, height - markerList[0].rows, markerList[0].cols, markerList[0].rows)));
    // Bottom-right corner
    markerList[3].copyTo(blank_image(cv::Rect(width - markerList[0].cols, height - markerList[0].rows, markerList[0].cols, markerList[0].rows)));
    return blank_image;
    cv::imwrite("output_image_with_markers.png", blank_image);
};

int main()
{
    std::vector<int> markerIds;
    std::vector<std::vector<cv::Point2f>> markerCorners, rejectedCandidates;
    cv::aruco::DetectorParameters detectorParams = cv::aruco::DetectorParameters();
    cv::aruco::Dictionary dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
    cv::aruco::ArucoDetector detector(dictionary, detectorParams);

    cv::VideoCapture cap = webcamPrepare();
    if (!cap.isOpened()) {
        std::cout << "Error: Could not open webcam" << std::endl;
        return -1;
    }
    // Create a window to display the video stream
    cv::namedWindow("Webcam Stream", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Output Stream", cv::WINDOW_AUTOSIZE);
    cv::Mat frame;
    int outputHeight = 1000; // Adjust as needed
    int outputWidth = static_cast<int>(outputHeight / 1.414); // Keep aspect ratio
    while (true) {
        cap >> frame;
        if (frame.empty()) {std::cout << "Error: Empty frame" << std::endl;break;}

        detector.detectMarkers(frame, markerCorners, markerIds, rejectedCandidates);

        cv::Mat frame_OG = frame.clone();
        cv::aruco::drawDetectedMarkers(frame, markerCorners, markerIds);
        cv::imshow("Webcam Stream", frame);
        //std::cout << markerCorners.size()<<"|_ ";
        cv::Mat SumPerspectiveTransform(cv::Size(3, 3), CV_64FC1);
        SumPerspectiveTransform = 0;
        int count = 0;
        std::vector<cv::Point2f> detectedCorners(4);
        for (int i = 0; i < markerIds.size(); i++)
        {
            if (markerIds[i] == 0)
            {
                detectedCorners[0] = markerCorners[i][0];
                count++;
            };
            if (markerIds[i] == 1)
            {
                detectedCorners[1] = markerCorners[i][1];
                count++;
            };
            if (markerIds[i] == 2)
            {
                detectedCorners[3] = markerCorners[i][3];
                count++;
            };
            if (markerIds[i] == 3)
            {
                detectedCorners[2] = markerCorners[i][2];
                count++;    
            };
        } 
        std::vector<cv::Point2f> originalCorners = {
            cv::Point2f(outputWidth * 1, outputHeight * 0),   // Right-Top
            cv::Point2f(outputWidth * 1, outputHeight * 1),   // Right-Bottom
            cv::Point2f(outputWidth * 0, outputHeight * 1),  // Left-Bottom
            cv::Point2f(outputWidth * 0, outputHeight * 0),        // Left-Top
        };
        if (count == 4)
        {
            cv::Mat outputImage;
            cv::Mat perspectiveTransform = cv::getPerspectiveTransform(detectedCorners, originalCorners);
            cv::warpPerspective(frame_OG, outputImage, perspectiveTransform, cv::Size(outputWidth, outputHeight));
            cv::imshow("Output Stream", outputImage);
        };
        //std::cout << "\n";
        if (cv::waitKey(30) == 'q') {break;}
    }
    cap.release();
    cv::destroyAllWindows();
    return 0;
};