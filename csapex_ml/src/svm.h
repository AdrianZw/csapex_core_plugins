#ifndef SVM_H
#define SVM_H

/// PROJECT
#include "extended_svm.hpp"
#include <csapex/model/node.h>

namespace csapex
{
class CSAPEX_EXPORT_PLUGIN SVM : public Node
{
public:
    SVM();

    void setup(csapex::NodeModifier& node_modifier) override;
    void setupParameters(Parameterizable& parameters) override;
    void process() override;

private:
    enum ThresholdType
    {
        GREATER = 0,
        LESS,
        LESS_EQUAL,
        GREATER_EQUAL
    };

    Input* in_;
    Output* out_;
    Slot* reload_;

#if CV_MAJOR_VERSION == 2
    ExtendedSVM svm_;
#elif CV_MAJOR_VERSION >= 3
    cv::Ptr<cv::ml::SVM> svm_;
#endif
    std::string path_;
    bool loaded_;

    void reload();
};
}  // namespace csapex

#endif  // SVM_H
