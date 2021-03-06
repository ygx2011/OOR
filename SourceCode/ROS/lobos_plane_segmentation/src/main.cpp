#include <ros/ros.h>
#include <sensor_msgs/PointCloud2.h>
#include <pcl/conversions.h>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>

#include <lobos_cloud_pubsub/CloudPublisher.hpp>
#include <lobos_cloud_pubsub/RangeImageSubscriber.hpp>
#include <string>
#include <iostream>

#include "rangeImage_utils.hpp"

int main (int argc, char **argv) {

    // ROS initialization
    ros::init (argc, argv, "lobos_plane_extraction");
    ros::NodeHandle nh;
    CloudPublisher<pcl::PointXYZRGB>  my_cloudPublisher (nh, std::string("planes_segmented"));
    RangeImageSubscriber my_rangeImageSubscriber (nh, std::string("/camera/depth/image_rect"), std::string("/camera/depth/camera_info"));

    ros::AsyncSpinner spinner(1); 
    spinner.start();
    ros::Rate r(1);
    while (ros::ok()) {

        if (my_rangeImageSubscriber.getIsThereNewData()) {
            pcl::RangeImage ri = my_rangeImageSubscriber.getCurrentRangeImage();
            std::cout << "range image size " << ri.height << " " << ri.width << std::endl;
	        pcl::PointCloud<pcl::PointXYZRGB> segmentesPlanes = computePlaneExtraction(ri.makeShared());	
            std::cout << "Cloud Size: " << segmentesPlanes.size() <<std::endl;
            std::cout << "Cloud computed " << std::endl;

            my_cloudPublisher.publishPointcloud(segmentesPlanes);

        } else {
            std::cout << "Waiting for data to be published" << std::endl;
        }

        r.sleep();
    }




    ros::waitForShutdown();
    return 0;
}
