#include <iostream>
#include "seamcarving.h"
// command: g++ -fPIC -shared -o shared_seamcarving.so shared_seamcarving.cpp `pkg-config --cflags --libs python` `pkg-config --cflags --libs opencv` -I/usr/local/include/opencv -I/usr/local/include/opencv2 -L/usr/local/lib/
extern "C" {
    void Rescale(char* filename, double r_height, double r_width)
    {
    	Mat image = imread(filename, CV_LOAD_IMAGE_COLOR);
	    if (!image.data) // Check for invalid input
	    {
	        cout << "Could not open or find the image" << std::endl;
	        return;
	    }
	    image = rescale(image, r_height, r_width);
	    string newfilename = "";
	    newfilename += filename;
	    int pos = newfilename.find_last_of(".");
	    newfilename = newfilename.substr(0,pos)+"_carved"+newfilename.substr(pos);
	    imwrite( newfilename.c_str(), image );
    }
    void Amplify(char* filename, double extent = 1.25)
    {
    	Mat image = imread(filename, CV_LOAD_IMAGE_COLOR);
	    if (!image.data) // Check for invalid input
	    {
	        cout << "Could not open or find the image" << std::endl;
	        return;
	    }
	    resize(image, image, Size(), extent, extent);
	    image = rescale(image, 1/extent, 1/extent);
	    string newfilename = "";
	    newfilename += filename;

	    int pos = newfilename.find_last_of(".");
	    newfilename = newfilename.substr(0,pos)+"_carved"+newfilename.substr(pos);
	    imwrite( newfilename.c_str(), image );
    }

}
