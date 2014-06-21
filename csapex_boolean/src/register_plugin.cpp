/// HEADER
#include "register_plugin.h"

/// PROJECT
#include <csapex/model/tag.h>
#include <csapex/model/message_factory.h>

/// SYSTEM
#include <csapex/utility/register_apex_plugin.h>

CSAPEX_REGISTER_CLASS(csapex::boolean::RegisterPlugin, csapex::CorePlugin)

using namespace csapex;
using namespace boolean;

RegisterPlugin::RegisterPlugin()
{
}


void RegisterPlugin::init(CsApexCore& core)
{
    Tag::createIfNotExists("Boolean");
}
