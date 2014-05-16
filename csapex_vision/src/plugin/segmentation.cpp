/// HEADER
#include "segmentation.h"

/// PROJECT
#include <csapex/model/connector_in.h>
#include <csapex/model/connector_out.h>
#include <utils_param/parameter_factory.h>

/// SYSTEM
#include <csapex/utility/register_apex_plugin.h>
#include <boost/foreach.hpp>

CSAPEX_REGISTER_CLASS(csapex::Segmentation, csapex::Node)

using namespace csapex;
using namespace csapex::connection_types;

Segmentation::Segmentation()
{
    addTag(Tag::get("Vision"));
    Tag::createIfNotExists("Segmentation");
    addTag(Tag::get("Segmentation"));
}

void Segmentation::setState(Memento::Ptr memento)
{
    Node::setState(memento);
    loaded_state_ = boost::dynamic_pointer_cast<GenericState>(memento);
}

void Segmentation::process()
{
    CvMatMessage::Ptr img = input_img_->getMessage<CvMatMessage>();

    bool encoding_changed = img->getEncoding() != current_encoding;
    current_encoding = img->getEncoding();

    CvMatMessage::Ptr out_mask(new CvMatMessage(enc::mono));

    if(encoding_changed) {
        recompute();

        if(loaded_state_) {
            Node::setState(loaded_state_);
            loaded_state_.reset((GenericState*)NULL);
            triggerParameterSetChanged();
            update();
        }

        Q_EMIT modelChanged();

        output_mask_->publish(out_mask);
        return;
    }

    cv::Mat bw;
    cv::inRange(img->value, min, max, bw);

    if(input_mask_->isConnected()) {
        CvMatMessage::Ptr mask = input_mask_->getMessage<CvMatMessage>();
        cv::min(mask->value, bw, out_mask->value);
    } else {
        out_mask->value = bw;
    }

    output_mask_->publish(out_mask);
}

namespace {
std::string channelName(int idx, const Channel& c)
{
    std::stringstream name;
    name << "c" << idx << " [" << c.name << "]";
    return name.str();
}
}

void Segmentation::update()
{
    min = cv::Scalar::all(0);
    max = cv::Scalar::all(0);

    for(std::size_t i = 0; i < current_encoding.size(); ++i) {
        std::pair<int,int> val = param<std::pair<int, int> >(channelName(i, current_encoding[i]));

        min[i] = val.first;
        max[i] = val.second;
    }
}

void Segmentation::recompute()
{
    setParameterSetSilence(true);
    removeTemporaryParameters();
    for(std::size_t i = 0; i < current_encoding.size(); ++i) {
        Channel c = current_encoding[i];

        std::string name = channelName(i, c);
        param::Parameter::Ptr p = param::ParameterFactory::declareInterval(name, c.min, c.max, c.min, c.max, 1);
        addTemporaryParameter(p, boost::bind(&Segmentation::update, this));
    }

    setParameterSetSilence(false);
    triggerParameterSetChanged();

    update();
}

void Segmentation::setup()
{
    input_img_ = addInput<CvMatMessage>("Image");
    input_mask_ = addInput<CvMatMessage>("Mask", true);
    output_mask_ = addOutput<CvMatMessage>("Mask");
}
