#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>
#include <stdio.h>
#include <iostream>

using namespace cv;
using namespace std;

int isValid(Mat img, int x, int y)
{
  if (x < 0 || y < 0 || x >= img.rows || y >= img.cols)
    return 0;
  else return 1;
}

void erosion(Mat img1, Mat img2)
{
  int i, j, k, l;

  for (i = 0; i < img1.rows; i++)
    {
      for (j = 0; j < img1.cols; j++)
	{
	  if (img1.at<uchar>(i, j) == 255)
	    {
	      for (k = -1; k < 2; k++)
		{
		  for (l = -1; l < 2; l++)
		    {
		      if (isValid(img1, i + k, j + l))
			{
			  if (img1.at<uchar>(i + k, j + l) == 0)
			    {
			      img2.at<uchar>(i, j) = 0;
			      break;
			    }
			}
		    }
		}
	    }
	}
    }
}

void dilation(Mat img1, Mat img2)
{
  int i, j, k, l;

  for (i = 0; i < img1.rows; i++)
    {
      for (j = 0; j < img1.cols; j++)
	{
	  if (img1.at<uchar>(i, j) == 0)
	    {
	      for (k = -1; k < 2; k++)
		{
		  for (l = -1; l < 2; l++)
		    {
		      if (isValid(img1, i + k, j + l))
			{
			  if (img1.at<uchar>(i + k, j + l) == 255)
			    {
			      img2.at<uchar>(i, j) = 255;
			      break;
			    }
			}
		    }
		}
	    }
	}
    }
}

void imageCallback(const sensor_msgs::ImageConstPtr& msg)
{
  try
    {

      vector< vector<Point> > contours;
      vector<Vec4i>hierarchy;

      //imshow("view", cv_bridge::toCvShare(msg, "bgr8")->image);
      Mat a = (cv_bridge::toCvShare(msg, "bgr8")->image);

      imshow("Original_Video",a);

      Mat f = Mat(a.rows,a.cols,CV_8UC1,Scalar(0));

      cvtColor(a,f,COLOR_BGR2HSV);
 
      Mat l;
      
      inRange(f,Scalar(0, 100, 100),Scalar(10, 255, 255), l);

      //imshow("HSV_Video",f);

      Mat b_gray = l.clone();

      // Mat g = b_gray.clone();
      // erosion(b_gray,g);

      // Mat h = g.clone();
      // dilation(g,h);

      // g = h.clone();
      // dilation(h,g);
      
      // b_gray = g.clone();
      // erosion(g,b_gray);

      //imshow("Post_Colour_Extraction",b_gray);

      Mat c;

      int x=80,y=100;

      Canny(b_gray,c,x,y,3);
      //imshow("Canny_Video",c);

      findContours(c,contours,hierarchy,CV_RETR_LIST,CV_CHAIN_APPROX_NONE);

      Mat d = Mat(c.rows,c.cols,CV_8UC1,Scalar(0));

      for(int s=0;s<contours.size();s++)
	{
	  for(int t=0;t<contours[s].size();t++)
	    {
       	      d.at<uchar>(contours[s][t].y,contours[s][t].x)=255;
	    }
        }

      //imshow("Contours_Video",d);

      Mat poly= Mat(d.rows,d.cols,CV_8UC1,Scalar(0));
      
      for(int i=0;i<contours.size();i++)
	approxPolyDP(contours[i],contours[i],10,true);

      RNG rng(12345);
      for(int i=0;i<contours.size();i++)
	{
          Scalar color = Scalar(rng.uniform(0,255),rng.uniform(0,255),rng.uniform(0,255));
      
          drawContours(poly,contours,i,color,2,8,hierarchy,0,Point());

	}


      int sum_x=0,sum_y=0,count=0;

      for(int s=0;s<f.rows;s++)
	{
	  for(int t=0;t<f.cols;t++)
	    {
	      if(b_gray.at<uchar>(s,t) == 255)
		{
		  sum_x += t;
		  sum_y += s;
		  count++;
		}
	    }
	}
	
      sum_x /= count;
      sum_y /= count;

      Mat m = Mat(l.rows,l.cols,CV_8UC3,Scalar(0,0,0));

      for(int s=0;s<f.rows;s++)
	{
	  for(int t=0;t<f.cols;t++)
	    {
	      if(poly.at<uchar>(s,t) != 0)
		{

		  m.at<Vec3b>(s,t)[0] = 255;
		  m.at<Vec3b>(s,t)[1] = 255;
		  m.at<Vec3b>(s,t)[2] = 255;
		}
	    }
	}

      if(count!=0)
	{
	  for(int x=-3;x<4;x++)
	    {
	      for(int y=-3;y<4;y++)
		{

		  m.at<Vec3b>(sum_y+x,sum_x+y)[0] = 0;
		  m.at<Vec3b>(sum_y+x,sum_x+y)[1] = 255;	    
		  m.at<Vec3b>(sum_y+x,sum_x+y)[2] = 255;
		}
	    }
	  printf("Centre of Marker is (%d,%d) \n",sum_x,sum_y);
	}
      
      imshow("Polygon_Video",m);

      waitKey(1);
    }
  catch (cv_bridge::Exception& e)
    {
      ROS_ERROR("Could not convert from '%s' to 'bgr8'.", msg->encoding.c_str());
    }
}

int main(int argc, char **argv)
{
  ros::init(argc, argv, "sub1");
  ros::NodeHandle nh;
  // cv::namedWindow("Original_Video");
  // cv::namedWindow("HSV_Video");
  // cv::namedWindow("Post_Colour_Extraction");
  // cv::namedWindow("Canny_Video");
  // cv::namedWindow("Contour_Video");
  // cv::namedWindow("Polygon_Video");
  cv::startWindowThread();
  image_transport::ImageTransport it(nh);
  image_transport::Subscriber sub = it.subscribe("camera/image", 1, imageCallback);
  ros::spin();
  // cv::destroyWindow("view");
  // cv::destroyWindow("view");
  cv::destroyWindow("Original_Video");
  // cv::destroyWindow("HSV_Video");
  // cv::destroyWindow("Post_Colour_Extraction");
  // cv::destroyWindow("Canny_Video");
  // cv::destroyWindow("Contours_Video");
  cv::destroyWindow("Polygon_Video");
}
