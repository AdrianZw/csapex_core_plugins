/// HEADER
#include "pointcloud_generator.h"

/// PROJECT
#include <csapex/param/parameter_factory.h>

/// SYSTEM
#include <csapex/utility/register_apex_plugin.h>

CSAPEX_REGISTER_CLASS(csapex::PointCloudGenerator, csapex::Node)

using namespace csapex;
using namespace csapex::connection_types;


PointCloudGenerator::PointCloudGenerator()
{
}

void PointCloudGenerator::process()
{

}

void PointCloudGenerator::setup(NodeModifier& node_modifier)
{
}
