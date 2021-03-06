#include "logical_camera_plugin.h"

using namespace gazebo;
using namespace std;
using namespace ros;

GZ_REGISTER_SENSOR_PLUGIN(LogicalCameraPlugin);

void LogicalCameraPlugin::Load(sensors::SensorPtr _sensor, sdf::ElementPtr _sdf){
    // Get the parent sensor.
    this->parentSensor = std::dynamic_pointer_cast<sensors::LogicalCameraSensor>(_sensor);

    // Make sure the parent sensor is valid.
    if (!this->parentSensor){
        gzerr << "LogicalCameraPlugin requires a LogicalCameraSensor.\n";
        return;
    }

    // Connect to the sensor update event.
    this->updateConnection = this->parentSensor->ConnectUpdated(std::bind(&LogicalCameraPlugin::OnUpdate, this));

    // Make sure the parent sensor is active.
    this->parentSensor->SetActive(true);

    ROS_INFO("LogicalCameraPlugin correctly loaded!!!");
    ROS_INFO("_near:=%g",this->parentSensor->Near());
    ROS_INFO("_far:=%g",this->parentSensor->Far());
    ROS_INFO("_horizontalFOV:=%g",this->parentSensor->HorizontalFOV());
    ROS_INFO("_aspect_ratio:=%g",this->parentSensor->AspectRatio());

    nh = new ros::NodeHandle("~");
    image_pub = nh->advertise<simulation_plugins::LogicalCameraImage>("logical_camera_image", 1, true);
}

void LogicalCameraPlugin::OnUpdate(){
    msgs::LogicalCameraImage logical_image;
    simulation_plugins::LogicalCameraImage msg;

    logical_image = this->parentSensor->Image();

    msg.pose.position.x = logical_image.pose().position().x();
    msg.pose.position.y = logical_image.pose().position().y();
    msg.pose.position.z = logical_image.pose().position().z();

    msg.pose.orientation.x = logical_image.pose().orientation().x();
    msg.pose.orientation.y = logical_image.pose().orientation().y();
    msg.pose.orientation.z = logical_image.pose().orientation().z();
    msg.pose.orientation.w = logical_image.pose().orientation().w();

    int number_of_models = logical_image.model_size();
    for(int i=0; i < number_of_models; i++){
        simulation_plugins::Model model_msg;
        model_msg.pose.position.x = logical_image.model(i).pose().position().x();
        model_msg.pose.position.y = logical_image.model(i).pose().position().y();
        model_msg.pose.position.z = logical_image.model(i).pose().position().z();

        model_msg.pose.orientation.x = logical_image.model(i).pose().orientation().x();
        model_msg.pose.orientation.y = logical_image.model(i).pose().orientation().y();
        model_msg.pose.orientation.z = logical_image.model(i).pose().orientation().z();
        model_msg.pose.orientation.w = logical_image.model(i).pose().orientation().w();

        model_msg.type = logical_image.model(i).name();

        msg.models.push_back(model_msg);
    }

    this->image_pub.publish(msg);
}
