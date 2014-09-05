/// HEADER
#include <csapex_vision/filter.h>

/// COMPONENT
#include <csapex_vision/cv_mat_message.h>

/// PROJECT
#include <csapex/msg/input.h>
#include <csapex/msg/output.h>
#include <csapex/model/node_modifier.h>

/// SYSTEM
#include <QLabel>

using namespace csapex;
using namespace connection_types;


Filter::Filter(const UUID& uuid)
    : Node(uuid), input_img_(NULL), input_mask_(NULL), output_img_(NULL), output_mask_(NULL)
{
}

Filter::~Filter()
{
}

void Filter::setup()
{
    input_img_ = modifier_->addInput<CvMatMessage>("Image");
    if(usesMask()) {
        input_mask_ = modifier_->addOptionalInput<CvMatMessage>("Mask");
    }
    output_img_ = modifier_->addOutput<CvMatMessage>("Image");
    if(usesMask()) {
        output_mask_ = modifier_->addOutput<CvMatMessage>("Mask");
    }
}

void Filter::process()
{
    CvMatMessage::Ptr img_msg = input_img_->getMessage<CvMatMessage>();
    CvMatMessage::Ptr mask_msg;
    if(usesMask() && input_mask_->hasMessage()) {
        mask_msg = input_mask_->getMessage<CvMatMessage>();
    }

    if(img_msg.get() && !img_msg->value.empty()) {
        if(!mask_msg.get()) {
            mask_msg.reset(new CvMatMessage(enc::mono));
        }

        filter(img_msg->value, mask_msg->value);

        output_img_->publish(img_msg);
        if(usesMask() && mask_msg.get()) {
            output_mask_->publish(mask_msg);
        }
    }
}

bool Filter::usesMask()
{
    return true;
}
