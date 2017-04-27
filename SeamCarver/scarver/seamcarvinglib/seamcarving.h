#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <cstdio>
#include "graph.h"

using namespace cv;
using namespace std;

double inner_dp[2010][2010], inner_choice[2010][2010];
double pixel_energy[2010][2010];
int pos[2010];
int cnt = 0;
vector<int> removed_pixels;
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

void remove_v_seam(graph& g, int check_negative = 0)
{

    int m = g.width, n = g.height, i, j;

    int cur = g.topleft, rowstart = g.topleft;
    int negative_count = 0;
    for (i = 0; i < n; i++)
    {
        cur = rowstart;
        for (j = 0; j < m; j++)
        {
            pixel_energy[i][j] = g.getEnergy(cur);
            if(pixel_energy[i][j] < 0)  negative_count ++;
            cur = g.pixelarray[cur].right;
        }
        rowstart = g.pixelarray[rowstart].bottom;
    }

    if(negative_count < check_negative)    return;   //if no negative energy pixel exists, object has been removed!

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

    // graph New;
    // New = g;
    g.width = g.width - 1;
    cur = getptr(g, n - 1, pos[n - 1]);
    for (i = n - 1; i >= 0; --i)
    {
        removed_pixels.push_back(cur);

        int Left = g.pixelarray[cur].left, Right = g.pixelarray[cur].right;
        if (Left != -1)
        {
            g.pixelarray[Left].right = Right;
            g.pixelarray[Left].updated = false;
        }
        if (Right != -1)
        {
            g.pixelarray[Right].left = Left;
            g.pixelarray[Right].updated = false;
        }
        if (!i)
        {
            if (cur == g.topleft)
                g.topleft = g.pixelarray[cur].right;
            break;
        }
        if (pos[i] == pos[i - 1])
            cur = g.pixelarray[cur].top;
        else if (pos[i] > pos[i - 1])
        {
            int Left = g.pixelarray[cur].left, Right = g.pixelarray[cur].top;
            g.pixelarray[Left].top = Right;
            g.pixelarray[Left].updated = false;

            g.pixelarray[Right].bottom = Left;
            g.pixelarray[Right].updated = false;

            cur = g.pixelarray[g.pixelarray[cur].top].left;
        }
        else
        {
            int Right = g.pixelarray[cur].right, Left = g.pixelarray[cur].top;
            g.pixelarray[Right].top = Left;
            g.pixelarray[Right].updated = false;

            g.pixelarray[Left].bottom = Right;
            g.pixelarray[Left].updated = false;

            cur = g.pixelarray[g.pixelarray[cur].top].right;
        }
    }
    // imwrite( "./GIF/"+std::to_string(cnt++)+".png", gif_write.convertgraphtoimage() );

    cout << g.height << ' ' << g.width << ' '<<negative_count<<' '<<check_negative<<'\n';
    // g.convertgraphtoimage();
    // return g;
}

