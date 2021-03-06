#ifndef BINOMIAL_FILTER_H
#define BINOMIAL_FILTER_H

/// COMPONENT
#include <csapex/model/node.h>
#include <opencv2/core/core.hpp>

namespace csapex
{
class BinomialFilter : public csapex::Node
{
public:
    BinomialFilter();

    void process() override;
    void setup(csapex::NodeModifier& node_modifier) override;
    void setupParameters(Parameterizable& parameters) override;

protected:
    int kernel_size_;
    cv::Mat kernel_;
    csapex::Output* output_;
    csapex::Input* input_;
};
}  // namespace csapex
#endif  // BINOMIAL_FILTER_H
