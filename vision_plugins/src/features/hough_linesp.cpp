/// HEADER
#include "hough_linesp.h"

/// PROJECT
#include <csapex/utility/register_apex_plugin.h>
#include <csapex/msg/input.h>
#include <csapex/msg/output.h>
#include <utils_param/parameter_factory.h>
#include <csapex_vision/cv_mat_message.h>

using namespace csapex;
using namespace csapex::connection_types;
using namespace vision_plugins;

CSAPEX_REGISTER_CLASS(vision_plugins::HoughLinesP, csapex::Node)

//double rho, double theta, int threshold, double minLineLength=0, double maxLineGap=0

HoughLinesP::HoughLinesP() :
    rho_(1.),
    theta_(CV_PI),
    threshold_(80),
    min_line_length_(30),
    max_line_gap_(10)
{
}

void HoughLinesP::process()
{
    CvMatMessage::Ptr in = input_->getMessage<connection_types::CvMatMessage>();

    if(in->getEncoding() != enc::mono) {
        throw std::runtime_error("image must be grayscale.");
    }

    CvMatMessage::Ptr out(new connection_types::CvMatMessage(enc::bgr));
    cv::cvtColor(in->value, out->value, CV_GRAY2BGR);

    std::vector<cv::Vec4i> lines;
    cv::HoughLinesP(in->value, lines, rho_, theta_/180, threshold_, min_line_length_, max_line_gap_);

    for( size_t i = 0; i < lines.size(); i++ ) {
        cv::line(out->value, cv::Point(lines[i][0], lines[i][1]),
                cv::Point(lines[i][2], lines[i][3]), cv::Scalar(0,0,255), 3, 8 );
    }

    output_->publish(out);
}

void HoughLinesP::setup()
{
    CornerLineDetection::setup();
    update();
}

void HoughLinesP::setupParameters()
{
    addParameter(param::ParameterFactory::declareRange("rho", 0.1, 100.0, 1.0, 1.0),
                 boost::bind(&HoughLinesP::update, this));
    addParameter(param::ParameterFactory::declareRange("theta", 0.1, 2 * CV_PI, CV_PI, 0.1),
                 boost::bind(&HoughLinesP::update, this));
    addParameter(param::ParameterFactory::declareRange("threshold", 1, 500, 80, 1),
                 boost::bind(&HoughLinesP::update, this));
    addParameter(param::ParameterFactory::declareRange("min length", 0.0, 1000.0, 30.0, 1.0),
                 boost::bind(&HoughLinesP::update, this));
    addParameter(param::ParameterFactory::declareRange("max gap", 0.0, 400.0, 10.0, 1.0),
                 boost::bind(&HoughLinesP::update, this));

}

void HoughLinesP::update()
{
    rho_ = readParameter<double>("rho");
    theta_ = readParameter<double>("theta");
    threshold_ = readParameter<int>("threshold");
    min_line_length_ = readParameter<double>("min length");
    max_line_gap_ = readParameter<double>("max gap");
}


