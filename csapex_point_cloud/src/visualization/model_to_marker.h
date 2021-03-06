#ifndef MODEL_TO_MARKER_H
#define MODEL_TO_MARKER_H

/// PROJECT
#include <csapex/model/node.h>
#include <csapex_point_cloud/msg/model_message.h>

/// SYSTEM
#include <visualization_msgs/MarkerArray.h>

namespace csapex
{
class ModelToMarker : public csapex::Node
{
public:
    ModelToMarker();

    void setup(csapex::NodeModifier& node_modifier) override;
    void setupParameters(Parameterizable& parameters) override;
    void process() override;

private:
    Input* input_;
    Output* output_;
    Output* output_text_;
    void generateMarker(const ModelMessage model_message, const visualization_msgs::Marker::Ptr marker, std::vector<int> color);
    void publishText(const ModelMessage model_message);
};

}  // end namespace csapex
#endif  // MODEL_TO_MARKER_H
