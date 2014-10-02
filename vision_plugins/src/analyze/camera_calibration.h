#ifndef CAMERA_CALIBRATION_PLUGIN_H
#define CAMERA_CALIBRATION_PLUGIN_H

/// COMPONENT
#include <utils_vision/utils/camera_calibration.h>
#include <csapex/model/node.h>

namespace vision_plugins {
class CameraCalibration : public csapex::Node
{
public:
    CameraCalibration();

    virtual void process();
    virtual void setup();
    virtual void setupParameters();

private:
    utils_vision::CameraCalibration::Ptr calibration_;

    csapex::Output*            output_;
    csapex::Input*             input_;

    cv::Mat                    buffer_frame_;

    bool                       update_request_;

    void add();
    void calibrate();
    void updateCalibration();
    void requestUpdateCalibration();
};
}
#endif // CAMERA_CALIBRATION_PLUGIN_H
