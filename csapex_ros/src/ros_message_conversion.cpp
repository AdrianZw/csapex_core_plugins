/// HEADER
#include <csapex_ros/ros_message_conversion.h>

/// PROJECT
#include <csapex/model/connector_out.h>

using namespace csapex;

RosMessageConversion::RosMessageConversion()
{
}

bool RosMessageConversion::canHandle(const ros::master::TopicInfo &topic)
{
    return converters_.find(topic.datatype) != converters_.end();
}

void RosMessageConversion::doRegisterConversion(Convertor::Ptr c)
{
    converters_[c->rosType()] = c;
    converters_inv_[c->apexType()] = c;
}

ros::Subscriber RosMessageConversion::subscribe(const ros::master::TopicInfo &topic, int queue, ConnectorOut* output)
{
    return converters_.at(topic.datatype)->subscribe(topic, queue, output);
}

ros::Publisher RosMessageConversion::advertise(ConnectionType::Ptr type, const std::string &topic, int queue, bool latch)
{
    return converters_inv_.at(type->name())->advertise(topic, queue, latch);
}

void RosMessageConversion::publish(ros::Publisher &pub, ConnectionType::Ptr msg)
{
    converters_inv_.at(msg->name())->publish(pub, msg);
}



void Convertor::publish_apex(ConnectorOut *output, ConnectionType::Ptr msg)
{
    output->setType(msg);
    output->publish(msg);
}
