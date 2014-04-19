/// HEADER
#include "export_cout.h"

/// PROJECT
#include <csapex/model/connector_in.h>
#include <csapex/utility/stream_interceptor.h>
#include <csapex/model/message.h>

/// SYSTEM
#include <csapex/utility/register_apex_plugin.h>

CSAPEX_REGISTER_CLASS(csapex::ExportCout, csapex::Node)

using namespace csapex;

ExportCout::ExportCout()
    : connector_(NULL)
{
    addTag(Tag::get("ConsoleIO"));
    addTag(Tag::get("General"));
    addTag(Tag::get("Output"));
}

QIcon ExportCout::getIcon() const
{
    return QIcon(":/terminal.png");
}

void ExportCout::setup()
{
    setSynchronizedInputs(true);

    connector_ = addInput<connection_types::AnyMessage>("Anything");
}

void ExportCout::process()
{
    ConnectionType::Ptr msg = connector_->getMessage<ConnectionType>();

    aout << "writing to cout: ";
    msg->write(aout);
    aout << std::endl;

    StreamInterceptor::instance().cout << *msg << "\n---" << std::endl;
}
