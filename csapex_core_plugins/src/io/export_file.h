#ifndef EXPORT_FILE_H_
#define EXPORT_FILE_H_

/// PROJECT
#include <csapex/model/node.h>
#include <csapex/model/token.h>
#include <csapex_core_plugins/vector_message.h>

namespace csapex {

class ExportFile : public Node
{
public:
    ExportFile();

    virtual void setup(csapex::NodeModifier& node_modifier) override;
    virtual void setupParameters(Parameterizable& parameters);
    virtual void process() override;

protected:
    void setExportPath();

    void exportVector(const connection_types::VectorMessage::ConstPtr& vector);
    void exportSingle(const Token::ConstPtr& msg);

private:
    Input* connector_;

    std::string path_;
    std::string base_;

    int suffix_;

    bool oneshot_;
    bool oneshot_allowed_;
};

}

#endif // EXPORT_FILE_H_
