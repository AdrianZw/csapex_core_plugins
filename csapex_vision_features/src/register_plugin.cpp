/// HEADER
#include "register_plugin.h"

/// COMPONENT
#include <csapex_vision_features/keypoint_message.h>
#include <csapex_vision_features/descriptor_message.h>

/// PROJECT
#include <csapex/manager/connection_type_manager.h>
#include <csapex/model/tag.h>

/// SYSTEM
#include <csapex/utility/register_apex_plugin.h>

CSAPEX_REGISTER_CLASS(csapex::RegisterVisionFeaturePlugin, csapex::CorePlugin)

using namespace csapex;

RegisterVisionFeaturePlugin::RegisterVisionFeaturePlugin()
{
}

void RegisterVisionFeaturePlugin::init(CsApexCore& core)
{
    Tag::createIfNotExists("Features");

    ConnectionTypeManager::registerMessage<connection_types::KeypointMessage>();
    ConnectionTypeManager::registerMessage<connection_types::DescriptorMessage>();
}
