#ifndef MATCH_MESSAGE_H
#define MATCH_MESSAGE_H

/// PROJECT
#include <csapex/msg/message_template.hpp>

/// SYSTEM
#include <opencv2/opencv.hpp>

namespace csapex {
namespace connection_types {


struct MatchMessage : public MessageTemplate<std::vector<cv::DMatch>, MatchMessage>
{
    MatchMessage();

    bool isContainer() const override;
    Token::Ptr nestedType() const override;
    virtual void addNestedValue(const Token::ConstPtr &msg) override;
    virtual Token::ConstPtr nestedValue(std::size_t i) const override;
    virtual std::size_t nestedValueCount() const override;
};


/// TRAITS
template <>
struct type<MatchMessage> {
    static std::string name() {
        return "std::vector<cv::DMatch>";
    }
};

}
}

/// YAML
namespace YAML {
template<>
struct convert<csapex::connection_types::MatchMessage> {
  static Node encode(const csapex::connection_types::MatchMessage& rhs);
  static bool decode(const Node& node, csapex::connection_types::MatchMessage& rhs);
};
}


#endif // MATCH_MESSAGE_H
