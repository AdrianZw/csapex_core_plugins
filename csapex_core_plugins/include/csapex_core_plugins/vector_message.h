#ifndef VECTOR_MESSAGE_H
#define VECTOR_MESSAGE_H

/// PROJECT
#include <csapex/msg/message.h>
#include <csapex/msg/message_traits.h>

/// SYSTEM
#include <string>

namespace csapex {
namespace connection_types {

struct VectorMessage : public Message
{
    typedef boost::shared_ptr<VectorMessage> Ptr;

    VectorMessage(const std::string& frame_id = "/");

    ConnectionType::Ptr getSubType() const;

    template <typename T>
    static void registerType()
    {
        // not used for this type
    }


    template <typename T>
    static VectorMessage::Ptr make()
    {
        return VectorMessage::Ptr (new VectorMessage(connection_types::makeEmpty<T>(), "/"));
    }

    static VectorMessage::Ptr make(ConnectionType::Ptr type)
    {
        return VectorMessage::Ptr (new VectorMessage(type->toType(), "/"));
    }

    static VectorMessage::Ptr make();

    virtual ConnectionType::Ptr clone();
    virtual ConnectionType::Ptr toType();

    virtual bool canConnectTo(const ConnectionType* other_side) const;
    virtual bool acceptsConnectionFrom(const ConnectionType *other_side) const;

private:
    VectorMessage(ConnectionType::Ptr type, const std::string& frame_id);

public:
    std::vector<ConnectionType::Ptr> value;

private:
    ConnectionType::Ptr type_;

};


struct GenericVectorMessage : public Message
{
private:

    struct EntryInterface : public Message
    {
        typedef boost::shared_ptr< EntryInterface > Ptr;
        static const Ptr NullPtr;

        EntryInterface(const std::string& name)
            : Message(name, "/")
        {
        }

        virtual ConnectionType::Ptr clone()
        {
            return cloneEntry();
        }

        virtual EntryInterface::Ptr cloneEntry() = 0;

        virtual void encode(YAML::Node& node) const = 0;
        virtual void decode(const YAML::Node& node) = 0;
    };

    template <typename T>
    struct Implementation : public EntryInterface
    {
    private:
        typedef Implementation<T> Self;

    public:
        typedef boost::shared_ptr< Self > Ptr;


    public:
        static Self::Ptr make() {
            return Self::Ptr (new Self);
        }

        Implementation()
            : EntryInterface(std::string("std::vector<") + type2nameWithoutNamespace(typeid(T)) + ">")
        {
            BOOST_STATIC_ASSERT(!boost::is_same<T, void*>::value);
        }

        virtual EntryInterface::Ptr cloneEntry()
        {
            Self::Ptr r(new Self);
            r->value = value;
            return r;
        }

        virtual ConnectionType::Ptr toType()
        {
            Self::Ptr r(new Self);
            return r;
        }

        virtual bool canConnectTo(const ConnectionType* other_side) const
        {
            const Self* vec = dynamic_cast<const Self*> (other_side);
            const VectorMessage* vec_deprecated =  dynamic_cast<const VectorMessage*> (other_side);
            if(vec_deprecated) {
                return false;
            }
            if(vec != 0) {
                return true;
            } else {
                return other_side->canConnectTo(this);
            }
        }
        virtual bool acceptsConnectionFrom(const ConnectionType *other_side) const
        {

            const Self* vec = dynamic_cast<const Self*> (other_side);
            return vec != 0;
        }

        void encode(YAML::Node& node) const
        {
            node["value_type"] = type2name(typeid(T));
            node["values"] = *value;
        }

        void decode(const YAML::Node& node)
        {
            value.reset(new std::vector<T>);
            *value = node["values"].as< std::vector<T> >();
        }

    public:
        boost::shared_ptr< std::vector<T> > value;
    };

    template <typename T>
    struct MessageImplementation : public Implementation<T>
    {
        typedef Implementation<T> Parent;
        typedef MessageImplementation<T> Self;

