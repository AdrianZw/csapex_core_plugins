/// HEADER
#include "register_plugin.h"

/// COMPONENT
#include <csapex_transform/transform_message.h>
#include "listener.h"

/// PROJECT
#include <csapex/manager/connection_type_manager.h>
#include <csapex/model/tag.h>
#include <csapex_ros/ros_message_conversion.h>

/// SYSTEM
#include <csapex/utility/register_apex_plugin.h>

CSAPEX_REGISTER_CLASS(csapex::RegisterTransformPlugin, csapex::CorePlugin)

using namespace csapex;

RegisterTransformPlugin::RegisterTransformPlugin()
{
}

void RegisterTransformPlugin::init(CsApexCore& core)
{
    Tag::createIfNotExists("Transform");
    Tag::createIfNotExists("Time");

    ConnectionTypeManager::registerMessage<connection_types::TransformMessage>();

    ROSHandler::instance().registerConnectionCallback(boost::bind(&Listener::start));
}