void remove_h_seam(graph& g, int check_negative = 0)
{
    int m = g.width, n = g.height, i, j;

    int cur = g.topleft, rowstart = g.topleft;
    int negative_count = 0;
    for (i = 0; i < n; i++)
    {
        cur = rowstart;
        for (j = 0; j < m; j++)
        {
            pixel_energy[i][j] = g.getEnergy(cur);
            if(pixel_energy[i][j] < 0)  negative_count++;
            cur = g.pixelarray[cur].right;
        }
        rowstart = g.pixelarray[rowstart].bottom;
    }

    if(negative_count < check_negative)    return;   //if no negative energy pixel exists, object has been removed!

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

    // graph New;
    // New = g;
    g.height = g.height - 1;
    cur = getptr(g, pos[m - 1], m - 1);
    for (i = m - 1; i >= 0; --i)
    {
        removed_pixels.push_back(cur);

        int Top = g.pixelarray[cur].top, Bottom = g.pixelarray[cur].bottom;
        
        if (Top != -1)
        {
            g.pixelarray[Top].bottom = Bottom;
            g.pixelarray[Top].updated = false;
        }
        if (Bottom != -1)
        {
            g.pixelarray[Bottom].top = Top;
            g.pixelarray[Bottom].updated = false;
        }
        if (!i)
        {
            if (cur == g.topleft)
                g.topleft = g.pixelarray[cur].bottom;
            break;
        }
        if (pos[i] == pos[i - 1])
            cur = g.pixelarray[cur].left;
        else if (pos[i] > pos[i - 1])
        {
            int Left = g.pixelarray[cur].left, Right = g.pixelarray[cur].top;
            g.pixelarray[Left].right = Right;
            g.pixelarray[Left].updated = false;

            g.pixelarray[Right].left = Left;
            g.pixelarray[Right].updated = false;

            cur = g.pixelarray[g.pixelarray[cur].left].top;
        }
        else
        {
            int Left = g.pixelarray[cur].left, Right = g.pixelarray[cur].bottom;
            g.pixelarray[Right].left = Left;
            g.pixelarray[Right].updated = false;

            g.pixelarray[Left].right = Right;
            g.pixelarray[Left].updated = false;

            cur = g.pixelarray[g.pixelarray[cur].left].bottom;
        }
    }
    // imwrite( "./GIF/"+std::to_string(cnt++)+".png", gif_write.convertgraphtoimage() );
    cout << g.height << ' ' << g.width << ' '<<negative_count<<' '<<check_negative<<'\n';
    // return g;
}

