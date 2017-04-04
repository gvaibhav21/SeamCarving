#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <cstdio>

using namespace cv;
using namespace std;

class pixel
{
public:
    double val[3];
    int top, bottom, right, left;
    bool updated;
    double energy;
    double penalty;
    pixel(int b = 0, int g = 0, int r = 0, int top = -1, int bottom = -1, int right = -1,
        int left = -1, bool updated = false)
    {
        val[0] = b;
        val[1] = g;
        val[2] = r;
        this->top = top;
        this->bottom = bottom;
        this->right = right;
        this->left = left;
        this->updated = updated;
        penalty = 0;
    }
    pixel& operator=(pixel p)
    {
        val[0] = p.val[0];
        val[1] = p.val[1];
        val[2] = p.val[2];
        top = p.top;
        bottom = p.bottom;
        right = p.right;
        left = p.left;
        updated = p.updated;
        energy = p.energy;
        penalty = p.penalty;
        return *this;
    }
};

class graph
{
public:
    double PENALTY;
    int image_type;
    vector<pixel> pixelarray;
    int height;
    int width;
    int topleft;
    int bottomright;
    int pixelarray_size;
    graph()
    {
        PENALTY = -1;
    }
    graph(const Mat& image)
    {
        PENALTY = -1;
        pixelarray_size = (int)image.size().height * (int)image.size().width + 100;
        pixelarray.resize(pixelarray_size);
        imageToGraph(image);
    }
    graph& operator=(graph g)
    {
        image_type = g.image_type;
        pixelarray_size = g.pixelarray_size;
        pixelarray.resize(pixelarray_size);
        for (int i = 0; i < pixelarray_size; i++)
            pixelarray[i] = g.pixelarray[i];
        height = g.height;
        PENALTY = g.PENALTY;
        width = g.width;
        topleft = g.topleft;
        bottomright = g.bottomright;
        return *this;
    }
    void imageToGraph(const Mat& image)
    {
        image_type = image.type();
        height = image.size().height;
        width = image.size().width;
        topleft = 0;
        bottomright = height * width - 1;
        int k = 0;
        for (int i = 0; i < height; ++i)
        {
            for (int j = 0; j < width; ++j)
            {
                if (i < image.size().height && j < image.size().width)
                {
                    int r, g, b;
                    int top = ((i == 0) ? (-1) : (k - width));
                    int bottom = ((i == height - 1) ? (-1) : (k + width));
                    int left = ((j == 0) ? (-1) : (k - 1));
                    int right = ((j == width - 1) ? (-1) : (k + 1));
                    b = image.at<Vec3b>(i, j).val[0];
                    g = image.at<Vec3b>(i, j).val[1];
                    r = image.at<Vec3b>(i, j).val[2];
                    // cout<<"adding pixel!"<<i<<" "<<j<<endl;
                    pixelarray[k++] = pixel(b, g, r, top, bottom, right, left);
                }
            }
        }
    }
    Mat convertgraphtoimage()
    {
        Mat New(height, width, image_type);
        int rowstart = topleft;
        int pixelindex = topleft;
        int rowi = 0;
        int colj = 0;
        while (rowstart != -1)
        {
            while (pixelindex != -1)
            {
                New.at<Vec3b>(rowi, colj) = Vec3b(pixelarray[pixelindex].val[0],
                    pixelarray[pixelindex].val[1], pixelarray[pixelindex].val[2]);
                colj++;
                pixelindex = pixelarray[pixelindex].right;
            }
            rowi++;
            colj = 0;
            rowstart = pixelarray[rowstart].bottom;
            pixelindex = rowstart;
        }
        // namedWindow("Display window!!!", WINDOW_AUTOSIZE);
        // imshow("Display window!!!", New);
        // waitKey(0);
        return New;
    }
    double getDiff(int p1, int p2)
    {
        double Sum = 0;
        for (int i = 0; i < 3; i++)
            Sum += abs(pixelarray[p1].val[i] - pixelarray[p2].val[i]);
        return Sum;
    }
    double getEnergy(int id)
    {
        if (pixelarray[id].updated)
            return pixelarray[id].energy+pixelarray[id].penalty;
        pixelarray[id].updated = true;
        int cnt = 0;
        double Sum = 0;
        if (pixelarray[id].top != -1)
        {
            cnt++;
            Sum += getDiff(id, pixelarray[id].top);
        }
        if (pixelarray[id].bottom != -1)
        {
            cnt++;
            Sum += getDiff(id, pixelarray[id].bottom);
        }
        if (pixelarray[id].right != -1)
        {
            cnt++;
            Sum += getDiff(id, pixelarray[id].right);
        }
        if (pixelarray[id].left != -1)
        {
            cnt++;
            Sum += getDiff(id, pixelarray[id].left);
        }
        pixelarray[id].energy = Sum / cnt;
        return pixelarray[id].energy + pixelarray[id].penalty;
    }
    void adjust(int x,int y)
    {
        pixelarray[x].penalty += PENALTY;
        if(y==-1)   return;
        for(int i=0;i<3;i++)
            pixelarray[x].val[i] = (pixelarray[x].val[i]+pixelarray[y].val[i])/2;
        if(pixelarray[x].top!=-1)       pixelarray[pixelarray[x].top].updated=false;
        if(pixelarray[x].bottom!=-1)    pixelarray[pixelarray[x].bottom].updated=false;
        if(pixelarray[x].left!=-1)      pixelarray[pixelarray[x].left].updated=false;
        if(pixelarray[x].right!=-1)     pixelarray[pixelarray[x].right].updated=false;
        
    }
    void modify(int id,int dir)
    {
        if(dir==0)
            adjust(id,pixelarray[id].top);
        else if(dir==1)
            adjust(id,pixelarray[id].right);
        else if(dir==2)
            adjust(id,pixelarray[id].bottom);
        else if(dir==3)
            adjust(id,pixelarray[id].left);
    }
};
