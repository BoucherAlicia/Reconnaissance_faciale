# Suivi de Couleur et Reconnaissance Faciale

Notre projet consiste en la création d'une application de suivi de couleur et de reconnaissance faciale utilisant OpenCV, une bibliothèque de vision par ordinateur, ainsi qu'une Caméra Pan-Tilt contrôlée par des servomoteurs Arduino. 

## Fonctionnalités

### 1. Suivi de Couleur (Figure imposée)
* Détection d'une couleur spécifique en temps réel à l'aide de la fonction `cv::inRange()`.
* Création d'un rectangle englobant l'objet détecté avec `cv::boundingRect()` et calcul de son point milieu.
* Affichage d'un point rouge indiquant le centre de l'objet coloré.
* Contrôle dynamique des servomoteurs (axes X et Y via `sendServoPosition`) pour maintenir constamment le point milieu de l'objet au centre de l'image.

### 2. Reconnaissance Faciale (Figure libre)
* Détection des visages dans une vidéo en temps réel via `cv::CascadeClassifier::detectMultiScale()` en utilisant le classificateur `haarcascade_frontalface_alt2.xml`.
* Enregistrement automatique des visages détectés dans des images individuelles au format PNG.
* Préparation des données d'apprentissage en redimensionnant les visages à 200x200 pixels et en les convertissant en niveaux de gris.
* Entraînement d'un modèle de reconnaissance avec l'algorithme LBPH (`cv::face::LBPHFaceRecognizer`).
* Identification en temps réel : affichage d'un rectangle vert avec le nom si l'individu est reconnu (indice de confiance inférieur à 95), ou d'un rectangle rouge pour les individus non-reconnus.
