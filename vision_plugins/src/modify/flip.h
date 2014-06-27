#ifndef FLIP_H
#define FLIP_H
/// COMPONENT
#include <csapex/model/node.h>

namespace vision_plugins {
class Flip : public csapex::Node
{
public:
    Flip();

    virtual void process();
    virtual void setup();
    virtual void setupParameters();

protected:
    csapex::ConnectorOut* output_;
    csapex::ConnectorIn*  input_;

    int mode_;

    void update();
};
}

#endif // FLIP_H
