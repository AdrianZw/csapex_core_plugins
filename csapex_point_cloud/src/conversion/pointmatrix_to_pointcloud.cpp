/// HEADER
#include "pointmatrix_to_pointcloud.h"

/// PROJECT
#include <csapex_point_cloud/point_cloud_message.h>
#include <csapex/msg/input.h>
#include <csapex/msg/output.h>
#include <utils_param/parameter_factory.h>
#include <csapex_vision/cv_mat_message.h>
#include <csapex/model/node_modifier.h>
#include <csapex/utility/register_apex_plugin.h>

/// SYSTEM
#include <pcl/point_types.h>
#include <pcl/conversions.h>

CSAPEX_REGISTER_CLASS(csapex::PointmatrixToPointcloud, csapex::Node)

using namespace csapex;
using namespace csapex::connection_types;

namespace implementation {
inline void convert(const cv::Mat &matrix, pcl::PointCloud<pcl::PointXYZ>::Ptr &cloud)
{
    if(matrix.type() != CV_32FC3) {
        throw std::runtime_error("3Channel float matrix required!");
    }

    cloud.reset(new pcl::PointCloud<pcl::PointXYZ>);

    for(int i = 0 ; i < matrix.rows ; ++i) {
        for(int j = 0 ; j < matrix.cols ; ++j) {
            pcl::PointXYZ pt;
            pt.x = matrix.at<float>(i, (j * 3 + 0));
            pt.y = matrix.at<float>(i, (j * 3 + 1));
            pt.z = matrix.at<float>(i, (j * 3 + 2));
            cloud->push_back(pt);
        }
    }

    cloud->height = matrix.rows;
    cloud->width  = matrix.cols;
    cloud->is_dense = false;

}
}

PointmatrixToPointcloud::PointmatrixToPointcloud()
{
}

void PointmatrixToPointcloud::process()
{
    CvMatMessage::Ptr in = input_->getMessage<connection_types::CvMatMessage>();
    PointCloudMessage::Ptr out(new PointCloudMessage(readParameter<std::string>("frame"), in->stamp));
    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud;

    implementation::convert(in->value, cloud);
    out->value = cloud;
    output_->publish(out);
}

void PointmatrixToPointcloud::setup()
{
    input_  = modifier_->addInput<CvMatMessage>("Point Matrix");
    output_ = modifier_->addOutput<PointCloudMessage>("PointCloud");
}

void PointmatrixToPointcloud::setupParameters()
{
    addParameter(param::ParameterFactory::declareText("frame", "/camera"));
}
