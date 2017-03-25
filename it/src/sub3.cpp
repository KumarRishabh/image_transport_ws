#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>
#include <stdio.h>
#include <iostream>
#include "std_msgs/MultiArrayLayout.h"
#include "std_msgs/MultiArrayDimension.h"
#include "std_msgs/Int32MultiArray.h"

using namespace cv;
using namespace std;

int VH=98,SH=255,HH=114,VL=84,SL=0,HL=86;


// int isValid(Mat img, int x, int y)
// {
//   if (x < 0 || y < 0 || x >= img.rows || y >= img.cols)
//     return 0;
//   else return 1;
// }

// void erosion(Mat img1, Mat img2)
// {
//   int i, j, k, l;

//   for (i = 0; i < img1.rows; i++)
//     {
//       for (j = 0; j < img1.cols; j++)
// 	{
// 	  if (img1.at<uchar>(i, j) == 255)
// 	    {
// 	      for (k = -1; k < 2; k++)
// 		{
// 		  for (l = -1; l < 2; l++)
// 		    {
// 		      if (isValid(img1, i + k, j + l))
// 			{
// 			  if (img1.at<uchar>(i + k, j + l) == 0)
// 			    {
// 			      img2.at<uchar>(i, j) = 0;
// 			      break;
// 			    }
// 			}
// 		    }
// 		}
// 	    }
// 	}
//     }
// }

// void dilation(Mat img1, Mat img2)
// {
//   int i, j, k, l;

//   for (i = 0; i < img1.rows; i++)
//     {
//       for (j = 0; j < img1.cols; j++)
// 	{
// 	  if (img1.at<uchar>(i, j) == 0)
// 	    {
// 	      for (k = -1; k < 2; k++)
// 		{
// 		  for (l = -1; l < 2; l++)
// 		    {
// 		      if (isValid(img1, i + k, j + l))
// 			{
// 			  if (img1.at<uchar>(i + k, j + l) == 255)
// 			    {
// 			      img2.at<uchar>(i, j) = 255;
// 			      break;
// 			    }
// 			}
// 		    }
// 		}
// 	    }
// 	}
//     }
// }


void imageCallback(const sensor_msgs::ImageConstPtr& msg)
{
  try
    {
      //imshow("view", cv_bridge::toCvShare(msg, "bgr8")->image);
      Mat a = (cv_bridge::toCvShare(msg, "bgr8")->image);

      Mat b,c,d,e,f,h;
      int z,w;



      cvtColor(a,b,CV_BGR2HSV);

      cv::imshow("Original_Video",a);

      Mat g(a.rows,a.cols,CV_8UC1,Scalar(0));

      for(int x=0;x<a.rows;x++)
	{
	  for(int y=0;y<a.cols;y++)
	    {
	      if(b.at<Vec3b>(x,y)[0] <= HH && b.at<Vec3b>(x,y)[0] >= HL && b.at<Vec3b>(x,y)[1] <= SH && b.at<Vec3b>(x,y)[1] >= SL && b.at<Vec3b>(x,y)[2] <= VH && b.at<Vec3b>(x,y)[2] >= VL)
		{
		  g.at<uchar>(x,y) = 255;
		}
	    }
	}

      // c = g.clone();
      // erosion(g,c);

      // f = c.clone();
      // dilation(c,f);

      // e = d.clone();
      // dilation(d,e);

      // g = e.clone();
      // erosion(e,g);

      int sum_x=0,sum_y=0,count=0;

      for(int x=0;x<g.rows;x++)
	{
	  for(int y=0;y<g.cols;y++)
	    {
	      if(g.at<uchar>(x,y) == 255)
		{
		  sum_x += y;
		  sum_y += x;
		  count++;
		}
	    }
	}


      if(count!=0)
	{
	  std::printf("Centre of Red Buoy is (%d,%d) \n",sum_y,sum_x);
	  sum_x /= count;
	  sum_y /= count;
	}
      h = Mat(g.rows,g.cols,CV_8UC3,Scalar(0,0,0));

      for(int x=0;x<g.rows;x++)
	{
	  for(int y=0;y<g.cols;y++)
	    {
	      if(g.at<uchar>(x,y) == 255)
		{

		  h.at<Vec3b>(x,y)[0] = 255;
		  h.at<Vec3b>(x,y)[1] = 255;
		  h.at<Vec3b>(x,y)[2] = 255;
		}
	    }
	}

      if(count!=0)
	{
	  for(int x=-3;x<4;x++)
	    {
	      for(int y=-3;y<4;y++)
		{
		  h.at<Vec3b>(sum_y+x,sum_x+y)[0] = 0;
		  h.at<Vec3b>(sum_y+x,sum_x+y)[1] = 0;
		}
	    }
	}
      cv::imshow("Final_Video",h);

      int MAX = -999, rad;

      for(int x=0;x<g.rows;x++)
	{
	  for(int y=0;y<g.cols;y++)
	    {
	      if(g.at<uchar>(x,y) == 255)
		{

		  rad = ((y-sum_x)*(y-sum_x) + (x-sum_y)*(x-sum_y));
		  if(rad > MAX);
		  MAX = rad;
		}
	    }
	}

      std::printf("Radius of Red Buoy = %d \n",(int)sqrt(MAX));

      waitKey(10);
    }

catch (cv_bridge::Exception& e)
  {
    ROS_ERROR("Could not convert from '%s' to 'bgr8'.", msg->encoding.c_str());
  }
}
void hsvcallback(const std_msgs::Int32MultiArray::ConstPtr &array)
{
    HH=array->data[0];
    HL=array->data[1];
    SH=array->data[2];
    SL=array->data[3];
    VH=array->data[4];
    VL=array->data[5];
    ROS_INFO("%d,%d,%d,%d,%d,%d",HH,HL,SH,SL,VH,VL);
}

int main(int argc, char **argv)
{
  ros::init(argc, argv, "sub1");
  ros::NodeHandle nh;
  // cv::namedWindow("Original_Video");
  // cv::namedWindow("Final_Video");
  namedWindow("Thresholds",WINDOW_AUTOSIZE);
  createTrackbar("H_High","Thresholds",&HH,255);
  createTrackbar("H_Low","Thresholds",&HL,255);
  createTrackbar("S_High","Thresholds",&SH,255);
  createTrackbar("S_Low","Thresholds",&SL,255);
  createTrackbar("V_High","Thresholds",&VH,255);
  createTrackbar("V_Low","Thresholds",&VL,255);

  cv::startWindowThread();
  image_transport::ImageTransport it(nh);
  image_transport::Subscriber sub = it.subscribe("/kraken/front_camera", 1, imageCallback);
  ros::Subscriber sub1 = nh.subscribe("/HSV",1,hsvcallback);
  ros::spin();
  // cv::destroyWindow("view");
  // cv::destroyWindow("view");
  cv::destroyWindow("Original_Video");
  cv::destroyWindow("Final_Video");
}
