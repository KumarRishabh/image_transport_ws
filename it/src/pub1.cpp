#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>
#include <sstream> // for converting the command line parameter to integer

int main(int argc, char** argv)
{
  ros::init(argc, argv, "pub1");
  ros::NodeHandle nh;
  image_transport::ImageTransport it(nh);
  image_transport::Publisher pub = it.advertise("camera/image", 1);

  cv::VideoCapture vid(argv[1]);
  cv::Mat frame;
  sensor_msgs::ImagePtr msg;

  for(int i=0;i<vid.get(CV_CAP_PROP_FRAME_COUNT);i++)
    {
      //ros::Rate loop_rate(1000);
      while (nh.ok()) {
	vid >> frame;
	// Check if grabbed frame is actually full with some content
	if(!frame.empty()) {
	  for(int j=0;j<180;j++)
	    {
	      msg = cv_bridge::CvImage(std_msgs::Header(), "bgr8", frame).toImageMsg();
	      pub.publish(msg);
	      cv::waitKey(20);
	    }
	}
      }
      ros::spinOnce();
      //loop_rate.sleep();
    }
}

