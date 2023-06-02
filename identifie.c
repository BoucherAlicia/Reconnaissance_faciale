//Alicia et Charbel aka le meilleur groupe
//Troisième code
//il detecte un visage par la camera ou une video et affiche le nom de la personne s'il la reconnait ou affiche inconnu sinon :)
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <opencv2/face.hpp>
#include "opencv2/face/facerec.hpp"

int main()
{
    cv::CascadeClassifier faceCascade; //cree une variable de type CascadeClassifier
    faceCascade.load("haarcascade_frontalface_alt2.xml");

    cv::Ptr<cv::face::LBPHFaceRecognizer> recognizer = cv::face::LBPHFaceRecognizer::create();
    recognizer->read("trainner.yml");

    int id_image = 0;
    cv::Scalar color_info(255, 255, 255);
    cv::Scalar color_ko(0, 0, 255); //rouge
    cv::Scalar color_ok(0, 255, 0); //vert

    std::map<int, std::string> labels;
    {
        std::ifstream file("labels.pickle", std::ios::binary);
        if (!file)
        {
            std::cout << "Impossible d'ouvrir le fichier des labels." << std::endl;
            return -1;
        }

        std::map<std::string, int> og_labels;
        std::string line;
        while (std::getline(file, line))
        {
            std::istringstream iss(line);
            std::string key;
            int value;
            if (!(iss >> key >> value))
                break;
            og_labels[key] = value;
        }
        for (const auto& pair : og_labels)
        {
            labels[pair.second] = pair.first;
        }
        file.close();
    }

    cv::VideoCapture cap(0); //ouvre camera ou "Alicia.MP4" pour video
    if (!cap.isOpened())
    {
        std::cout << "Impossible d'ouvrir le fichier vidéo." << std::endl;
        return -1;
    }

    cv::Mat frame; 
    while (true)
    {
        bool ret = cap.read(frame);
        if (!ret)
        {
            std::cout << "Fin de la vidéo atteinte." << std::endl; //si video
            break;
        }

        cv::Mat gray;
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY); //conversion en gris

        std::vector<cv::Rect> faces;
        faceCascade.detectMultiScale(gray, faces, 1.2, 4, 0, cv::Size(30, 30));

        for (const cv::Rect& rect : faces)
        {
            cv::Mat roi_gray = gray(rect);
            int id_; //identifiant de la personne dans labels.pickle
            double conf; //l'indice de confiance
            recognizer->predict(roi_gray, id_, conf);
            cv::Scalar color;
            std::string name;
            if (conf <= 95) //si indice de confiance > 95 --> affiche inconnu
            {
                color = color_ok;
                if (id_ == 1)
                    name = "Alicia";
                else if (id_ == 0)
                    name = "Charbel";
            }
            else
            {
                color = color_ko;
                name = "Inconnu";
            }
            std::string label = name;
            cv::putText(frame, label, cv::Point(rect.x, rect.y - 10), cv::FONT_HERSHEY_DUPLEX, 1, color_info, 1, cv::LINE_AA); //ecrire le nom de la personne
            cv::rectangle(frame, rect, color, 2);
        }

        cv::imshow("L42Project", frame);

        int key = cv::waitKey(1) & 0xFF;
        if (key == 'q') //pour fermer la video
        {
            break;
        }
        if (key == 'a') //pour avance rapide
        {
            for (int cpt = 0; cpt < 100; cpt++)
            {
                ret = cap.read(frame);
                if (!ret)
                {
                    std::cout << "Fin de la vidéo atteinte." << std::endl;
                    break;
                }
            }
        }
    }

    cap.release();
    cv::destroyAllWindows();

    std::cout << "Fin" << std::endl;

    return 0;
}

