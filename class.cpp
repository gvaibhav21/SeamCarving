
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

using namespace cv;
using namespace std; 

class pixel
{
	
public:
	unsigned int r,g,b;
	int top, bottom, right, left;
	bool flag;
	pixel(unsigned int r=0, unsigned int g=0, unsigned int b=0, int top=-1, int bottom=-1, int right=-1, int left=-1, bool flag=false)
	{
		this->r = r;
		this->g=g;
		this->b=b;
		this->top = top; 
		this->bottom = bottom; 
		this->right = right; 
		this->left = left;
		this->flag = flag;
	}
	
};

class graph
{
public:
	Mat origimage;
	pixel * pixelarray;
	int height;
	int width;
	int topleft;
	int bottomright;
	graph(Mat image){
		pixelarray = new pixel[1000000];
		imageToGraph(image);

	}
	
	void imageToGraph(Mat image)
	{
		origimage = image;
		height = image.size().height;
		width = image.size().width;
		topleft = 0;
		bottomright = height*width -1;
		int k = 0;
		for (int i = 0; i < height; ++i)
		{
			for (int j = 0; j < width; ++j)
			{
				if(i<image.size().height && j<image.size().width)
				{
					unsigned int r, g, b;
					int top = ((i==0)?(-1):(k-width));
					int bottom = ((i==height-1)?(-1):(k+width));
					int left = ((j==0)?(-1):(k-1));
					int right = ((j==width -1)?(-1):(k+1));	
					b =  image.at<Vec3b>(i,j).val[0];
					g = image.at<Vec3b>(i,j).val[1];
					r = image.at<Vec3b>(i,j).val[2];
					// cout<<"adding pixel!"<<i<<" "<<j<<endl;
					pixelarray[k++]= pixel(r,g,b,top, bottom, right, left);
				}
				
			}
		}
	}
	void convertgraphtoimage()
	{
		Mat New(height,width,origimage.type());

		int rowstart = topleft;
		int pixelindex = topleft;
		int rowi = 0;
		int colj = 0;
		while(rowstart!=-1)
		{
			while(pixelindex!=-1)
			{
				// cout<<"putting at "<<rowi<<" "<<colj<<endl;
				New.at<Vec3b>(rowi, colj) = Vec3b(pixelarray[pixelindex].b, pixelarray[pixelindex].g, pixelarray[pixelindex].r);
				colj++;
				pixelindex = pixelarray[pixelindex].right;
			}
			rowi++;
			colj = 0;
			rowstart = pixelarray[rowstart].bottom;
			pixelindex = rowstart;
		}
		 namedWindow( "Display window!!!", WINDOW_AUTOSIZE );
		 imshow( "Display window!!!", New ); 
		 waitKey(0);                  
	}
	void showgraphvalues()
	{
		for (int i = 0; i < height*width; ++i)
		{

			 cout<<i<<": "<<pixelarray[i].r<<" "<<pixelarray[i].g<<" "<<pixelarray[i].b<<" "<<pixelarray[i].top<<" "<<pixelarray[i].bottom<<" "<<pixelarray[i].left<<" "<<pixelarray[i].right;
			 cout<<endl;
		}
	}
	
};

int main()
{
	// cout<<"starting!"<<endl;
	Mat image = imread("jena.jpg", CV_LOAD_IMAGE_COLOR);
	if(! image.data )                              // Check for invalid input
     {
         cout <<  "Could not open or find the image" << std::endl ;
         return -1;
     }
     namedWindow( "original image window!!!", WINDOW_AUTOSIZE );
		 imshow( "original image window!!!", image ); 
		 waitKey(0);  
     graph g(image);
     // g.showgraph();
     g.convertgraphtoimage();
    return 0;

}