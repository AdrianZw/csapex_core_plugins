#ifndef TRANSFORM_MESSAGE_H
#define TRANSFORM_MESSAGE_H

/// PROJECT
#include <csapex/msg/message_template.hpp>

/// COMPONENT
#include <csapex_transform/binary_io.h>

/// SYSTEM
#include <tf/LinearMath/Transform.h>

namespace csapex
{
namespace connection_types
{
struct TransformMessage : public MessageTemplate<tf::Transform, TransformMessage>
{
    TransformMessage();
    TransformMessage(const std::string& frame_id, const std::string& child_frame_id);

    void sanitize();

    bool cloneData(const TransformMessage& other);

public:
    std::string child_frame;
};

/// TRAITS
template <>
struct type<TransformMessage>
{
    static std::string name()
    {
        return "TransformMessage";
    }
};

}  // namespace connection_types
}  // namespace csapex

/// YAML
namespace YAML
{
template <>
struct convert<csapex::connection_types::TransformMessage>
{
    static Node encode(const csapex::connection_types::TransformMessage& rhs);
    static bool decode(const Node& node, csapex::connection_types::TransformMessage& rhs);
};
}  // namespace YAML

#endif  // TRANSFORM_MESSAGE_H
