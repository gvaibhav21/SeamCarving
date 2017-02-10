#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

using namespace cv;
using namespace std;

#define INF 1e8

int dr[4]={-1,0,1,0},dc[4]={0,1,0,-1},n,m;
Mat image,New;

// R: 158 G: 112 B: 94
// R: 164 G: 137 B: 92

// int error

bool check(int r,int c)
{
    return r>=0 && r<n && c>=0 && c<m;
}

float Energy(int r,int c)
{
    int ret=0,cnt=0;
    for(int d=0;d<4;d++)
    {
        if(!check(r+dr[d],c+dc[d])) continue;
        cnt++;
        for(int color=0;color<3;color++)
            ret += abs( (int)image.at<Vec3b>(r+dr[d],c+dc[d]).val[color] - (int)image.at<Vec3b>(r,c).val[color] );
    }
    return (ret*1.0)/cnt;
}



int pos[1000];
float dp[1000][1000];
int choice[1000][1000];

int main()
{
    
    image = imread("scenery1.jpg", CV_LOAD_IMAGE_COLOR);   // Read the file

    if(! image.data )                              // Check for invalid input
    {
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }

    m=image.size().width;

    int newM = m*.75;
    int diff = m - newM;
    //diff = 1;      // COMMENT THIS!!!!----------------

    cout<<"TYPE: "<<image.type()<<'\n';

    for(int iter=0;iter<diff;iter++)
    {
        m=image.size().width,n=image.size().height;
        cout<<m<<' '<<n<<'\n';
        
        int i,j;
        
        for(i=0;i<m;i++)
            dp[0][i] = Energy(0,i);
        for(i=1;i<n;i++)
        {
            for(j=0;j<m;j++)
            {
                float e = Energy(i,j);
                dp[i][j] = e + dp[i-1][j];
                choice[i][j] = j;
                if(j && dp[i][j] > e + dp[i-1][j-1])
                {
                    dp[i][j] = e + dp[i-1][j-1];
                    choice[i][j] = j-1;
                }
                if(j+1 < m && dp[i][j] > e + dp[i-1][j+1])
                {
                    dp[i][j] = e + dp[i-1][j+1];
                    choice[i][j] = j+1;
                }
            }
        }

        float minn = INF;
        int minpos;
        for(i=0;i<m;i++)
        {
            if(dp[n-1][i] < minn)
            {
                minn = dp[n-1][i];
                minpos = i;
            }
        }

        cout<<minn/n<<'\n';

        int cur = minpos;
        pos[n-1] = minpos;
        for(i=n-2;i>=0;--i)
        {
            cur = choice[i+1][cur];
            pos[i] = cur;
        }

        Mat New(n,m-1,image.type());

        for(i=0;i<n;i++)
        {
            int p=0;
            for(j=0;j<m;j++)
                if(pos[i]!=j)
                    New.at<Vec3b>(i,p++) = Vec3b(image.at<Vec3b>(i,j).val[0],image.at<Vec3b>(i,j).val[1],image.at<Vec3b>(i,j).val[2]);
        }
        New.copyTo(image);
    }

    namedWindow( "Display window", WINDOW_AUTOSIZE );// Create a window for display.
    imshow( "Display window", image );                   // Show our image inside it.

    waitKey(0);                                          // Wait for a keystroke in the window
}