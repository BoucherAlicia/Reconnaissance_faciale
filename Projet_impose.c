//Alicia et Charbel
//Code projet imposé
#include <opencv2/opencv.hpp>
#include <fstream> // Bibliothèque pour la lecture de fichiers
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

int arduino;
  int servoY = 90;
  int servoX = 90;
void serial_init(int fd) {
  struct termios serial;
  // Configure serial port settings
  memset(&serial, 0, sizeof(serial));
  cfsetspeed(&serial, B9600); // Baud rate

  serial.c_cflag &= ~PARENB;                 // No parity
  serial.c_cflag &= ~CSTOPB;                 // 1 stop bit
  serial.c_cflag &= ~CSIZE;                  // Mask data size
  serial.c_cflag |= CS8;                     // 8 data bits
  serial.c_cflag &= ~CRTSCTS;                // no flow control
  serial.c_cflag |= CREAD | CLOCAL;          // turn on READ & ignore ctrl lines
  serial.c_iflag &= ~(IXON | IXOFF | IXANY); // turn off s/w flow ctrl
  serial.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // make raw
  serial.c_oflag &= ~OPOST;                          // make raw

  tcflush(fd, TCIFLUSH);
  tcsetattr(fd, TCSANOW, &serial);
}

void sendServoPosition(int servoX, int servoY)
{
    std::string message = std::to_string(servoX) + " " + std::to_string(servoY) + "\n";
    write(arduino, message.c_str(), message.length());
}
int map(int value, int inMin, int inMax, int outMin, int outMax) {
  return (value - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
}

int main()
{
    
    cv::VideoCapture cap(2); // ouvre camera 0 ou 2

    if (!cap.isOpened())
    {
        std::cout << "Failed to open the USB camera." << std::endl;
        return -1;
    }

  
    std::ofstream file("position.txt"); //fichier pour enregistrer la position du centre


    
    arduino = open("/dev/ttyACM0", O_RDWR | O_NOCTTY); //ouvrir le serial port arduino (0 ou 1)
    if (arduino == -1)
    {
        perror("Error opening serial port");
        return 1;
    }
    serial_init(arduino);

    double minContourArea = 100.0; 

    while (1)
    {
        cv::Mat frame;
        cap >> frame;

        cv::Mat frame_hsv;
        cv::cvtColor(frame, frame_hsv, cv::COLOR_BGR2HSV);

        //couleur green
        cv::Scalar lower_green(40, 50, 50);
        cv::Scalar upper_green(80, 255, 255);

        cv::Mat mask;
        cv::inRange(frame_hsv, lower_green, upper_green, mask);

        //contours
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        //contour le plus grand
        double maxArea = 0;
        int maxAreaIdx = -1;
        for (size_t i = 0; i < contours.size(); i++)
        {
            double area = cv::contourArea(contours[i]);
            if (area > maxArea)
            {
                maxArea = area;
                maxAreaIdx = i;
            }
        }

        //calculer le centre du contour et le marquer
        if (maxAreaIdx != -1 && maxArea > minContourArea)
        {
            cv::Moments moments = cv::moments(contours[maxAreaIdx]);
            int centerX = moments.m10 / moments.m00;
            int centerY = moments.m01 / moments.m00;


            cv::Rect boundingRect = cv::boundingRect(contours[maxAreaIdx]);
            cv::rectangle(frame, boundingRect, cv::Scalar(0, 255, 0), 2);

            //point rouge sur le centre
            cv::circle(frame, cv::Point(centerX, centerY), 4, cv::Scalar(0, 0, 255), cv::FILLED);

            //enregistre la position du centre
            file << centerY << " " << centerX << std::endl;

            //dimensions du frame
            int largeur_frame = frame.cols;
            int longueur_frame = frame.rows;

            //diviser le frame en 4 quadrants
            int largeur_quad = largeur_frame / 2;
            int longueur_quad = longueur_frame / 2;
           
          
           //0 < longeur_frame < 480 
           if(centerY<240) {
            	servoY = servoY - 2;
            }
            else {
            	servoY = servoY + 2;
            	}
    
    	   //0 < largeur_frame < 640
            if(centerX<320) {
            	servoX = servoX +2;
            }
            else {
          	  servoX = servoX -2;
            	}
            	

            //envoyer la position des servos sur arduino
            if(0<=servoX<=180 && 0<= servoY <=180) {
            sendServoPosition(servoX, servoY);
            }
        }
        cv::imshow("Green Object Detection", frame);
    }
    
    cap.release();

    file.close();

    return 0;
}

