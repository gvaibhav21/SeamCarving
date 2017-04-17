#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <cstdio>
#include "graph.h"

using namespace cv;
using namespace std;

double inner_dp[1010][1010], inner_choice[1010][1010];
double pixel_energy[1010][1010];
int pos[1010];
int cnt = 0;
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

graph remove_v_seam(graph& g, int check_negative = 0)
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
    
    if(negative_count < check_negative)    return g;   //if no negative energy pixel exists, object has been removed!
    
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

    graph New, gif_write;
    New = g;
    gif_write = g;
    New.width = New.width - 1;
    cur = getptr(New, n - 1, pos[n - 1]);
    for (i = n - 1; i >= 0; --i)
    {
        gif_write.pixelarray[cur].val[0] = gif_write.pixelarray[cur].val[1] = 0;
        gif_write.pixelarray[cur].val[2] = 255;
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
    // imwrite( "./GIF/"+std::to_string(cnt++)+".png", gif_write.convertgraphtoimage() );
    
    cout << New.height << ' ' << New.width << ' '<<negative_count<<' '<<check_negative<<'\n';
    // New.convertgraphtoimage();
    return New;
}

graph remove_h_seam(graph& g, int check_negative = 0)
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

    if(negative_count < check_negative)    return g;   //if no negative energy pixel exists, object has been removed!
    
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

    graph New, gif_write;
    New = g;
    gif_write = g;
    New.height = New.height - 1;
    cur = getptr(New, pos[m - 1], m - 1);
    for (i = m - 1; i >= 0; --i)
    {
        int Top = New.pixelarray[cur].top, Bottom = New.pixelarray[cur].bottom;
        gif_write.pixelarray[cur].val[0] = gif_write.pixelarray[cur].val[1] = 0;
        gif_write.pixelarray[cur].val[2] = 255;
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
    // imwrite( "./GIF/"+std::to_string(cnt++)+".png", gif_write.convertgraphtoimage() );
    cout << New.height << ' ' << New.width << ' '<<negative_count<<' '<<check_negative<<'\n';
    return New;
}

graph insert_h_seam(graph& g)
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
    cout << New.height << ' ' << New.width <<' '<<g.PENALTY<<'\n';
    // New.convertgraphtoimage();
    return New;
}

graph insert_v_seam(graph& g)
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
Mat rescale(const Mat& image, double r_height, double r_width)
{
    int r = abs((int)(image.size().height * (1 - r_height)));
    int c = abs((int)(image.size().width * (1 - r_width)));
    graph g(image);
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
                        g = insert_v_seam(g);
                    else
                        g = remove_v_seam(g);
                    // imwrite( "./GIF/"+std::to_string(cnt++)+".png", g.convertgraphtoimage() );
                }
                if(r_height > 1)
                    g = insert_h_seam(g);
                else
                    g = remove_h_seam(g);
                // imwrite( "./GIF/"+std::to_string(cnt++)+".png", g.convertgraphtoimage() );
                remaining += q;
            }
        }
        for (int i = 0; i < c; i++)
        {
            if(r_width > 1)
                g = insert_v_seam(g);
            else
                g = remove_v_seam(g);
            // imwrite( "./GIF/"+std::to_string(cnt++)+".png", g.convertgraphtoimage() );
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
                        g = insert_h_seam(g);
                    else
                        g = remove_h_seam(g);
                    // imwrite( "./GIF/"+std::to_string(cnt++)+".png", g.convertgraphtoimage() );
                }

                if(r_width > 1)
                    g = insert_v_seam(g);
                else
                    g = remove_v_seam(g);
                // imwrite( "./GIF/"+std::to_string(cnt++)+".png", g.convertgraphtoimage() );
                remaining += q;
            }
        }
        for (int i = 0; i < r; i++)
        {
            if(r_height > 1)
                g = insert_h_seam(g);
            else
                g = remove_h_seam(g);
            // imwrite( "./GIF/"+std::to_string(cnt++)+".png", g.convertgraphtoimage() );
        }
    }
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
                g.pixelarray[cur].penalty = 1e9;
            cur=g.pixelarray[cur].right;
        }
    }
    if(maxr == -1)  return g.convertgraphtoimage();
    int r=g.height, c = g.width;
    int r_orig = r, c_orig = c;
    while(true)
    {
        if(maxr-minr>=maxc-minc)
            g=remove_v_seam(g,1);
        else
            g=remove_h_seam(g,1);
        if(g.height == r && g.width == c)   break;
        r=g.height, c=g.width;
    }
    while(g.height < r_orig)
        g=insert_h_seam(g);
    while(g.width < c_orig)
        g=insert_v_seam(g);
    return g.convertgraphtoimage();
}