void insert_h_seam(graph& g)
{
    // cout << "inserting horiz seam!" << endl;
    int m = g.width, n = g.height, i, j;

    int cur = g.topleft, rowstart = g.topleft;

    double energy_sum = 0;
    for (i = 0; i < n; i++)
    {
        cur = rowstart;
        for (j = 0; j < m; j++)
        {
            // cout << "nextcur: " << cur;
            pixel_energy[i][j] = g.getEnergy(cur);
            energy_sum += pixel_energy[i][j];
            cur = g.pixelarray[cur].right;
        }
        rowstart = g.pixelarray[rowstart].bottom;
    }
    energy_sum /= (n*m);
    if(g.PENALTY < 0)   g.PENALTY = energy_sum;
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

    // graph New;
    // New = g;
    g.height = g.height + 1;
    // cout << "I am here!" << endl;
    cur = getptr(g, pos[m - 1], m - 1);

    int index = g.pixelarray_size;
    g.pixelarray.push_back(
        pixel(g.pixelarray[cur].val[0], g.pixelarray[cur].val[1], g.pixelarray[cur].val[2]));
    g.pixelarray_size++;
    g.pixelarray[index].updated = false;
    int Top = g.pixelarray[cur].top, Bottom = g.pixelarray[cur].bottom;
    // cout << "top=" << Top << " bottom= " << Bottom << "\n";
    for (i = m - 1; i >= 0; --i)
    {
        // cout<<"cur: "<<cur<<endl;
        // cout<<"index: "<<index<<" matrix last index: "<<g.pixelarray.size()-1;
        // cout << "here1" << endl;

        g.pixelarray[cur].bottom = index;
        g.pixelarray[cur].updated = false;

        if (Bottom != -1)
        {
            g.pixelarray[Bottom].top = index;
            g.pixelarray[Bottom].updated = false;
        }
        g.pixelarray[index].top = cur;
        g.pixelarray[index].bottom = Bottom;
        g.pixelarray[index].updated = false;

        if (!i)
        {
            if (cur == g.topleft)
            {
                // cout<<"true!"<<endl;

                // g.topleft = g.pixelarray[cur].bottom;
            }
            break;
        }
        if (pos[i] == pos[i - 1])
        {
            // cout << "here2" << endl;

            int prevcur = cur;
            cur = g.pixelarray[cur].left;
            // make new pixel & set the corresponding right for it
            Top = g.pixelarray[cur].top;
            Bottom = g.pixelarray[cur].bottom;
            g.pixelarray.push_back(pixel(g.pixelarray[cur].val[0], g.pixelarray[cur].val[1],
                g.pixelarray[cur].val[2]));
            g.pixelarray_size++;

            g.pixelarray[index].left = index + 1;
            g.pixelarray[index].updated = false;

            g.pixelarray[index + 1].right = index;
            g.pixelarray[index].updated = false;

            g.modify(prevcur,0);
            g.modify(index,2);
            index++;
        }
        else if (pos[i] > pos[i - 1])
        {
            // cout << "here3" << endl;
            try
            {
                int prevcur = cur;
                // cout << "oldcur" << cur << endl;

                int nextleft = g.pixelarray[cur].left;
                // cout << "nextleft" << nextleft << endl;
                cur = g.pixelarray[g.pixelarray[cur].left].top;
                // cout << "i: " << i << endl;
                // cout << "posi" << pos[i] << "posi-1" << pos[i - 1] << endl;
                // cout << "newcur" << cur << endl;
                // cout << "top" << Top << "Bottom" << Bottom;
                Top = g.pixelarray[cur].top;
                Bottom = g.pixelarray[cur].bottom;
                g.pixelarray.push_back(pixel(g.pixelarray[cur].val[0],
                    g.pixelarray[cur].val[1], g.pixelarray[cur].val[2]));

                g.pixelarray[prevcur].left = index + 1;
                g.pixelarray[prevcur].updated = false;

                g.pixelarray[index + 1].right = prevcur;
                g.pixelarray[index + 1].updated = false;

                g.pixelarray_size++;

                g.pixelarray[index].left = nextleft;
                g.pixelarray[index].updated = false;

                g.pixelarray[nextleft].right = index;
                g.pixelarray[nextleft].updated = false;
                g.modify(prevcur,0);
                g.modify(index,2);
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
            cur = g.pixelarray[g.pixelarray[cur].left].bottom;
            int nextleft = cur;
            int prevbot = Bottom;
            Top = g.pixelarray[cur].top;
            Bottom = g.pixelarray[cur].bottom;
            g.pixelarray.push_back(pixel(g.pixelarray[cur].val[0], g.pixelarray[cur].val[1],
                g.pixelarray[cur].val[2]));
            g.pixelarray_size++;
            g.pixelarray[index + 1].right = prevbot;
            g.pixelarray[index + 1].updated = false;

            g.pixelarray[prevbot].left = index + 1;
            g.pixelarray[prevbot].updated = false;

            g.pixelarray[index].left = nextleft;
            g.pixelarray[index].updated = false;

            g.pixelarray[nextleft].right = index;
            g.pixelarray[nextleft].updated = false;
            g.modify(prevcur,0);
            g.modify(index,2);
            index++;
        }
    }
    cout << g.height << ' ' << g.width <<' '<<g.PENALTY<<'\n';
    // g.convertgraphtoimage();
    // return g;
}

void insert_v_seam(graph& g)
{
    int m = g.width, n = g.height, i, j;

    int cur = g.topleft, rowstart = g.topleft;
    double energy_sum = 0;
    for (i = 0; i < n; i++)
    {
        cur = rowstart;
        for (j = 0; j < m; j++)
        {
            pixel_energy[i][j] = g.getEnergy(cur);
            energy_sum += pixel_energy[i][j];
            cur = g.pixelarray[cur].right;
        }
        rowstart = g.pixelarray[rowstart].bottom;
    }
    energy_sum /= (n*m);
    if(g.PENALTY < 0)   g.PENALTY = energy_sum;
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

    // graph New;
    // New = g;
    g.width = g.width + 1;
    cur = getptr(g, n-1,pos[n - 1]);

    //debug(1);

    int index = g.pixelarray_size;
    g.pixelarray.push_back(
        pixel(g.pixelarray[cur].val[0], g.pixelarray[cur].val[1], g.pixelarray[cur].val[2]));
    g.pixelarray_size++;
    g.pixelarray[index].updated = false;

    int Left = g.pixelarray[cur].left, Right = g.pixelarray[cur].right;
    // cout << "left=" << Left << " right= " << Right << "\n";
    for (i = n - 1; i >= 0; --i)
    {
        // cout<<"cur: "<<cur<<endl;
        // cout<<"index: "<<index<<" matrix last index: "<<g.pixelarray.size()-1;
        // cout << "here1" << endl;

        //debug(2);

        g.pixelarray[cur].right = index;
        g.pixelarray[cur].updated = false;

        if (Right != -1)
        {
            g.pixelarray[Right].left = index;
            g.pixelarray[Right].updated = false;
        }

        //debug(3);

        g.pixelarray[index].left = cur;
        g.pixelarray[index].right = Right;
        g.pixelarray[index].updated = false;

        if (!i)
        {
            if (cur == g.topleft)
            {
                // cout<<"true!"<<endl;

                // g.topleft = g.pixelarray[cur].right;
            }
            break;
        }

        //debug(4);

        if (pos[i] == pos[i - 1])
        {
            // cout << "here2" << endl;
            //debug(5);
            int prevcur = cur;
            cur = g.pixelarray[cur].top;
            // make new pixel & set the corresponding bottom for it
            //debug(1000);
            Left = g.pixelarray[cur].left;
            //debug(2000);
            Right = g.pixelarray[cur].right;
            //debug(100);
            g.pixelarray.push_back(pixel(g.pixelarray[cur].val[0], g.pixelarray[cur].val[1],
                g.pixelarray[cur].val[2]));
            //debug(200);
            g.pixelarray_size++;
            g.pixelarray[index].top = index + 1;
            g.pixelarray[index].updated = false;
            g.pixelarray[index + 1].bottom = index;
            g.pixelarray[index + 1].updated = false;
            g.modify(prevcur,3);
            g.modify(index,1);
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

                int nextleft = g.pixelarray[cur].top;
                // cout << "nextleft" << nextleft << endl;
                cur = g.pixelarray[g.pixelarray[cur].top].left;
                // cout << "i: " << i << endl;
                // cout << "posi" << pos[i] << "posi-1" << pos[i - 1] << endl;
                // cout << "newcur" << cur << endl;
                // cout << "left" << Left << "Right" << Right;
                Left = g.pixelarray[cur].left;
                Right = g.pixelarray[cur].right;
                g.pixelarray.push_back(pixel(g.pixelarray[cur].val[0],
                    g.pixelarray[cur].val[1], g.pixelarray[cur].val[2]));
                g.pixelarray[prevcur].top = index + 1;
                g.pixelarray[prevcur].updated = false;

                g.pixelarray[index + 1].bottom = prevcur;
                g.pixelarray[index + 1].updated = false;

                g.pixelarray_size++;
                g.pixelarray[index].top = nextleft;
                g.pixelarray[index].updated = false;

                g.pixelarray[nextleft].bottom = index;
                g.pixelarray[nextleft].updated = false;
                g.modify(prevcur,3);
                g.modify(index,1);

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
            cur = g.pixelarray[g.pixelarray[cur].top].right;
            int nextleft = cur;
            int prevbot = Right;
            Left = g.pixelarray[cur].left;
            Right = g.pixelarray[cur].right;
            g.pixelarray.push_back(pixel(g.pixelarray[cur].val[0], g.pixelarray[cur].val[1],
                g.pixelarray[cur].val[2]));
            g.pixelarray_size++;
            g.pixelarray[index + 1].bottom = prevbot;
            g.pixelarray[index + 1].updated = false;

            g.pixelarray[prevbot].top = index + 1;
            g.pixelarray[prevbot].updated = false;


            g.pixelarray[index].top = nextleft;
            g.pixelarray[index].updated = false;

            g.pixelarray[nextleft].bottom = index;
            g.pixelarray[nextleft].updated = false;
            g.modify(prevcur,3);
            g.modify(index,1);

            index++;
        }
    }
    //debug(5);
    cout << g.height << ' ' << g.width << '\n';
    // g.convertgraphtoimage();
    // return g;
}
Mat rescale(const Mat& image, double r_height, double r_width)
{
    removed_pixels.clear();
    int r = abs((int)(image.size().height * (1 - r_height)));
    int c = abs((int)(image.size().width * (1 - r_width)));
    graph g(image), original(image);
    cout<<r<<' '<<c<<'\n';
    int cnt = 0;
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
                    if(r_width > 1)
                        insert_v_seam(g);
                    else
                        remove_v_seam(g);
                }
                if(r_height > 1)
                    insert_h_seam(g);
                else
                    remove_h_seam(g);
                remaining += q;
            }
        }
        for (int i = 0; i < c; i++)
        {
            if(r_width > 1)
                insert_v_seam(g);
            else
                remove_v_seam(g);
        }
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
                    if(r_height > 1)
                        insert_h_seam(g);
                    else
                        remove_h_seam(g);
                }

                if(r_width > 1)
                    insert_v_seam(g);
                else
                    remove_v_seam(g);
                remaining += q;
            }
        }
        for (int i = 0; i < r; i++)
        {
            if(r_height > 1)
                insert_h_seam(g);
            else
                remove_h_seam(g);
        }
    }

    for(auto cur:removed_pixels)
    {
        original.pixelarray[cur].val[0] = original.pixelarray[cur].val[1] = 0;
        original.pixelarray[cur].val[2] = 255;
    }
    imwrite( "./scarver/static/UploadedImages/original.jpg", original.convertgraphtoimage() );

    return g.convertgraphtoimage();
}

