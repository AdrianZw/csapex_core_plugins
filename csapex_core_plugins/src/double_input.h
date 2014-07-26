#ifndef DOUBLE_INPUT_H
#define DOUBLE_INPUT_H

/// PROJECT
#include <csapex/model/node.h>

/// SYSTEM
#include <QDoubleSpinBox>

namespace csapex {

template <typename T>
class NumberInput : public Node
{
public:
    NumberInput();

    void tick();
    void setup();
    void process();

private:
    Output* out_;
};

}

#endif // DOUBLE_INPUT_H
