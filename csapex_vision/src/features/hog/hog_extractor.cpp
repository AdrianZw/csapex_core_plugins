﻿/// HEADER
#include "hog_extractor.h"

/// PROJECT
#include <csapex/model/node_modifier.h>
#include <csapex/msg/generic_vector_message.hpp>
#include <csapex/msg/io.h>
#include <csapex/param/parameter_factory.h>
#include <csapex/utility/register_apex_plugin.h>
#include <csapex_ml/features_message.h>
#include <csapex_opencv/cv_mat_message.h>
#include <csapex_opencv/roi_message.h>

/// SYSTEM
#include <opencv2/objdetect/objdetect.hpp>

CSAPEX_REGISTER_CLASS(csapex::HOGExtractor, csapex::Node)
/// TODO : L2HysThreshold - derivAperture

using namespace csapex;
using namespace csapex::connection_types;
using namespace csapex;

HOGExtractor::HOGExtractor()
{
}

void HOGExtractor::setupParameters(Parameterizable& parameters)
{
    parameters.addParameter(param::factory::declareRange("hog/sigma", param::ParameterDescription("Standard deviation for Gaussian blur."), 0.0, 10.0, 0.0, 0.1), hog_.winSigma);

    parameters.addParameter(param::factory::declareBool("hog/gamma_correction", param::ParameterDescription("Enable the gamma correction."), true), hog_.gammaCorrection);
    parameters.addParameter(param::factory::declareBool("hog/signed_gradient", param::ParameterDescription("Un-/directed gradients."), hog_.signedGradient), hog_.signedGradient);

    addParameter(param::factory::declareRange("hog/gradient_bins", param::ParameterDescription("Amount of gradient bins."), 2, 18, hog_.nbins, 1), hog_.nbins);

    parameters.addParameter(param::factory::declareRange("hog/cells_x", param::ParameterDescription("Cells in x direction."), 2, 16, 8, 1), cells_x_);

    parameters.addParameter(param::factory::declareRange("hog/cells_y", param::ParameterDescription("Cells in x direction."), 2, 16, 16, 1), cells_y_);

    parameters.addParameter(param::factory::declareRange("hog/cell_size", param::ParameterDescription("Size of the cells."), 4, 16, 8, 1), cell_size_);
    parameters.addParameter(param::factory::declareRange("hog/block_size", param::ParameterDescription("Cell count in both dimension of a block."), 1, 4, 2, 1), block_size_);
    parameters.addParameter(param::factory::declareRange("hog/bock_stride", param::ParameterDescription("Overlap of each block in cells."), 1, 3, 1, 1), block_stride_);

    std::map<std::string, int> adpation_types = { { "Scale", SCALE }, { "TryGrow", TRY_GROW }, { "GrowStrict", GROW_STRICT } };

    parameters.addParameter(param::factory::declareParameterSet("hog/adaption_mode", param::ParameterDescription("Adaption of rois to window size of hog."), adpation_types, (int)SCALE),
                            adaption_type_);
    parameters.addParameter(param::factory::declareBool("mirror", true), mirror_);
}

void HOGExtractor::setup(NodeModifier& node_modifier)
{
    in_img_ = node_modifier.addInput<CvMatMessage>("image");
    in_rois_ = node_modifier.addInput<GenericVectorMessage, RoiMessage>("rois");
    out_ = node_modifier.addOutput<GenericVectorMessage, FeaturesMessage>("features");
}

void HOGExtractor::process()
{
    CvMatMessage::ConstPtr in = msg::getMessage<CvMatMessage>(in_img_);
    std::shared_ptr<std::vector<RoiMessage> const> in_rois = msg::getMessage<GenericVectorMessage, RoiMessage>(in_rois_);
    std::shared_ptr<std::vector<FeaturesMessage>> out(new std::vector<FeaturesMessage>);

    /// update HOG parameters
    hog_.cellSize.width = cell_size_;
    hog_.cellSize.height = cell_size_;
    hog_.winSize.height = cell_size_ * cells_y_;
    hog_.winSize.width = cell_size_ * cells_x_;
    hog_.blockSize = hog_.cellSize * block_size_;
    hog_.blockStride = hog_.cellSize * block_stride_;
    ratio_hog_ = hog_.winSize.width / (double)hog_.winSize.height;
    if (hog_.winSigma == 0)
        hog_.winSigma = -1;

    if (in->value.channels() != 1 && in->value.channels() != 3) {
        throw std::runtime_error("Only 1 or 3 channel matrices supported!");
    }

    for (auto& roi : *in_rois) {
        cv::Mat data;

        if (!getData(in->value, roi.value.rect(), data))
            continue;

        if (data.empty())
            continue;

        assert(data.rows == hog_.winSize.height);
        assert(data.cols == hog_.winSize.width);

        FeaturesMessage feature;
        feature.type = FeaturesMessage::Type::CLASSIFICATION;
        feature.classification = roi.value.classification();
        hog_.computeSingle(data, feature.value);
        out->push_back(feature);
        if (mirror_) {
            cv::flip(data, data, 1);
            hog_.compute(data, feature.value);
            out->push_back(feature);
        }
    }
    msg::publish<GenericVectorMessage, FeaturesMessage>(out_, out);
}

bool HOGExtractor::getData(const cv::Mat& src, const cv::Rect& roi, cv::Mat& dst)
{
    cv::Mat window;
    double ratio_roi = roi.width / (double)roi.height;
    cv::Rect roi_adapted = roi;

    switch (adaption_type_) {
        case SCALE:
            break;
        case TRY_GROW:
        case GROW_STRICT:
            if (ratio_roi < ratio_hog_) {
                /// scale the width
                double scale_width = ratio_roi / ratio_hog_;
                roi_adapted.width *= scale_width;
                roi_adapted.x -= (roi_adapted.width - roi.width) / 2;
                /// move to fit into image
                if (roi_adapted.x < 0) {
                    roi_adapted.x = 0;
                } else {
                    int overshoot = (src.cols - 1) - (roi_adapted.x + roi.width);
                    if (overshoot < 0) {
                        roi_adapted.x += overshoot;
                    }
                }
            } else if (ratio_roi > ratio_hog_) {
                /// scale the height
                double scale_height = ratio_hog_ / ratio_roi;
                roi_adapted.height *= scale_height;
                roi_adapted.y -= (roi_adapted.height - roi.height) / 2;
                /// move to fit into image
                if (roi_adapted.y < 0) {
                    roi_adapted.y = 0;
                } else {
                    int overshoot = (src.rows - 1) - (roi_adapted.y + roi.height);
                    if (overshoot < 0) {
                        roi_adapted.y += overshoot;
                    }
                }
            }
            if (roi.height > src.rows || roi.width > src.cols) {
                cv::Rect img_rect(0, 0, src.cols, src.rows);
                roi_adapted = roi_adapted & img_rect;

                if (adaption_type_ == GROW_STRICT)
                    return false;
            }
            break;
        default:
            throw std::runtime_error("Unknown adaption type!");
    }

    cv::Rect img_rect(0, 0, src.cols, src.rows);
    roi_adapted = roi_adapted & img_rect;

    if (roi_adapted.area() <= 0)
        return false;

    window = cv::Mat(src, roi_adapted);
    cv::resize(window, dst, hog_.winSize);
    return true;
}
