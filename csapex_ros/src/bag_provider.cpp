/// HEADER
#include "bag_provider.h"

/// PROJECT
#include <utils_param/range_parameter.h>

/// SYSTEM
#include <boost/assign.hpp>
#include <csapex/utility/register_apex_plugin.h>

//CSAPEX_REGISTER_CLASS(csapex::BagProvider, csapex::MessageProvider)

using namespace csapex;

//BagProvider::BagProvider()
//    : view_(NULL), initiated(false)
//{
//}

//void BagProvider::load(const std::string& file)
//{
//    file_ = file;
//    bag.open(file_);

//    view_ = new rosbag::View(bag, rosbag::TopicQuery("/marlin/camera/image_raw"));
//    for(rosbag::View::iterator it = view_->begin(); it != view_->end(); ++it) {
//        frames_++;
//    }
//    view_it = view_->begin();
//    frames_--;

//    initiated = true;
//}

//BagProvider::~BagProvider()
//{
//}

//std::vector<std::string> BagProvider::getExtensions() const
//{
//    return boost::assign::list_of<std::string> (".bag");
//}

//bool BagProvider::hasNext()
//{
//    return initiated;
//}

//connection_types::Message::Ptr BagProvider::next()
//{
//    connection_types::Message::Ptr r;

//    if(!initiated) {
//        return r;
//    }

////    if(next_frame != -1) {
////        view_it = view_->begin();
////        for(state.current_frame=0; state.current_frame != next_frame; ++state.current_frame) {
////            view_it++;
////        }
////        next_frame = -1;
////    }
////    apex_assert_hard(hasNext());

////    if(view_it != view_->end()) {
////        sensor_msgs::ImageConstPtr img_msg = view_it->instantiate<sensor_msgs::Image>();
////        apex_assert_hard(img_msg != NULL);

////        cv_bridge::CvImageConstPtr mat = cv_bridge::toCvShare(img_msg, "bgr8");
////        mat->image.copyTo(last_frame_);

////        view_it++;
////        state.current_frame++;
////    }

////    img = last_frame_;

////    if(!slider_->isSliderDown()) {
////        slider_->setValue(state.current_frame);
////    }

////    if(state.current_frame == frames_) {
////        setPlaying(false);
////    }
//    return r;
//}

//Memento::Ptr BagProvider::getState() const
//{
//    return Memento::Ptr();
//}

//void BagProvider::setParameterState(Memento::Ptr memento)
//{

//}
