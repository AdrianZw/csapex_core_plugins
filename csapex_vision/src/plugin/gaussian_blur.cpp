/// HEADER
#include "gaussian_blur.h"

/// PROJECT
#include <csapex/msg/input.h>
#include <csapex/msg/output.h>
#include <csapex_vision/cv_mat_message.h>
#include <utils_param/parameter_factory.h>
#include <csapex/model/node_modifier.h>
#include <csapex/utility/register_apex_plugin.h>


CSAPEX_REGISTER_CLASS(csapex::GaussianBlur, csapex::Node)

using namespace csapex;
using namespace csapex::connection_types;

GaussianBlur::GaussianBlur() :
    kernel_(1),
    sigma_x_(0.1),
    sigma_y_(0.0)
{
    addParameter(param::ParameterFactory::declareRange("kernel", 1, 255, kernel_, 2),
                 boost::bind(&GaussianBlur::update, this));
    addParameter(param::ParameterFactory::declareRange("sigma x", 0.1, 128.0, sigma_x_, 0.1),
                 boost::bind(&GaussianBlur::update, this));
    addParameter(param::ParameterFactory::declareRange("sigma y", 0.0, 128.0, sigma_y_, 0.1),
                 boost::bind(&GaussianBlur::update, this));


}

void GaussianBlur::process()
{
    CvMatMessage::Ptr in = input_->getMessage<connection_types::CvMatMessage>();
    CvMatMessage::Ptr out(new connection_types::CvMatMessage(in->getEncoding()));

    cv::GaussianBlur(in->value, out->value, cv::Size(kernel_, kernel_), sigma_x_, sigma_y_);
    output_->publish(out);
}

void GaussianBlur::setup()
{
    input_ = modifier_->addInput<CvMatMessage>("Unblurred");
    output_ = modifier_->addOutput<CvMatMessage>("Blurred");

    update();
}

void GaussianBlur::update()
{
    kernel_  = readParameter<int>("kernel");
    sigma_x_ = readParameter<double>("sigma x");
    sigma_y_ = readParameter<double>("sigma y");
}
