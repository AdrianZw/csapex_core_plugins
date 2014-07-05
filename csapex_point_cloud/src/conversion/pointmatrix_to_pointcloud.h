#ifndef POINTMATRIX_TO_POINTCLOUD_H
#define POINTMATRIX_TO_POINTCLOUD_H

/// PROJECT
#include <csapex/model/node.h>

namespace csapex {
class PointmatrixToPointcloud : public csapex::Node
{
public:
    PointmatrixToPointcloud();

    virtual void process();
    virtual void setup();
    virtual void setupParameters();

private:
    ConnectorIn* input_;
    ConnectorOut* output_;

};
}
#endif // POINTMATRIX_TO_POINTCLOUD_H
