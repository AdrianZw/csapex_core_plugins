#ifndef LISTENER_H
#define LISTENER_H

/// PROJECT
#include <csapex_ros/ros_handler.h>
#include <csapex/utility/assert.h>

/// SYSTEM
#define BOOST_SIGNALS_NO_DEPRECATION_WARNING
#include <tf/transform_listener.h>

namespace csapex
{

struct LockedTFListener;

struct TFListener {
    friend class LockedTFListener;

public:
    boost::shared_ptr<tf::TransformListener> tfl;

    static LockedTFListener getLocked();

    static void start();
    static void stop();

    void reset() {
        //tfl.clear();
        tfl.reset(new tf::TransformListener);
    }
    bool ok();

private:
    static TFListener* raw_instance() {
        static TFListener l;
        apex_assert_hard(&l);
        return &l;
    }

    TFListener();

    void cb(const tf::tfMessage::ConstPtr& msg);
    bool tryFrameAsReference(const tf::tfMessage::ConstPtr &msg, const std::string& frame);

    int retries;
    std::string reference_frame;
    bool init;
    ros::Time last_;
    ros::Subscriber tf_sub;

    QMutex m;
};

struct LockedTFListener {
public:
    TFListener* l;

    LockedTFListener(TFListener* ll)
        : l(nullptr)
    {
        if(ll) {
            ll->m.lock();
            l = ll;
        }
    }

    ~LockedTFListener()
    {
        if(l) {
            l->m.unlock();
        }
    }
};

}

#endif // LISTENER_H
