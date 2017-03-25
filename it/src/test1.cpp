#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>
#include <iostream>

using namespace cv;
using namespace std;

int isValid(cv::Mat img, int x, int y)
{
  if (x < 0 || y < 0 || x >= img.rows || y >= img.cols)
    return 0;
  else return 1;
}

void imageCallback(const sensor_msgs::ImageConstPtr& msg)
{
  try
    {
      cv::imshow("view", cv_bridge::toCvShare(msg, "bgr8")->image);

      cv::Mat a = cv_bridge::toCvShare(msg, "bgr8")->image;

      // Mat b;

      // cv::fastNlMeansDenoisingColored(a,b,3,3,7,21);

      // imshow("view2",b);

      // int A[25],B[25],C[25];
      
      // for (int i = 2; i < a.rows - 2; i++)
      // 	{
      // 	  for (int j = 2; j < a.cols - 2; j++)
      // 	    {
      // 	      for (int m = -2,k = 0; m < 3; m++)
      // 		{
      // 		  for (int l = -2; l < 3; l++)
      // 		    {
      // 		      if (isValid(a, i + m, j + l))
      // 			{
      // 			  A[k++] = a.at<Vec3b>(i + m, j + l)[0];
      // 			  B[k++] = a.at<Vec3b>(i + m, j + l)[1];
      // 			  C[k++] = a.at<Vec3b>(i + m, j + l)[2];
      // 			}
      // 		    }
      // 		}

      // 	      std::sort(A, A + 25);
      // 	      std::sort(B, B + 25);
      // 	      std::sort(C, C + 25);
	      
      // 	      a.at<Vec3b>(i, j)[0] = A[12];
      // 	      a.at<Vec3b>(i, j)[1] = B[12];
      // 	      a.at<Vec3b>(i, j)[2] = C[12];

      // 	    }
      // 	}

      //cv::Mat b;

      // cv::cvtColor(a,b,CV_BGR2GRAY);

      // cv::imshow("view2",b);
      // // cout<<1;
      // Mat c;

      // adaptiveThreshold(b,c,255,ADAPTIVE_THRESH_GAUSSIAN_C,THRESH_BINARY,5,0);

      // imshow("Otsu",c);


      //      printf("%d \t  %d \n",a.rows,a.cols);
      // cv::cvtColor(a,b,CV_BGR2HSV);
      // cv::imshow("view1",b);
      cv::waitKey(1);
    }
  catch (cv_bridge::Exception& e)
    {
      ROS_ERROR("Could not convert from '%s' to 'bgr8'.", msg->encoding.c_str());
    }
}

int main(int argc, char **argv)
{
  ros::init(argc, argv, "test1");
  ros::NodeHandle nh;
  cv::namedWindow("view");
  cv::startWindowThread();
  image_transport::ImageTransport it(nh);
  image_transport::Subscriber sub = it.subscribe("kraken/front_camera", 1, imageCallback);
  ros::spin();
  cv::destroyWindow("view");
}
