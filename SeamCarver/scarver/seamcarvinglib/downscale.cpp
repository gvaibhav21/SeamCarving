#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <cstdio>

using namespace cv;
using namespace std;

#define debug(mark) printf("check%d\n", mark)
#define PENALTY 100.0
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
    int image_type;
    vector<pixel> pixelarray;
    int height;
    int width;
    int topleft;
    int bottomright;
    int pixelarray_size;
    graph()
    {
    }
    graph(const Mat& image)
    {
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
    void convertgraphtoimage()
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
        namedWindow("Display window!!!", WINDOW_AUTOSIZE);
        imshow("Display window!!!", New);
        waitKey(0);
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

double inner_dp[1010][1010], inner_choice[1010][1010];
double pixel_energy[1010][1010];
int pos[1010];

int getptr(graph& g, int x, int y)
{
    // cout << "here!" << endl;
    int cur = g.topleft;
    while (x--)
        cur = g.pixelarray[cur].bottom;
    while (y--)
        cur = g.pixelarray[cur].right;
    return cur;
}

graph remove_v_seam(graph& g)
{
    int m = g.width, n = g.height, i, j;

    int cur = g.topleft, rowstart = g.topleft;

    for (i = 0; i < n; i++)
    {
        cur = rowstart;
        for (j = 0; j < m; j++)
        {
            pixel_energy[i][j] = g.getEnergy(cur);
            cur = g.pixelarray[cur].right;
        }
        rowstart = g.pixelarray[rowstart].bottom;
    }
    for (i = 0; i < m; i++)
        inner_dp[0][i] = pixel_energy[0][i];
    for (i = 1; i < n; i++)
    {
        for (j = 0; j < m; j++)
        {
            double e = pixel_energy[i][j];
            inner_dp[i][j] = e + inner_dp[i - 1][j];
            inner_choice[i][j] = j;
            if (j && inner_dp[i][j] > e + inner_dp[i - 1][j - 1])
            {
                inner_dp[i][j] = e + inner_dp[i - 1][j - 1];
                inner_choice[i][j] = j - 1;
            }
            if (j + 1 < m && inner_dp[i][j] > e + inner_dp[i - 1][j + 1])
            {
                inner_dp[i][j] = e + inner_dp[i - 1][j + 1];
                inner_choice[i][j] = j + 1;
            }
        }
    }
    double minn = 1e50;
    int minpos;
    for (i = 0; i < m; i++)
    {
        if (inner_dp[n - 1][i] < minn)
        {
            minn = inner_dp[n - 1][i];
            minpos = i;
        }
    }

    cur = minpos;
    pos[n - 1] = minpos;
    for (i = n - 2; i >= 0; --i)
    {
        cur = inner_choice[i + 1][cur];
        pos[i] = cur;
    }

    graph New;
    New = g;
    New.width = New.width - 1;
    cur = getptr(New, n - 1, pos[n - 1]);
    for (i = n - 1; i >= 0; --i)
    {
        int Left = New.pixelarray[cur].left, Right = New.pixelarray[cur].right;
        if (Left != -1)
        {
            New.pixelarray[Left].right = Right;
            New.pixelarray[Left].updated = false;
        }
        if (Right != -1)
        {
            New.pixelarray[Right].left = Left;
            New.pixelarray[Right].updated = false;
        }
        if (!i)
        {
            if (cur == New.topleft)
                New.topleft = New.pixelarray[cur].right;
            break;
        }
        if (pos[i] == pos[i - 1])
            cur = New.pixelarray[cur].top;
        else if (pos[i] > pos[i - 1])
        {
            int Left = New.pixelarray[cur].left, Right = New.pixelarray[cur].top;
            New.pixelarray[Left].top = Right;
            New.pixelarray[Left].updated = false;

            New.pixelarray[Right].bottom = Left;
            New.pixelarray[Right].updated = false;

            cur = New.pixelarray[New.pixelarray[cur].top].left;
        }
        else
        {
            int Right = New.pixelarray[cur].right, Left = New.pixelarray[cur].top;
            New.pixelarray[Right].top = Left;
            New.pixelarray[Right].updated = false;

            New.pixelarray[Left].bottom = Right;
            New.pixelarray[Left].updated = false;

            cur = New.pixelarray[New.pixelarray[cur].top].right;
        }
    }
    // cout << New.height << ' ' << New.width << '\n';
    // New.convertgraphtoimage();
    return New;
}

graph remove_h_seam(graph& g)
{
    int m = g.width, n = g.height, i, j;

    int cur = g.topleft, rowstart = g.topleft;

    for (i = 0; i < n; i++)
    {
        cur = rowstart;
        for (j = 0; j < m; j++)
        {
            pixel_energy[i][j] = g.getEnergy(cur);
            cur = g.pixelarray[cur].right;
        }
        rowstart = g.pixelarray[rowstart].bottom;
    }
    for (i = 0; i < n; i++)
        inner_dp[i][0] = pixel_energy[i][0];
    for (j = 1; j < m; j++)
    {
        for (i = 0; i < n; i++)
        {
            double e = pixel_energy[i][j];
            inner_dp[i][j] = e + inner_dp[i][j - 1];
            inner_choice[i][j] = i;
            if (i && inner_dp[i][j] > e + inner_dp[i - 1][j - 1])
            {
                inner_dp[i][j] = e + inner_dp[i - 1][j - 1];
                inner_choice[i][j] = i - 1;
            }
            if (i + 1 < n && inner_dp[i][j] > e + inner_dp[i + 1][j - 1])
            {
                inner_dp[i][j] = e + inner_dp[i + 1][j - 1];
                inner_choice[i][j] = i + 1;
            }
        }
    }
    double minn = 1e50;
    int minpos;
    for (i = 0; i < n; i++)
    {
        if (inner_dp[i][m - 1] < minn)
        {
            minn = inner_dp[i][m - 1];
            minpos = i;
        }
    }

    cur = minpos;
    pos[m - 1] = minpos;
    for (i = m - 2; i >= 0; --i)
    {
        cur = inner_choice[cur][i + 1];
        pos[i] = cur;
    }

    graph New;
    New = g;
    New.height = New.height - 1;
    cur = getptr(New, pos[m - 1], m - 1);
    for (i = m - 1; i >= 0; --i)
    {
        int Top = New.pixelarray[cur].top, Bottom = New.pixelarray[cur].bottom;
        if (Top != -1)
        {
            New.pixelarray[Top].bottom = Bottom;
            New.pixelarray[Top].updated = false;
        }
        if (Bottom != -1)
        {
            New.pixelarray[Bottom].top = Top;
            New.pixelarray[Bottom].updated = false;
        }
        if (!i)
        {
            if (cur == New.topleft)
                New.topleft = New.pixelarray[cur].bottom;
            break;
        }
        if (pos[i] == pos[i - 1])
            cur = New.pixelarray[cur].left;
        else if (pos[i] > pos[i - 1])
        {
            int Left = New.pixelarray[cur].left, Right = New.pixelarray[cur].top;
            New.pixelarray[Left].right = Right;
            New.pixelarray[Left].updated = false;

            New.pixelarray[Right].left = Left;
            New.pixelarray[Right].updated = false;

            cur = New.pixelarray[New.pixelarray[cur].left].top;
        }
        else
        {
            int Left = New.pixelarray[cur].left, Right = New.pixelarray[cur].bottom;
            New.pixelarray[Right].left = Left;
            New.pixelarray[Right].updated = false;

            New.pixelarray[Left].right = Right;
            New.pixelarray[Left].updated = false;

            cur = New.pixelarray[New.pixelarray[cur].left].bottom;
        }
    }
    // cout << New.height << ' ' << New.width << '\n';
    return New;
}

void downscale(const Mat& image, int r, int c)
{
    graph g(image);
    if (r <= c)
    {
        if (r > 0)
        {
            double q = ((double)c) / r;
            double remaining = q;
            while (r--)
            {
                while (remaining > 0.5)
                {
                    c--;
                    remaining--;
                    g = remove_v_seam(g);
                }
                g = remove_h_seam(g);
                remaining += q;
            }
        }
        for (int i = 0; i < c; i++)
            g = remove_v_seam(g);
    }
    else
    {
        if (c > 0)
        {
            double q = ((double)r) / c;
            double remaining = q;
            while (c--)
            {
                while (remaining > 0.5)
                {
                    r--;
                    remaining--;
                    g = remove_h_seam(g);
                }
                g = remove_v_seam(g);
                remaining += q;
            }
        }
        for (int i = 0; i < r; i++)
            g = remove_h_seam(g);
    }
    g.convertgraphtoimage();
}
void downscale(const Mat& image, double r_height, double r_width)
{
    downscale(image, (int)(image.size().height * (1 - r_height)),
        (int)(image.size().width * (1 - r_width)));
}
graph insert_h_seam(graph& g)
{
    // cout << "inserting horiz seam!" << endl;
    int m = g.width, n = g.height, i, j;

    int cur = g.topleft, rowstart = g.topleft;

    for (i = 0; i < n; i++)
    {
        cur = rowstart;
        for (j = 0; j < m; j++)
        {
            // cout << "nextcur: " << cur;

            pixel_energy[i][j] = g.getEnergy(cur);
            cur = g.pixelarray[cur].right;
        }
        rowstart = g.pixelarray[rowstart].bottom;
    }
    // cout << "calculating dp " << endl;
    for (i = 0; i < n; i++)
        inner_dp[i][0] = pixel_energy[i][0];
    for (j = 1; j < m; j++)
    {
        for (i = 0; i < n; i++)
        {
            double e = pixel_energy[i][j];
            inner_dp[i][j] = e + inner_dp[i][j - 1];
            inner_choice[i][j] = i;
            if (i && inner_dp[i][j] > e + inner_dp[i - 1][j - 1])
            {
                inner_dp[i][j] = e + inner_dp[i - 1][j - 1];
                inner_choice[i][j] = i - 1;
            }
            if (i + 1 < n && inner_dp[i][j] > e + inner_dp[i + 1][j - 1])
            {
                inner_dp[i][j] = e + inner_dp[i + 1][j - 1];
                inner_choice[i][j] = i + 1;
            }
        }
    }
    double minn = 1e50;
    int minpos;
    for (i = 0; i < n; i++)
    {
        if (inner_dp[i][m - 1] < minn)
        {
            minn = inner_dp[i][m - 1];
            minpos = i;
        }
    }

    cur = minpos;
    pos[m - 1] = minpos;
    for (i = m - 2; i >= 0; --i)
    {
        cur = inner_choice[cur][i + 1];
        pos[i] = cur;
    }

    graph New;
    New = g;
    New.height = New.height + 1;
    // cout << "I am here!" << endl;
    cur = getptr(New, pos[m - 1], m - 1);

    int index = New.pixelarray_size;
    New.pixelarray.push_back(
        pixel(New.pixelarray[cur].val[0], New.pixelarray[cur].val[1], New.pixelarray[cur].val[2]));
    New.pixelarray_size++;
    New.pixelarray[index].updated = false;
    int Top = New.pixelarray[cur].top, Bottom = New.pixelarray[cur].bottom;
    // cout << "top=" << Top << " bottom= " << Bottom << "\n";
    for (i = m - 1; i >= 0; --i)
    {
        // cout<<"cur: "<<cur<<endl;
        // cout<<"index: "<<index<<" matrix last index: "<<New.pixelarray.size()-1;
        // cout << "here1" << endl;

        New.pixelarray[cur].bottom = index;
        New.pixelarray[cur].updated = false;

        if (Bottom != -1)
        {
            New.pixelarray[Bottom].top = index;
            New.pixelarray[Bottom].updated = false;
        }
        New.pixelarray[index].top = cur;
        New.pixelarray[index].bottom = Bottom;
        New.pixelarray[index].updated = false;
        
        if (!i)
        {
            if (cur == New.topleft)
            {
                // cout<<"true!"<<endl;

                // New.topleft = New.pixelarray[cur].bottom;
            }
            break;
        }
        if (pos[i] == pos[i - 1])
        {
            // cout << "here2" << endl;

            int prevcur = cur;
            cur = New.pixelarray[cur].left;
            // make new pixel & set the corresponding right for it
            Top = New.pixelarray[cur].top;
            Bottom = New.pixelarray[cur].bottom;
            New.pixelarray.push_back(pixel(New.pixelarray[cur].val[0], New.pixelarray[cur].val[1],
                New.pixelarray[cur].val[2]));
            New.pixelarray_size++;

            New.pixelarray[index].left = index + 1;
            New.pixelarray[index].updated = false;

            New.pixelarray[index + 1].right = index;
            New.pixelarray[index].updated = false;

            New.modify(prevcur,0);
            New.modify(index,2);
            index++;
        }
        else if (pos[i] > pos[i - 1])
        {
            // cout << "here3" << endl;
            try
            {
                int prevcur = cur;
                // cout << "oldcur" << cur << endl;

                int nextleft = New.pixelarray[cur].left;
                // cout << "nextleft" << nextleft << endl;
                cur = New.pixelarray[New.pixelarray[cur].left].top;
                // cout << "i: " << i << endl;
                // cout << "posi" << pos[i] << "posi-1" << pos[i - 1] << endl;
                // cout << "newcur" << cur << endl;
                // cout << "top" << Top << "Bottom" << Bottom;
                Top = New.pixelarray[cur].top;
                Bottom = New.pixelarray[cur].bottom;
                New.pixelarray.push_back(pixel(New.pixelarray[cur].val[0],
                    New.pixelarray[cur].val[1], New.pixelarray[cur].val[2]));
                
                New.pixelarray[prevcur].left = index + 1;
                New.pixelarray[prevcur].updated = false;
                
                New.pixelarray[index + 1].right = prevcur;
                New.pixelarray[index + 1].updated = false;
                
                New.pixelarray_size++;
                
                New.pixelarray[index].left = nextleft;
                New.pixelarray[index].updated = false;

                New.pixelarray[nextleft].right = index;
                New.pixelarray[nextleft].updated = false;
                New.modify(prevcur,0);
                New.modify(index,2);
                index++;
            }
            catch (Exception e)
            {
                cout << "exception" << e.what() << endl;
            }
        }
        else
        {
            int prevcur = cur;
            // cout << "here4" << endl;
            cur = New.pixelarray[New.pixelarray[cur].left].bottom;
            int nextleft = cur;
            int prevbot = Bottom;
            Top = New.pixelarray[cur].top;
            Bottom = New.pixelarray[cur].bottom;
            New.pixelarray.push_back(pixel(New.pixelarray[cur].val[0], New.pixelarray[cur].val[1],
                New.pixelarray[cur].val[2]));
            New.pixelarray_size++;
            New.pixelarray[index + 1].right = prevbot;
            New.pixelarray[index + 1].updated = false;
            
            New.pixelarray[prevbot].left = index + 1;
            New.pixelarray[prevbot].updated = false;
            
            New.pixelarray[index].left = nextleft;
            New.pixelarray[index].updated = false;

            New.pixelarray[nextleft].right = index;
            New.pixelarray[nextleft].updated = false;
            New.modify(prevcur,0);
            New.modify(index,2);
            index++;
        }
    }
    cout << New.height << ' ' << New.width << '\n';
    // New.convertgraphtoimage();
    return New;
}
// bool checkgraphokay(graph &g)
// {
// 	int rowstart = g.topleft;
// 	while(rowstart!=-1)
// 	{
// 		int colstart = rowstart;
// 		while(colstart!=-1)
// 		{
// 			int right = colstart.right;

// 		}
// 	}
// }

graph insert_v_seam(graph& g)
{
    int m = g.width, n = g.height, i, j;

    int cur = g.topleft, rowstart = g.topleft;

    for (i = 0; i < n; i++)
    {
        cur = rowstart;
        for (j = 0; j < m; j++)
        {
            pixel_energy[i][j] = g.getEnergy(cur);
            cur = g.pixelarray[cur].right;
        }
        rowstart = g.pixelarray[rowstart].bottom;
    }
    for (i = 0; i < m; i++)
        inner_dp[0][i] = pixel_energy[0][i];
    for (i = 1; i < n; i++)
    {
        for (j = 0; j < m; j++)
        {
            double e = pixel_energy[i][j];
            inner_dp[i][j] = e + inner_dp[i - 1][j];
            inner_choice[i][j] = j;
            if (j && inner_dp[i][j] > e + inner_dp[i - 1][j - 1])
            {
                inner_dp[i][j] = e + inner_dp[i - 1][j - 1];
                inner_choice[i][j] = j - 1;
            }
            if (j + 1 < m && inner_dp[i][j] > e + inner_dp[i - 1][j + 1])
            {
                inner_dp[i][j] = e + inner_dp[i - 1][j + 1];
                inner_choice[i][j] = j + 1;
            }
        }
    }
    double minn = 1e50;
    int minpos;
    for (i = 0; i < m; i++)
    {
        if (inner_dp[n - 1][i] < minn)
        {
            minn = inner_dp[n - 1][i];
            minpos = i;
        }
    }



    cur = minpos;
    pos[n - 1] = minpos;
    for (i = n - 2; i >= 0; --i)
    {
        cur = inner_choice[i + 1][cur];
        pos[i] = cur;
    }

    graph New;
    New = g;
    New.width = New.width + 1;
    cur = getptr(New, n-1,pos[n - 1]);

    //debug(1);

    int index = New.pixelarray_size;
    New.pixelarray.push_back(
        pixel(New.pixelarray[cur].val[0], New.pixelarray[cur].val[1], New.pixelarray[cur].val[2]));
    New.pixelarray_size++;
    New.pixelarray[index].updated = false;

    int Left = New.pixelarray[cur].left, Right = New.pixelarray[cur].right;
    // cout << "left=" << Left << " right= " << Right << "\n";
    for (i = n - 1; i >= 0; --i)
    {
        // cout<<"cur: "<<cur<<endl;
        // cout<<"index: "<<index<<" matrix last index: "<<New.pixelarray.size()-1;
        // cout << "here1" << endl;

        //debug(2);

        New.pixelarray[cur].right = index;
        New.pixelarray[cur].updated = false;

        if (Right != -1)
        {
            New.pixelarray[Right].left = index;
            New.pixelarray[Right].updated = false;
        }

        //debug(3);

        New.pixelarray[index].left = cur;
        New.pixelarray[index].right = Right;
        New.pixelarray[index].updated = false;

        if (!i)
        {
            if (cur == New.topleft)
            {
                // cout<<"true!"<<endl;

                // New.topleft = New.pixelarray[cur].right;
            }
            break;
        }

        //debug(4);

        if (pos[i] == pos[i - 1])
        {
            // cout << "here2" << endl;
            //debug(5);
            int prevcur = cur;
            cur = New.pixelarray[cur].top;
            // make new pixel & set the corresponding bottom for it
            //debug(1000);
            Left = New.pixelarray[cur].left;
            //debug(2000);
            Right = New.pixelarray[cur].right;
            //debug(100);
            New.pixelarray.push_back(pixel(New.pixelarray[cur].val[0], New.pixelarray[cur].val[1],
                New.pixelarray[cur].val[2]));
            //debug(200);
            New.pixelarray_size++;
            New.pixelarray[index].top = index + 1;
            New.pixelarray[index].updated = false;
            New.pixelarray[index + 1].bottom = index;
            New.pixelarray[index + 1].updated = false;
            New.modify(prevcur,3);
            New.modify(index,1);
            index++;
        }
        else if (pos[i] > pos[i - 1])
        {
            //debug(6);
            // cout << "here3" << endl;
            try
            {
                int prevcur = cur;
                // cout << "oldcur" << cur << endl;

                int nextleft = New.pixelarray[cur].top;
                // cout << "nextleft" << nextleft << endl;
                cur = New.pixelarray[New.pixelarray[cur].top].left;
                // cout << "i: " << i << endl;
                // cout << "posi" << pos[i] << "posi-1" << pos[i - 1] << endl;
                // cout << "newcur" << cur << endl;
                // cout << "left" << Left << "Right" << Right;
                Left = New.pixelarray[cur].left;
                Right = New.pixelarray[cur].right;
                New.pixelarray.push_back(pixel(New.pixelarray[cur].val[0],
                    New.pixelarray[cur].val[1], New.pixelarray[cur].val[2]));
                New.pixelarray[prevcur].top = index + 1;
                New.pixelarray[prevcur].updated = false;
                
                New.pixelarray[index + 1].bottom = prevcur;
                New.pixelarray[index + 1].updated = false;

                New.pixelarray_size++;
                New.pixelarray[index].top = nextleft;
                New.pixelarray[index].updated = false;

                New.pixelarray[nextleft].bottom = index;
                New.pixelarray[nextleft].updated = false;
                New.modify(prevcur,3);
                New.modify(index,1);
            
                index++;
            }
            catch (Exception e)
            {
                cout << "exception" << e.what() << endl;
            }
        }
        else
        {
            int prevcur = cur;
            // cout << "here4" << endl;
            cur = New.pixelarray[New.pixelarray[cur].top].right;
            int nextleft = cur;
            int prevbot = Right;
            Left = New.pixelarray[cur].left;
            Right = New.pixelarray[cur].right;
            New.pixelarray.push_back(pixel(New.pixelarray[cur].val[0], New.pixelarray[cur].val[1],
                New.pixelarray[cur].val[2]));
            New.pixelarray_size++;
            New.pixelarray[index + 1].bottom = prevbot;
            New.pixelarray[index + 1].updated = false;

            New.pixelarray[prevbot].top = index + 1;
            New.pixelarray[prevbot].updated = false;


            New.pixelarray[index].top = nextleft;
            New.pixelarray[index].updated = false;

            New.pixelarray[nextleft].bottom = index;
            New.pixelarray[nextleft].updated = false;
            New.modify(prevcur,3);
            New.modify(index,1);
            
            index++;
        }
    }
    //debug(5);
    cout << New.height << ' ' << New.width << '\n';
    // New.convertgraphtoimage();
    return New;
}
void upscale(const Mat& image, int r, int c)
{
    cout << "upscaling!\n";
    graph g(image);
    if (r <= c)
    {
        if (r > 0)
        {
            double q = ((double)c) / r;
            double remaining = q;
            while (r--)
            {
                while (remaining > 0.5)
                {
                    c--;
                    remaining--;
                    g = insert_v_seam(g);
                }
                g = insert_h_seam(g);
                remaining += q;
            }
        }
        for (int i = 0; i < c; i++)
            g = insert_v_seam(g);
    }
    else
    {
        if (c > 0)
        {
            double q = ((double)r) / c;
            double remaining = q;
            while (c--)
            {
                while (remaining > 0.5)
                {
                    r--;
                    remaining--;
                    g = insert_h_seam(g);
                }
                g = insert_v_seam(g);
                remaining += q;
            }
        }
        for (int i = 0; i < r; i++)
            g = insert_h_seam(g);
    }
    g.convertgraphtoimage();
}
// r_height == final height in terms of original height. ex (1.25)
void upscale(const Mat& image, double r_height, double r_width)
{
    upscale(image, (int)(image.size().height * (r_height - 1)),
        (int)(image.size().width * (r_width - 1)));
}
int main()
{
    // cout<<"starting!"<<endl;
    Mat image = imread("../TestImages/group2.jpg", CV_LOAD_IMAGE_COLOR);
    if (!image.data) // Check for invalid input
    {
        cout << "Could not open or find the image" << std::endl;
        return -1;
    }
    namedWindow("original image window!!!", WINDOW_AUTOSIZE);
    imshow("original image window!!!", image);
    // waitKey(0);
    graph g(image);
    upscale(image, 1.0, 1.3);
    g.convertgraphtoimage();
    
    return 0;
}
