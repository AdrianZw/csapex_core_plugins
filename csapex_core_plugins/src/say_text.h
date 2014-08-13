#ifndef TEXT_DISPLAY_H_
#define TEXT_DISPLAY_H_

/// PROJECT
#include <csapex/model/node.h>

/// SYSTEM
#include <QLabel>

namespace csapex {

class SayText : public Node
{
public:
    SayText();

    virtual void process();
    virtual void setup();

private:
    Input* connector_;
};

}

#endif // TEXT_DISPLAY_H_
