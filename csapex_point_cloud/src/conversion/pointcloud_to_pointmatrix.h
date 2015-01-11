#ifndef TO_POINT_MATRIX_H
#define TO_POINT_MATRIX_H

/// PROJECT
#include <csapex/model/node.h>
#include <csapex_point_cloud/point_cloud_message.h>

namespace csapex {
class PointCloudToPointMatrix : public csapex::Node
{
public:
    PointCloudToPointMatrix();

    virtual void process();
    virtual void setup();

    template <class PointT>
    void inputCloud(typename pcl::PointCloud<PointT>::ConstPtr cloud);

protected:
    Input*  input_;
    Output* output_;
    Output* mask_;
};
}
#endif // TO_POINT_MATRIX_H
