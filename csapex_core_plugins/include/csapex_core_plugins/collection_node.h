#ifndef COLLECTION_NODE_H
#define COLLECTION_NODE_H

/// PROJECT
#include <csapex/model/node.h>
#include <csapex_core_plugins/vector_message.h>
#include <csapex/model/node_modifier.h>
#include <csapex/msg/io.h>
#include <utils_param/parameter_factory.h>
#include <csapex/signal/trigger.h>

/// SYSTEM
namespace csapex {

template <typename MessageType>
class CollectionNode : public Node
{
public:
    CollectionNode()
    {}

    void setupParameters()
    {
        addParameter(param::ParameterFactory::declareTrigger("process"), std::bind(&CollectionNode<MessageType>::doProcessCollection, this, boost::ref(buffer_)));
        addParameter(param::ParameterFactory::declareTrigger("clear"), std::bind(&CollectionNode<MessageType>::clearCollection, this));
    }

    void setup()
    {
        in_vector  = modifier_->addOptionalInput<connection_types::GenericVectorMessage, MessageType>("messages to collect");
        in_single  = modifier_->addOptionalInput<MessageType>("message to collect");
    }

    void process()
    {
        if(msg::hasMessage(in_vector)) {
            std::shared_ptr<std::vector<MessageType> const> input = msg::getMessage<connection_types::GenericVectorMessage, MessageType>(in_vector);
            buffer_.insert(buffer_.end(), input->begin(), input->end());
        }
        if(msg::hasMessage(in_single)) {
            std::shared_ptr<MessageType const> input = msg::getMessage<MessageType>(in_single);
            buffer_.push_back(*input);
        }
    }

protected:
    void doProcessCollection(std::vector<MessageType>& collection)
    {
        processCollection(collection);
    }

    virtual void processCollection(std::vector<MessageType>& collection) = 0;

    void clearCollection()
    {
        buffer_.clear();
    }

protected:
    Input* in_vector;
    Input* in_single;

private:
    std::vector<MessageType> buffer_;
};

}

#endif // COLLECTION_NODE_H
