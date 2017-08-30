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
    string topic_name = "test_topic";
    model_pub = nh->advertise<std_msgs::String>(topic_name, 1, true);
}

void LogicalCameraPlugin::OnUpdate(){
    msgs::LogicalCameraImage logical_image;

    logical_image = this->parentSensor->Image();
    stringstream string_msg;
    msgs::Vector3d cameraPosition = logical_image.pose().position();
    msgs::Quaternion cameraOrientation = logical_image.pose().orientation();

    string_msg << "Camera" << endl;
    string_msg << "Position: " << cameraPosition.x()
         << " - " << cameraPosition.y()
         << " - " << cameraPosition.z() << endl;
    string_msg << "Orientation: " << cameraOrientation.x()
         << " - " << cameraOrientation.y()
         << " - " << cameraOrientation.z()
         << " - " << cameraOrientation.w() << endl;

    int number_of_models = logical_image.model_size();
    string_msg << "there are " << number_of_models << " models:" << endl;
    for(int i=0; i < number_of_models; i++){
        string_msg << logical_image.model(i).name() << endl;
    }

    std_msgs::String msg;
    msg.data = string_msg.str();
    model_pub.publish(msg);

}
