//
// Created by Lightweh on 2018/11/21.
//

#ifndef FACEDETECTION_FACE_DETECTOR_H
#define FACEDETECTION_FACE_DETECTOR_H


#include <vector>
#include <string>
#include <opencv2/opencv.hpp>
#include <dlib/opencv/cv_image.h>
#include <dlib/image_processing/frontal_face_detector.h>

#include <dlib/dnn.h>
//#include <dlib/gui_widgets.h>
#include <dlib/clustering.h>
#include <dlib/string.h>
#include <dlib/image_io.h>
#include <math.h>

class FaceDetector {
private:

    dlib::frontal_face_detector face_detector;
    std::vector<dlib::rectangle> det_rects;
    cv::Mat frame;

public:

    FaceDetector();

    int Detect(const cv::Mat &image);

    std::vector<dlib::rectangle> getDetResultRects();
    std::string getFacesRecognized(const dlib::rectangle face);
};


#endif //FACEDETECTION_FACE_DETECTOR_H