Mat remove_object(const Mat& image, const Mat& mask)
{
    graph g(image);
    // int start = getptr(g, 0, 0),i,j;
    // double energy_sum = 0;
    // for(i=0;i<g.height;i++)
    // {
    //     int cur=start;
    //     start = g.pixelarray[start].bottom;
    //     for(j=0;j<g.width;j++)
    //     {
    //         energy_sum += g.getEnergy(cur);
    //         cur=g.pixelarray[cur].right;
    //     }
    // }
    // energy_sum /= (g.width*g.height);
    // cout<<energy_sum<<'\n';
    int start = getptr(g, 0, 0), i,j;
    int height = mask.size().height, width = mask.size().width;
    int minr=1e8, minc=1e8, maxr=-1, maxc=-1;
    for(i=0;i<height;i++)
    {
        int cur=start;
        start = g.pixelarray[start].bottom;
        for(j=0;j<width;j++)
        {
            if(mask.at<uchar>(i,j) == 0)
            {
                minr = min(minr,i);
                maxr = max(maxr,i);
                minc = min(minc,j);
                maxc = max(maxc,j);
                g.pixelarray[cur].penalty = -1e9;
            }
            else if(mask.at<uchar>(i,j) == 254)
                g.pixelarray[cur].penalty = 1e7;
            cur=g.pixelarray[cur].right;
        }
    }
    if(maxr == -1)  return g.convertgraphtoimage();
    int r=g.height, c = g.width;
    int r_orig = r, c_orig = c;
    while(true)
    {
        if(maxr-minr>=maxc-minc)
            remove_v_seam(g,1);
        else
            remove_h_seam(g,1);
        if(g.height == r && g.width == c)   break;
        r=g.height, c=g.width;
    }
    while(g.height < r_orig)
        insert_h_seam(g);
    while(g.width < c_orig)
        insert_v_seam(g);
    return g.convertgraphtoimage();
}
