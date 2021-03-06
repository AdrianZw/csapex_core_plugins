/// HEADER
#include "transform_to_odometry.h"

/// COMPONENT
#include <csapex_transform/transform_message.h>

/// PROJECT
#include <csapex/model/node_modifier.h>
#include <csapex/msg/generic_pointer_message.hpp>
#include <csapex/msg/io.h>
#include <csapex/param/parameter_factory.h>
#include <csapex/utility/register_apex_plugin.h>
#include <csapex_ros/ros_message_conversion.h>
#include <csapex_ros/yaml_io.hpp>

/// SYSTEM
#include <nav_msgs/Odometry.h>
// clang-format off
#include <csapex/utility/suppress_warnings_start.h>
#include <tf/tf.h>
#include <csapex/utility/suppress_warnings_end.h>
// clang-format on

CSAPEX_REGISTER_CLASS(csapex::TransformToOdometry, csapex::Node)

using namespace csapex;
using namespace csapex::connection_types;

TransformToOdometry::TransformToOdometry()
{
}

void TransformToOdometry::setupParameters(Parameterizable& parameters)
{
}

void TransformToOdometry::setup(NodeModifier& node_modifier)
{
    in_ = node_modifier.addInput<connection_types::TransformMessage>("TF");
    out_ = node_modifier.addOutput<nav_msgs::Odometry>("Odometry");
}

void TransformToOdometry::process()
{
    connection_types::TransformMessage::ConstPtr a = msg::getMessage<connection_types::TransformMessage>(in_);

    nav_msgs::Odometry::Ptr msg(new nav_msgs::Odometry);
    msg->header.frame_id = a->frame_id;
    msg->header.stamp = msg->header.stamp.fromNSec(a->stamp_micro_seconds * 1e3);
    tf::poseTFToMsg(a->value, msg->pose.pose);

    msg::publish<nav_msgs::Odometry>(out_, msg);
}
