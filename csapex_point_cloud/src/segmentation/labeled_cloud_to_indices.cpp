#include "labeled_cloud_to_indices.h"

/// PROJECT
#include <csapex/msg/input.h>
#include <csapex/msg/output.h>
#include <csapex_vision/cv_mat_message.h>
#include <utils_param/parameter_factory.h>
#include <csapex_core_plugins/vector_message.h>
#include <csapex/model/node_modifier.h>
#include <csapex_point_cloud/point_cloud_message.h>
#include <csapex/utility/register_apex_plugin.h>
#include <csapex_point_cloud/indeces_message.h>

/// SYSTEM
#include <pcl/point_types.h>
#include <pcl/conversions.h>
#include <pcl/filters/extract_indices.h>

CSAPEX_REGISTER_CLASS(csapex::LabeledCloudToIndices, csapex::Node)

using namespace csapex;
using namespace csapex::connection_types;

#define FLOOD_DEFAULT_LABEL 0

LabeledCloudToIndices::LabeledCloudToIndices()
{
}


void LabeledCloudToIndices::process()
{
    PointCloudMessage::ConstPtr msg(input_->getMessage<PointCloudMessage>());

    boost::apply_visitor (PointCloudMessage::Dispatch<LabeledCloudToIndices>(this, msg), msg->value);
}

void LabeledCloudToIndices::setup()
{
    input_  = modifier_->addInput<PointCloudMessage>("Labeled PointCloud");
    output_ = modifier_->addOutput<GenericVectorMessage, pcl::PointIndices > ("Clusters");
}


namespace implementation {

template<class PointT>
struct Impl {
    inline static void extract(const typename pcl::PointCloud<PointT>::ConstPtr src,
                               typename boost::shared_ptr<std::vector<pcl::PointIndices> > dst)
    {
        //for(typename pcl::PointCloud<PointT>::const_iterator it = src->begin() ; it != src->end() ; ++it) {
        std::map<unsigned int, std::vector<int> > clusters;
        clusters.insert(std::make_pair(FLOOD_DEFAULT_LABEL, std::vector<int>()));
        for (unsigned j = 0; j < src->points.size(); j++) {
        //for(typename pcl::PointCloud<PointT>::const_iterator it = src->begin() ; it != src->end() ; ++it) {
            // if the label ist not found in the clusters map
            if (clusters.find(src->at(j).label) == clusters.end()) {
                std::vector<int> indices;
                clusters.insert(std::make_pair(src->at(j).label, indices));
            }

            // save the indice in the vector of the right label
            clusters.at(src->at(j).label).push_back(j);
        }

        // Save the indices from the map in the vector of indices
        for (std::map<unsigned int, std::vector<int> >::const_iterator it = clusters.begin(); it != clusters.end(); it++) {
            pcl::PointIndices ind;
            ind.header = src->header;
            //ind.indices = it->second();
            dst->push_back(ind);
        }
    }
};

template<class PointT>
struct Conversion {
    static void apply(const typename pcl::PointCloud<PointT>::ConstPtr src,
                      typename boost::shared_ptr<std::vector<pcl::PointIndices> > dst)
    {
        throw std::runtime_error("Type of pointcloud must be labeled!");
    }
};

template<>
struct Conversion<pcl::PointXYZL>{
    static void apply(const typename pcl::PointCloud<pcl::PointXYZL>::ConstPtr src,
                      typename boost::shared_ptr<std::vector<pcl::PointIndices> > dst)
    {
      Impl<pcl::PointXYZL>::extract(src, dst);
    }

};

template<>
struct Conversion<pcl::PointXYZRGBL>{
    static void apply(const typename pcl::PointCloud<pcl::PointXYZRGBL>::ConstPtr src,
                      typename boost::shared_ptr<std::vector<pcl::PointIndices> > dst)
    {
        Impl<pcl::PointXYZRGBL>::extract(src, dst);
    }
};
}



template <class PointT>
void LabeledCloudToIndices::inputCloud(typename pcl::PointCloud<PointT>::ConstPtr cloud)
{
    PointCloudMessage::Ptr out(new PointCloudMessage(cloud->header.frame_id, cloud->header.stamp));
    pcl::PointCloud<pcl::PointXYZRGB>::Ptr out_cloud(new pcl::PointCloud<pcl::PointXYZRGB>);

    boost::shared_ptr<std::vector<pcl::PointIndices> > cluster_indices(new std::vector<pcl::PointIndices>);

    implementation::Conversion<PointT>::apply(cloud, cluster_indices);


    output_->publish<GenericVectorMessage, pcl::PointIndices >(cluster_indices);
}
