#ifndef DESCRIPTOR_MESSAGE_H
#define DESCRIPTOR_MESSAGE_H

/// PROJECT
#include <csapex/model/message.h>

/// SYSTEM
#include <opencv2/opencv.hpp>

namespace csapex {
namespace connection_types {


struct DescriptorMessage : public MessageTemplate<cv::Mat, DescriptorMessage>
{
    DescriptorMessage();

    bool isBinary() const;
};

}
}

#endif // DESCRIPTOR_MESSAGE_H
