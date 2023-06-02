//Alicia et Charbel
//Premier code
//Pour detecter visage et prendre plusieurs captures d'écran pour faire une base de données
#include <opencv2/opencv.hpp>
#include <iostream>

int main()
{
    cv::CascadeClassifier faceCascade;
    faceCascade.load("./haarcascade_frontalface_alt2.xml");

    cv::VideoCapture cap(0); //cap("Alicia.MP4") exemple si video
    if (!cap.isOpened())
    {
        std::cout << "Failed to open the camera." << std::endl;
        return -1;
    }

    int id = 0;
    cv::Mat frame;
    while (true)
    {
        cap >> frame;
        if (frame.empty())
        {
            std::cout << "Failed to capture frame." << std::endl;
            break;
        }

        cv::Mat gray; //cree variable gray type matrice multidim
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY); //conversion color to gris

        std::vector<cv::Rect> faces;
        faceCascade.detectMultiScale(gray, faces, 1.2, 4); //4 : controle la sensibilité
        for (const cv::Rect& rect : faces)
        {
            cv::imwrite("photos/charbel/p-" + std::to_string(id) + ".png", frame(rect)); //enregistre les photos
            cv::rectangle(frame, rect, cv::Scalar(0, 0, 255), 2); //met les rectangles autour du visage
            id++;
        }

        cv::imshow("video", frame); //affichage image avec les visages détectés
        int key = cv::waitKey(1) & 0xFF;
        if (key == 'q')
        {
            break;
        }
    }

    cap.release();
    cv::destroyAllWindows();

    return 0;
}
