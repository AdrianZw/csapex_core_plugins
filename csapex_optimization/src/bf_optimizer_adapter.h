#ifndef BF_OPTIMIZER_ADAPTER_H
#define BF_OPTIMIZER_ADAPTER_H

/// PROJECT
#include <csapex_optimization/optimizer_adapter.h>

/// COMPONENT
#include "bf_optimizer.h"

class QDialog;
class QProgressBar;

namespace csapex
{
class BFOptimizerAdapter : public OptimizerAdapter
{
    Q_OBJECT

public:
    BFOptimizerAdapter(NodeFacadeImplementationPtr worker, NodeBox* parent, std::weak_ptr<BFOptimizer> node);
    ~BFOptimizerAdapter();

    void setupUi(QBoxLayout* layout) override;

    void parameterAdded(param::ParameterPtr p) override;

private:
    void triggerStep(int step);

protected:
    std::weak_ptr<BFOptimizer> wrapped_;

    QProgressBar* progress_;
};

}  // namespace csapex
#endif  // BF_OPTIMIZER_ADAPTER_H
