/// HEADER
#include "listener.h"

using namespace csapex;

Listener::Listener()
{
    init = false;
}

LockedListener Listener::getLocked()
{
    Listener* l = raw_instance();
    if(l->tfl) {
        return LockedListener(l);
    } else {
        return LockedListener(NULL);
    }
}

void Listener::start()
{
    Listener* i = Listener::raw_instance();

    i->tfl.reset(new tf::TransformListener);
    i->tf_sub = ROSHandler::instance().nh()->subscribe<tf::tfMessage>("/tf", 0, boost::bind(&Listener::cb, i, _1));
    i->retries = 10;
}

void Listener::stop()
{
    Listener* i = Listener::raw_instance();
    i->tfl.reset();
}

bool Listener::ok()
{
    return init && tfl;
}

void Listener::cb(const tf::tfMessage::ConstPtr &msg)
{
    ros::Time now;
    if(init) {
        for(unsigned i = 0, n = msg->transforms.size(); i < n; ++i) {
            if(msg->transforms[i].child_frame_id == reference_frame) {
                now = msg->transforms[i].header.stamp;
                if(last_ > now + ros::Duration(3.0)) {
                    std::cout << "warning: reset tf listener, negative time change (" << last_ << " vs. " << now << ")" << std::endl;
                    reset();
                }

                last_ = now;

                break;
            }
        }
    } else {
        // try to use /base_link as reference frame, if it exists
        reference_frame = "";
        if(tryFrameAsReference(msg, "base_link")) {
            return;
        } else if(tryFrameAsReference(msg, "/base_link")) {
            return;
        }

        // if /base_link doesn't exist, use the first random one

        if(--retries <= 0) {
            if(msg->transforms.empty()) {
                std::cerr << "warning: no tf frames available!" << std::endl;
                init = false;
            } else {
                reference_frame = msg->transforms[0].child_frame_id;
                init = true;

                last_ = msg->transforms[0].header.stamp;
            }
        }
    }
}

bool Listener::tryFrameAsReference(const tf::tfMessage::ConstPtr &msg, const std::string &frame)
{

    for(unsigned i = 0, n = msg->transforms.size(); i < n; ++i) {
        if(msg->transforms[i].child_frame_id == frame) {
            reference_frame = frame;
            init = true;

            last_ = msg->transforms[i].header.stamp;
            return true;
        }
    }

    return false;
}
