/// HEADER
#include "hough_circle.h"

/// COMPONENT
#include <csapex_vision/cv_mat_message.h>

/// PROJECT
#include <csapex/msg/input.h>
#include <csapex/msg/output.h>
#include <utils_param/parameter_factory.h>
#include <csapex/model/node_modifier.h>
#include <csapex/utility/register_apex_plugin.h>

CSAPEX_REGISTER_CLASS(csapex::HoughCircle, csapex::Node)

using namespace csapex;
using namespace csapex::connection_types;

HoughCircle::HoughCircle()
{
    std::vector< std::pair<std::string, int> > methods;
    methods.push_back(std::make_pair("CV_HOUGH_GRADIENT", (int) CV_HOUGH_GRADIENT));
    methods.push_back(std::make_pair("CV_HOUGH_STANDARD", (int) CV_HOUGH_STANDARD));
    methods.push_back(std::make_pair("CV_HOUGH_PROBABILISTIC", (int) CV_HOUGH_PROBABILISTIC));
    methods.push_back(std::make_pair("CV_HOUGH_MULTI_SCALE", (int) CV_HOUGH_MULTI_SCALE));
    addParameter(param::ParameterFactory::declareParameterSet("method", methods));

    addParameter(param::ParameterFactory::declare<double>("dp", 0.01, 10.00, 1.0, 0.01));
    addParameter(param::ParameterFactory::declare<double>("minDist", 0.0, 800.0, 100.0, 0.1));
    addParameter(param::ParameterFactory::declare<double>("param1", 0.00, 500.0, 200.0, 0.1));
    addParameter(param::ParameterFactory::declare<double>("param2", 0.00, 500.0, 100.0, 0.1));
    addParameter(param::ParameterFactory::declare<int>("minRadius", 0, 100, 0, 1));
    addParameter(param::ParameterFactory::declare<int>("maxRadius", 0, 100, 0, 1));
}

void HoughCircle::setup()
{
    input_ = modifier_->addInput<CvMatMessage>("Image");
    output_ = modifier_->addOutput<CvMatMessage>("Debug Image");
}

void HoughCircle::process()
{
    CvMatMessage::Ptr msg = input_->getMessage<CvMatMessage>();

    int method = readParameter<int>("method");
    double dp = readParameter<double>("dp");
    double minDist = readParameter<double>("minDist");
    double param1 = readParameter<double>("param1");
    double param2 = readParameter<double>("param2");
    int minRadius = readParameter<int>("minRadius");
    int maxRadius = readParameter<int>("maxRadius");

    std::vector<cv::Vec3f> circles;
    cv::HoughCircles(msg->value, circles, method, dp, minDist, param1, param2, minRadius, maxRadius);

    CvMatMessage::Ptr out(new CvMatMessage(msg->getEncoding()));
    if(msg->hasChannels(1, CV_8U)) {
        cv::cvtColor(msg->value, out->value, CV_GRAY2BGR);
    } else {
        msg->value.copyTo(out->value);
    }

    for(unsigned i = 0; i < circles.size(); ++i) {
        cv::Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
        int radius = cvRound(circles[i][2]);
        cv::circle(out->value, center, 3, cv::Scalar(0,255,0), -1, 8, 0 );
        cv::circle(out->value, center, radius, cv::Scalar(0,0,255), 3, 8, 0 );
    }

    output_->publish(out);
}

