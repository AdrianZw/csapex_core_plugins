/// HEADER
#include "filter_debayer.h"

/// PROJECT
#include <utils_param/parameter_factory.h>
#include <csapex/msg/io.h>
#include <csapex/model/node_modifier.h>
#include <csapex_vision/cv_mat_message.h>

/// SYSTEM
#include <csapex/utility/register_apex_plugin.h>
#include <boost/assign/list_of.hpp>

CSAPEX_REGISTER_CLASS(vision_plugins::Debayer, csapex::Node)

using namespace vision_plugins;
using namespace csapex;
using namespace connection_types;

Debayer::Debayer()
{
}

void Debayer::setup(NodeModifier &node_modifier)
{
    input_img_ = node_modifier.addInput<CvMatMessage>("Image");
    output_img_ = node_modifier.addOutput<CvMatMessage>("Image");
}

void Debayer::setupParameters(Parameterizable& parameters)
{
    std::map<std::string, int> methods = boost::assign::map_list_of
            ("BayerBG2RGB", (int) CV_BayerBG2RGB)
            ("BayerGB2RGB", (int) CV_BayerGB2RGB)
            ("BayerRG2RGB", (int) CV_BayerRG2RGB)
            ("BayerGR2RGB", (int) CV_BayerGR2RGB)
            ("NNRG2RGB", 667);

    parameters.addParameter(param::ParameterFactory::declareParameterSet("method", methods, (int) CV_BayerBG2RGB));
}

void Debayer::process(csapex::Parameterizable& parameters)
{
    CvMatMessage::ConstPtr img_msg = msg::getMessage<CvMatMessage>(input_img_);
    const cv::Mat& img = img_msg->value;

    int mode = readParameter<int>("method");

    // assume 1 channel raw image comes in
    cv::Mat raw;
    if(img.channels() == 1) {
        raw = img;
    } else {
        cv::cvtColor(img, raw, CV_RGB2GRAY);
    }

    CvMatMessage::Ptr img_out(new CvMatMessage(enc::bgr, img_msg->stamp_micro_seconds));
    if (mode == 667) {
        debayerAndResize(raw, img_out->value);
        cv::cvtColor(img_out->value, img_out->value, CV_BGR2RGB);
    }
    else {
        cv::cvtColor(raw, img_out->value, mode);
    }
    msg::publish(output_img_, img_out);
}

bool Debayer::usesMask()
{
    return false;
}

// Debayer: bayer-Pattern
// - every pixel has it's own color filter (e.g. only sees red)
// - pixel returns brightness value
void Debayer::debayerAndResize(const cv::Mat& source, cv::Mat& dest) {

    cv::MatConstIterator_<uchar> it = source.begin<uchar>(),
            itEnd = source.end<uchar>();
    uchar* destination = (uchar*) dest.data;

    while(it != itEnd) {
        // r g r g r g
        // g b g b g b
        cv::MatConstIterator_<uchar> itLineEnd = it + 640;
        while(it != itLineEnd) {
            *destination = *it;
            ++it;
            ++destination;
            *destination = *it;
            ++it;
            ++destination;
            ++destination;
        }
        itLineEnd = it + 640;
        destination -= 320*3;
        while(it != itLineEnd) {
            // maybe add some green
            ++it;
            ++destination;
            ++destination;
            // add blue
            *destination = *it;
            ++it;
            ++destination;
        }
        destination += 320*3;
    }
}

