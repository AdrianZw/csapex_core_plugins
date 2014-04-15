/// HEADER
#include <csapex_core_plugins/ros_handler.h>

/// SYSTEM
#include <QtConcurrentRun>
#include <boost/foreach.hpp>

using namespace csapex;

ROSHandler::ROSHandler()
    : initialized_(false)
{
    initHandle(true);
}

ROSHandler::~ROSHandler()
{
    stop();
    ros::shutdown();
}

void ROSHandler::stop()
{
    if(spinner_) {
        spinner_->stop();
        spinner_.reset(static_cast<ros::AsyncSpinner*>(NULL));
    }
    if(nh_) {
        nh_->shutdown();
        nh_.reset((static_cast<ros::NodeHandle*>(NULL)));
    }
}

boost::shared_ptr<ros::NodeHandle> ROSHandler::nh()
{
    refresh();
    return nh_;
}


void ROSHandler::initHandle(bool try_only)
{
    if(!initialized_) {
        checkMasterConnection();
    }

    QMutexLocker lock(&has_connection_mutex);
    if(try_only && has_connection.isRunning()) {
        std::cout << "init handle: still probing master" << std::endl;
        return;
    }

    if(has_connection.result() && !nh_) {
        nh_.reset(new ros::NodeHandle("~"));
        spinner_.reset(new ros::AsyncSpinner(2));
        spinner_->start();

        lock.unlock();

        BOOST_FOREACH (const boost::function<void()>& f, callbacks_) {
            f();
        }
    }
}

bool ROSHandler::isConnected()
{
    QMutexLocker lock(&has_connection_mutex);
    if(has_connection.isRunning()) {
        return false;
    } else {
        return has_connection.result();
    }
}

bool ROSHandler::topicExists(const std::string &topic)
{
    refresh();

    ros::master::V_TopicInfo topics;
    ros::master::getTopics(topics);

    for(ros::master::V_TopicInfo::iterator it = topics.begin(); it != topics.end(); ++it) {
        if(it->name == topic) {
            return true;
        }
    }

    return false;
}

void ROSHandler::registerConnectionCallback(boost::function<void ()> f)
{
    if(isConnected()) {
        f();
    }
    callbacks_.push_back(f);
}


void ROSHandler::checkMasterConnection()
{
    QMutexLocker lock(&has_connection_mutex);

    if(!ros::isInitialized()) {
        int c = 0;
        ros::init(c, (char**) NULL, "csapex");
    }
    //initialized_ = true;

    if(!has_connection.isRunning()) {
        has_connection = QtConcurrent::run(ros::master::check);
    }
}

bool ROSHandler::waitForConnection()
{
    if(!initialized_) {
        checkMasterConnection();
    }

    QMutexLocker lock(&has_connection_mutex);
    has_connection.waitForFinished();
    return has_connection.result();
}

void ROSHandler::refresh()
{
    waitForConnection();

    if(nh_) {
        QMutexLocker lock(&has_connection_mutex);
        // connection was there
        has_connection.waitForFinished();
        if(!has_connection.result()) {
            // connection no longer there
            stop();
        }
    }

    initHandle();
}
