#ifndef REGISTER_PLUGIN_H
#define REGISTER_PLUGIN_H

/// HEADER
#include <csapex/core/core_plugin.h>

namespace csapex
{
class RegisterPlugin : public CorePlugin
{
public:
    RegisterPlugin();

    void init(CsApexCore& core);
};

}  // namespace csapex

#endif  // REGISTER_PLUGIN_H
