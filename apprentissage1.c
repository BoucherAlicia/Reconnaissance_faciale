//Alicia et Charbel
//Deuxieme code
//Pour entrainer le modèle sur la base de données récuperée
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h> //manipulation des répertoires, notamment l'ouverture, la lecture et la fermeture de répertoires, ainsi que la récupération des noms des fichiers contenus dans un répertoire
#include <sys/stat.h> //pour l'accès aux informations sur les fichiers, telles que les permissions, la taille... elle permet de vérifier si un fichier existe, d'obtenir ses attributs et de manipuler les droits d'accès aux fichiers
#include <opencv2/opencv.hpp>
#include <opencv2/face.hpp> //propose des algorithmes de reconnaissance faciale populaires tels que le modèle LBPH (Local Binary Patterns Histograms)

using namespace cv;
using namespace cv::face;
using namespace std;

int main() {
    const char* imageDir = "./photos/";
    const char* labelsFile = "labels.pickle"; //enregistre l'identifiant de la personne
    const char* trainedModelFile = "trainner.yml"; //enregistre les coordonnées du visage

    int currentId = 0;
    map<string, int> labelIds;
    vector<Mat> images;
    vector<int> labels;

    DIR* directory = opendir(imageDir); //ouvre dossier photos
    if (directory == NULL) {
        fprintf(stderr, "Failed to open image directory: %s\n", imageDir);
        return 1;
    }

    struct dirent* file; //parcours tous les sous dossiers dans photos 
    while ((file = readdir(directory)) != NULL) {
        if (file->d_type == DT_DIR && strcmp(file->d_name, ".") != 0 && strcmp(file->d_name, "..") != 0) {
            char subDirPath[256];
            snprintf(subDirPath, sizeof(subDirPath), "%s%s", imageDir, file->d_name);

            DIR* subDirectory = opendir(subDirPath);
            if (subDirectory == NULL) {
                fprintf(stderr, "Failed to open sub-directory: %s\n", subDirPath);
                continue;
            }

            struct dirent* subFile;
            while ((subFile = readdir(subDirectory)) != NULL) {
                if (subFile->d_type == DT_REG && strstr(subFile->d_name, ".png") != NULL) {
                    char imagePath[512];
                    snprintf(imagePath, sizeof(imagePath), "%s/%s", subDirPath, subFile->d_name);

                    string label(file->d_name);
                    if (labelIds.find(label) == labelIds.end()) {
                        labelIds[label] = currentId;
                        currentId++;
                    }

                    int id = labelIds[label];

    
                    Mat image = imread(imagePath, IMREAD_GRAYSCALE); //lecture de l'image
                    if (image.empty()) {
                        fprintf(stderr, "Failed to read image: %s\n", imagePath);
                        continue;
                    }

                   //detecte s'il y a un visage ou non
                    CascadeClassifier cascade("haarcascade_frontalface_alt2.xml");
                    vector<Rect> faces;
                    cascade.detectMultiScale(image, faces);
                    if (faces.empty()) {
                        fprintf(stderr, "Failed to detect face in image: %s\n", imagePath);
                        continue;
                    }

                    //redimensionner l'image
                    Mat croppedFace = image(faces[0]);
                    Mat resizedFace;
                    resize(croppedFace, resizedFace, Size(200, 200));

                    images.push_back(resizedFace);
                    labels.push_back(id);
                }
            }

            closedir(subDirectory);
        }
    }

    closedir(directory);

    // enregistre l'identifiant
    FILE* labelsFilePtr = fopen(labelsFile, "wb");
    if (labelsFilePtr == NULL) {
        fprintf(stderr, "Failed to open labels file: %s\n", labelsFile);
        return 1;
    }

    for (const auto& entry : labelIds) {
        fprintf(labelsFilePtr, "%s:%d\n", entry.first.c_str(), entry.second);
    }

    fclose(labelsFilePtr);

    //transformer l'image en vecteurs
    vector<Mat> imagesVector;
    for (size_t i = 0; i < images.size(); i++) {
        imagesVector.push_back(images[i]);
    }

    //entraine le modèle
    Ptr<LBPHFaceRecognizer> recognizer = LBPHFaceRecognizer::create();
    recognizer->train(imagesVector, labels);

    //enregistre dans trainner.yml
    recognizer->save(trainedModelFile);

    return 0;
}