        using Parent::value;

    public:
        typedef boost::shared_ptr< Self > Ptr;

        static Self::Ptr make() {
            return Self::Ptr (new Self);
        }
    };

public:
    typedef boost::shared_ptr<GenericVectorMessage> Ptr;

    template <typename T>
    struct TypeMap {
        typedef std::vector<T> type;
        typedef boost::shared_ptr<type> Ptr;
        typedef boost::shared_ptr<type const> ConstPtr;
    };

    struct SupportedTypes {
        static SupportedTypes& instance() {
            static SupportedTypes i;
            return i;
        }
        static EntryInterface::Ptr make(const std::string& type) {
            return instance().map_.at(type)->cloneEntry();
        }

        template <typename T>
        static void registerType()
        {
            std::string type = type2name(typeid(T));
            std::map<std::string, EntryInterface::Ptr>& map = instance().map_;
            if(map.find(type) == map.end()) {
                std::cerr << "register " << type << " at map of size " << map.size() << std::endl;
                map[type].reset(new Implementation<T>());
            }
        }

    private:
        std::map<std::string, EntryInterface::Ptr> map_;
    };

    template <typename T>
    static void registerType()
    {
        SupportedTypes::registerType<T>();
    }

    template <typename T>
    static GenericVectorMessage::Ptr make(typename boost::enable_if<boost::is_base_of<ConnectionType, T> >::type* dummy = 0)
    {
        registerType<T>();
        return GenericVectorMessage::Ptr(new GenericVectorMessage(MessageImplementation<T>::make(), "/"));
    }

    template <typename T>
    static GenericVectorMessage::Ptr make(typename boost::disable_if<boost::is_base_of<ConnectionType, T> >::type* dummy = 0)
    {
        registerType<T>();
        return GenericVectorMessage::Ptr(new GenericVectorMessage(Implementation<T>::make(), "/"));
    }

    template <typename T>
    boost::shared_ptr<std::vector<T> const>  makeShared()
    {
        return boost::dynamic_pointer_cast< Implementation<T> > (impl)->value;
    }


    template <typename T>
    void set(const boost::shared_ptr< std::vector<T> > & v) {
        boost::dynamic_pointer_cast< Implementation<T> > (impl)->value = v;
    }

    void encode(YAML::Node& node) const
    {
        impl->encode(node);
    }

    void decode(const YAML::Node& node)
    {
        std::string type = node["value_type"].as<std::string>();
        impl = SupportedTypes::make(type);
        assert(impl);
        impl->decode(node);
    }


    virtual ConnectionType::Ptr clone();
    virtual ConnectionType::Ptr toType();

    virtual bool canConnectTo(const ConnectionType* other_side) const;
    virtual bool acceptsConnectionFrom(const ConnectionType *other_side) const;

private:
    GenericVectorMessage(EntryInterface::Ptr impl, const std::string &frame_id);

private:
    EntryInterface::Ptr impl;

};

template <>
struct type<GenericVectorMessage> {
    static std::string name() {
        return "Vector";
    }
};

template <>
inline boost::shared_ptr<GenericVectorMessage> makeEmpty<GenericVectorMessage>()
{
    return GenericVectorMessage::make<int>();
}

}
}


/// YAML
namespace YAML {
template<>
struct convert<csapex::connection_types::GenericVectorMessage> {
    static Node encode(const csapex::connection_types::GenericVectorMessage& rhs);
    static bool decode(const Node& node, csapex::connection_types::GenericVectorMessage& rhs);
};
template<>
struct convert<csapex::connection_types::VectorMessage> {
    static Node encode(const csapex::connection_types::VectorMessage& rhs);
    static bool decode(const Node& node, csapex::connection_types::VectorMessage& rhs);
};

template<>
struct convert<csapex::ConnectionType> {
    static Node encode(const csapex::ConnectionType& rhs);
    static bool decode(const Node& node, csapex::ConnectionType& rhs);
};
}

#endif // VECTOR_MESSAGE_H
