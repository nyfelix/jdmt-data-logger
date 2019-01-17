// This is the main application for the Neurnal Network Trainer
// This applicaiton in a first run is desingt to be run on the
// native OS, not on Arduino. However all data processing is
// encapsulated in the image and brain classes. So they could be
// used in an Arduino environement, too.
// Crossplatform compling is prepared, but a major switch has to be
// implemented here.
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <image.h>
#include <sample0001.h>

// Implementation for native OS
int main()
{
    printf("This is the Image Trainer v.0.1\n");
    printf("Load Image OK ...\n");
    Image* imgOK = new Image();
    imgOK->loadFrom265GrayArray(sample0001_map);
    imgOK->cropToBoundingBox();
    imgOK->normalize();
    imgOK->printImage();
    imgOK->printNormImage();
    printf("Load Image Not OK ...\n");
    Image* imgNOK = new Image();
    imgNOK->loadFrom265GrayArray(sample0002_map);
    imgNOK->cropToBoundingBox();
    imgNOK->printImage();
    //imgNOK->printNormImage();
    // Classiifcation: Momenten-Invarianten berechnung

    return 0;
}
