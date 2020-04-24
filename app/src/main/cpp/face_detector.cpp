//
// Created by Lightweh on 2018/11/21.
//
#include <iostream>
#include "face_detector.h"

// --------------------- RECOGNITION ---------------------
template <template <int, template<typename>class, int, typename> class block, int N, template<typename>class BN, typename SUBNET>
using residual = dlib::add_prev1<block<N, BN, 1, dlib::tag1<SUBNET>>>;

template <template <int, template<typename>class, int, typename> class block, int N, template<typename>class BN, typename SUBNET>
using residual_down = dlib::add_prev2<dlib::avg_pool<2, 2, 2, 2, dlib::skip1<dlib::tag2<block<N, BN, 2, dlib::tag1<SUBNET>>>>>>;

template <int N, template <typename> class BN, int stride, typename SUBNET>
using block = BN<dlib::con<N, 3, 3, 1, 1, dlib::relu<BN<dlib::con<N, 3, 3, stride, stride, SUBNET>>>>>;

template <int N, typename SUBNET> using ares = dlib::relu<residual<block, N, dlib::affine, SUBNET>>;
template <int N, typename SUBNET> using ares_down = dlib::relu<residual_down<block, N, dlib::affine, SUBNET>>;

template <typename SUBNET> using alevel0 = ares_down<256, SUBNET>;
template <typename SUBNET> using alevel1 = ares<256, ares<256, ares_down<256, SUBNET>>>;
template <typename SUBNET> using alevel2 = ares<128, ares<128, ares_down<128, SUBNET>>>;
template <typename SUBNET> using alevel3 = ares<64, ares<64, ares<64, ares_down<64, SUBNET>>>>;
template <typename SUBNET> using alevel4 = ares<32, ares<32, ares<32, SUBNET>>>;

std::vector<float> convert_matrix_to_vector(dlib::matrix<float, 0, 1> descritores)
{
    std::vector<float> result;
    for (auto d : descritores)
        result.push_back(d);

    return result;
}

std::vector<float> distanciaEuclidiana(std::vector<std::vector<float>> descs, std::vector<float> desc)
{
    std::vector<std::vector<float>> vetaux;
    std::vector<float> res;
    float som;
    for (int i = 0; i < descs.size(); i++)
    {
        som = 0;
        std::vector<float> mataux;
        for (int j = 0; j < 128; j++)
            //mataux.push_back(desc[j] - descs[i][j]);
            som += pow(desc[j] - descs[i][j], 2);
        res.push_back(sqrt(som));

        //vetaux.push_back(mataux);
    }

    /*std::vector<float> res;
    float som;
    for (auto mat : vetaux)
    {
        som = 0;
        for (auto val : mat)
            som += pow(val, 2);

        res.push_back(sqrt(som));
    }*/
    return res;
}

int minarg(std::vector<float> vet)
{
    float min = vet[0];
    int min_index = 0;
    for(int i = 1; i < vet.size(); i++)
        if (vet[i] < min)
        {
            min = vet[i];
            min_index = i;
        }
    return min_index;
}

FaceDetector::FaceDetector() {
    face_detector = dlib::get_frontal_face_detector();
}

int FaceDetector::Detect(const cv::Mat &image) {

    if (image.empty())
        return 0;

    /*if (image.channels() == 1) {
        cv::cvtColor(image, image, CV_GRAY2BGR);
    }*/

    dlib::cv_image<dlib::bgr_pixel> dlib_image(image);

    det_rects.clear();
    frame = image;
    det_rects = face_detector(dlib_image);

    return det_rects.size();
}

std::string FaceDetector::getFacesRecognized(const dlib::rectangle face)
{
    dlib::shape_predictor sp;
    dlib::deserialize("/storage/emulated/0/shape_predictor_5_face_landmarks.dat") >> sp;

    std::vector<std::vector<float>> face_descriptors;
    dlib::deserialize("/storage/emulated/0/descritores") >> face_descriptors;

    dlib::loss_metric<dlib::fc_no_bias<128, dlib::avg_pool_everything<alevel0<alevel1<alevel2<alevel3<alevel4<dlib::max_pool<3, 3, 2, 2, dlib::relu<dlib::affine<dlib::con<32, 7, 7, 2, 2, dlib::input_rgb_image_sized<150>>>>>>>>>>>>> net;
    dlib::deserialize("/storage/emulated/0/dlib_face_recognition_resnet_model_v1.dat") >> net;

    std::vector<std::string> labels;
    dlib::deserialize("/storage/emulated/0/labels") >> labels;

    float limiar = 0.5;

    std::vector<dlib::matrix<dlib::rgb_pixel>> faces;
    dlib::cv_image<dlib::bgr_pixel> img(frame);

    auto shape = sp(img, face);
    dlib::matrix<dlib::rgb_pixel> face_chip;
    dlib::extract_image_chip(img, dlib::get_face_chip_details(shape, 150, 0.25), face_chip);
    faces.push_back(std::move(face_chip));

    //if (faces.size() != 0)
    //{
        //std::vector<dlib::matrix<float, 0, 1>> descritorFacial = net(/*std::move*/(face_chip));
        std::vector<dlib::matrix<float, 0, 1>> descritorFacial = net(faces);
        //for (int j = 0; j < descritorFacial.size(); j++)
        //{
            std::vector<float> distancias = distanciaEuclidiana(face_descriptors, convert_matrix_to_vector(descritorFacial[0]));
            int index = minarg(distancias);

            if (distancias[index] <= limiar)
                return labels[index];
        //}
   // }
    return "";
}

std::vector<dlib::rectangle> FaceDetector::getDetResultRects() {
    return det_rects;
}