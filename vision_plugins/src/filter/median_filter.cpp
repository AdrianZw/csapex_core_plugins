#include "median_filter.h"

/// PROJECT
#include <csapex/utility/register_apex_plugin.h>
#include <csapex/msg/input.h>
#include <csapex/msg/output.h>
#include <utils_param/parameter_factory.h>
#include <csapex_vision/cv_mat_message.h>
#include <csapex/model/node_modifier.h>

CSAPEX_REGISTER_CLASS(vision_plugins::MedianFilter, csapex::Node)

using namespace csapex;
using namespace csapex::connection_types;
using namespace vision_plugins;

MedianFilter::MedianFilter() :
    kernel_size_(3)
{
}

void MedianFilter::process()
{
    CvMatMessage::ConstPtr in = input_->getMessage<connection_types::CvMatMessage>();
    CvMatMessage::Ptr out(new connection_types::CvMatMessage(in->getEncoding(), in->stamp_micro_seconds));

    if(in->value.channels() > 4)
        throw std::runtime_error("To many channels!");

    cv::medianBlur(in->value, out->value, kernel_size_);
    output_->publish(out);
}

void MedianFilter::setup()
{
    input_ = modifier_->addInput<CvMatMessage>("original");
    output_ = modifier_->addOutput<CvMatMessage>("filtered");
    update();
}

void MedianFilter::setupParameters()
{
    addParameter(param::ParameterFactory::declareRange("kernel", 3, 31, kernel_size_, 2),
                 std::bind(&MedianFilter::update, this));
}

void MedianFilter::update()
{
    kernel_size_ = readParameter<int>("kernel");
}
