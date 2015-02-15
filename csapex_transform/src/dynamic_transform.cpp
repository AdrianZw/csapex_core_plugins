/// HEADER
#include "dynamic_transform.h"

/// COMPONENT
#include <csapex_transform/transform_message.h>
#include <csapex_ros/time_stamp_message.h>
#include <csapex_ros/tf_listener.h>

/// PROJECT
#include <csapex/msg/io.h>
#include <csapex/signal/trigger.h>
#include <utils_param/parameter_factory.h>
#include <csapex/model/node_modifier.h>
#include <csapex/utility/register_apex_plugin.h>
#include <csapex/msg/generic_value_message.hpp>

/// SYSTEM
#include <tf/transform_datatypes.h>

CSAPEX_REGISTER_CLASS(csapex::DynamicTransform, csapex::Node)

using namespace csapex;

DynamicTransform::DynamicTransform()
    : init_(false), initial_retries_(10)
{
}

void DynamicTransform::setupParameters()
{
    std::vector<std::string> topics;
    addParameter(param::ParameterFactory::declareParameterStringSet("source", topics), std::bind(&DynamicTransform::update, this));
    addParameter(param::ParameterFactory::declareParameterStringSet("target", topics), std::bind(&DynamicTransform::update, this));

    addParameter(param::ParameterFactory::declareTrigger("refresh"), std::bind(&DynamicTransform::refresh, this));
    addParameter(param::ParameterFactory::declareTrigger("reset tf"), std::bind(&DynamicTransform::resetTf, this));

    source_p = std::dynamic_pointer_cast<param::SetParameter>(getParameter("source"));
    target_p = std::dynamic_pointer_cast<param::SetParameter>(getParameter("target"));

    refresh();
}

bool DynamicTransform::canTick()
{
    return !init_
            ||
            (source_p->noParameters() != 0 && target_p->noParameters() != 0 &&
            !msg::isConnected(frame_in_source_) && !msg::isConnected(frame_in_target_) && !msg::isConnected(time_in_));
}

void DynamicTransform::tick()
{
    process();
}

void DynamicTransform::process()
{
    if(!init_) {
        refresh();
    }

    modifier_->setNoError();
    bool update = false;

    bool use_in_frame = msg::hasMessage(frame_in_source_);
    source_p->setEnabled(!use_in_frame);

    if(use_in_frame) {
        std::string from = msg::getValue<std::string>(frame_in_source_);

        if(readParameter<std::string>("source") != from) {
            setParameter("source", from);
            update = true;
        }
    }


    bool use_to_frame = msg::hasMessage(frame_in_target_);
    target_p->setEnabled(!use_to_frame);

    if(use_to_frame) {
        std::string target = msg::getValue<std::string>(frame_in_target_);

        if(readParameter<std::string>("target") != target) {
            setParameter("target", target);
            update = true;
        }
    }

    if(update) {
        refresh();
    }


    if(msg::isConnected(time_in_) && msg::hasMessage(time_in_)) {
        connection_types::TimeStampMessage::ConstPtr time_msg = msg::getMessage<connection_types::TimeStampMessage>(time_in_);
        publishTransform(time_msg->value);
    } else {
        publishTransform(ros::Time(0));
    }
}

void DynamicTransform::publishTransform(const ros::Time& time)
{
    if(!init_ || source_p->noParameters() == 0 || target_p->noParameters() == 0) {
        refresh();
    }

    if(!init_) {
        return;
    }

    tf::StampedTransform t;

    if(getParameter("source")->is<void>()) {
        throw std::runtime_error("from is not a string");
    }
    if(getParameter("target")->is<void>()) {
        throw std::runtime_error("to is not a string");
    }

    std::string target = readParameter<std::string>("target");
    std::string source = readParameter<std::string>("source");

    try {
        LockedTFListener l = TFListener::getLocked();

        if(l.l) {
            tf::TransformListener& tfl = *l.l->tfl;
            if(tfl.waitForTransform(target, source, time, ros::Duration(0.1))) {
                tfl.lookupTransform(target, source, time, t);
            } else {
                if(tfl.canTransform(target, source, ros::Time(0))) {
                    modifier_->setWarning("cannot transform, using latest transform");
                    tfl.lookupTransform(target, source, ros::Time(0), t);
                } else {
                    modifier_->setWarning("cannot transform at all...");
                    return;
                }
            }
            modifier_->setNoError();
        } else {
            return;
        }
    } catch(const tf2::TransformException& e) {
        modifier_->setWarning(e.what());
        return;
    }

    connection_types::TransformMessage::Ptr msg(new connection_types::TransformMessage);
    msg->value = t;
    msg->frame_id = target;
    msg->child_frame = source;
    msg::publish(output_, msg);

    msg::publish(output_frame_, readParameter<std::string>("target"));
}

void DynamicTransform::setup()
{
    time_in_ = modifier_->addOptionalInput<connection_types::TimeStampMessage>("Time");
    frame_in_source_ = modifier_->addOptionalInput<std::string>("Origin Frame");
    frame_in_target_ = modifier_->addOptionalInput<std::string>("Target Frame");

    output_ = modifier_->addOutput<connection_types::TransformMessage>("Transform");
    output_frame_ = modifier_->addOutput<std::string>("Target Frame");

    modifier_->addSlot("reset", std::bind(&DynamicTransform::resetTf, this));
    reset_ = modifier_->addTrigger("reset");
}


void DynamicTransform::resetTf()
{
    {
        LockedTFListener l = TFListener::getLocked();
        if(l.l) {
            l.l->reset();
        }
    }

    reset_->trigger();
}

void DynamicTransform::refresh()
{
    std::vector<std::string> frames;

    std::string to, from;

    if(getParameter("source")->is<std::string>()) {
        to = target_p->as<std::string>();
    }
    if(getParameter("target")->is<std::string>()) {
        from = source_p->as<std::string>();
    }
    ainfo << "from: " << from << ", to: " << to << std::endl;

    LockedTFListener l = TFListener::getLocked();
    if(!l.l) {
        return;
    }

    if(l.l) {
        std::vector<std::string> f;
        l.l->tfl->getFrameStrings(f);

        bool has_from = false;
        bool has_to = false;
        for(std::size_t i = 0; i < f.size(); ++i) {
            std::string frame = std::string("/") + f[i];
            frames.push_back(frame);

            if(frame == from) {
                has_from = true;
            }
            if(frame == to) {
                has_to = true;
            }
        }

        if(!has_from || !has_to) {
            if(initial_retries_ --> 0) {
                ainfo << "retry" << std::endl;
                return;
            }
        }


    } else {
        return;
    }

    if(!from.empty()) {
        if(std::find(frames.begin(), frames.end(), from) == frames.end()) {
            frames.push_back(from);
        }
    }
    if(!to.empty()) {
        if(std::find(frames.begin(), frames.end(), to) == frames.end()) {
            frames.push_back(to);
        }
    }

    source_p->setSet(frames);
    target_p->setSet(frames);

    init_ = true;
}

void DynamicTransform::update()
{
}
