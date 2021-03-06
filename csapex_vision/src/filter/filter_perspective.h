#ifndef FILTER_PERSPECTIVE_H
#define FILTER_PERSPECTIVE_H

/// COMPONENT
#include <csapex_opencv/filter.h>
#include <cslibs_vision/utils/perspective_transform.h>

namespace csapex
{
class PerspectiveTransform : public csapex::Filter
{
public:
    PerspectiveTransform();

    void filter(cv::Mat& img, cv::Mat& mask) override;
    void setupParameters(Parameterizable& parameters) override;

    void update();

private:
    cslibs_vision::PerspectiveTransformer transformer_;
};

}  // namespace csapex
#endif  // FILTER_PERSPECTIVE_H
