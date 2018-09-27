#ifndef CIRCLE_MESSAGE_H
#define CIRCLE_MESSAGE_H

#include <csapex_opencv/circle.h>
#include <csapex/msg/message_template.hpp>
#include <memory>

namespace csapex {
namespace connection_types {
struct CircleMessage : public MessageTemplate<Circle, CircleMessage>
{
  typedef std::shared_ptr<CircleMessage> Ptr;
  typedef std::shared_ptr<CircleMessage const> ConstPtr;

  CircleMessage() {}
};


/// TRAITS
template <>
struct type<CircleMessage> {
  static std::string name() {
    return "CircleMessage";
  }
};
}
}

/// YAML
namespace YAML {
template<>
struct convert<csapex::connection_types::CircleMessage> {
  static Node encode(const csapex::connection_types::CircleMessage& rhs);
  static bool decode(const Node& node, csapex::connection_types::CircleMessage& rhs);
};
}
#endif // CIRCLE_MESSAGE_H
