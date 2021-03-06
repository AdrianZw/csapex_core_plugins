/// HEADER
#include "output_display_adapter_remote.h"

/// PROJECT
#include <csapex/io/raw_message.h>
#include <csapex/model/node.h>
#include <csapex/model/node_facade_impl.h>
#include <csapex/model/node_facade_proxy.h>
#include <csapex/model/node_state.h>
#include <csapex/msg/io.h>
#include <csapex/utility/assert.h>
#include <csapex/view/utility/register_node_adapter.h>

/// COMPONENT
#include "output_display.h"

/// SYSTEM
#include <QBuffer>
#include <QEvent>
#include <QImageReader>
#include <QPushButton>

using namespace csapex;

CSAPEX_REGISTER_NODE_ADAPTER(OutputDisplayAdapter, csapex::OutputDisplay)

OutputDisplayAdapter::OutputDisplayAdapter(NodeFacadePtr node, NodeBox* parent) : ResizableNodeAdapter(node, parent)
{
    observe(node->raw_data_connection, [this](StreamableConstPtr msg) {
        if (std::shared_ptr<RawMessage const> raw = std::dynamic_pointer_cast<RawMessage const>(msg)) {
            std::vector<uint8_t> data = raw->getData();
            QByteArray array(reinterpret_cast<const char*>(data.data()), static_cast<int>(data.size()));

            QBuffer buffer(&array);
            buffer.open(QIODevice::ReadOnly);

            QImageReader reader(&buffer, "JPG");
            QImage image = reader.read();

            if (!image.isNull()) {
                displayRequest(image);

            } else {
                throw std::runtime_error("Invalid image received!");
            }
        }
    });
}

OutputDisplayAdapter::~OutputDisplayAdapter()
{
    stopObserving();
}

bool OutputDisplayAdapter::eventFilter(QObject* o, QEvent* e)
{
    (void)o;
    if (e->type() == QEvent::Resize) {
        QSize s = label_view_->sizeHint();
        setSize(s.width(), s.height());
    }

    return false;
}

void OutputDisplayAdapter::resize(const QSize& size)
{
    label_view_->setSize(size);
}

void OutputDisplayAdapter::setupUi(QBoxLayout* layout)
{
    label_view_ = new ImageWidget;
    label_view_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    label_view_->installEventFilter(this);

    layout->addWidget(label_view_);

    QHBoxLayout* sub = new QHBoxLayout;

    QPushButton* fit = new QPushButton("resize to content");
    sub->addWidget(fit, 0, Qt::AlignLeft);
    QObject::connect(fit, SIGNAL(clicked()), this, SLOT(fitInView()));

    sub->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));

    layout->addLayout(sub);

    connect(this, &OutputDisplayAdapter::displayRequest, this, &OutputDisplayAdapter::display);

    if (NodeFacadePtr nf = node_.lock()) {
        if (param::ParameterPtr p = nf->getParameter("jpg/quality")) {
            p->setHidden(false);
        }
    }

    ResizableNodeAdapter::setupUi(layout);
}

void OutputDisplayAdapter::setManualResize(bool manual)
{
    label_view_->setManualResize(manual);
}

void OutputDisplayAdapter::fitInView()
{
    setSize(last_image_size_.width(), last_image_size_.height());

    doResize();
}

void OutputDisplayAdapter::display(const QImage& img)
{
    last_image_size_ = img.size();
    label_view_->setPixmap(QPixmap::fromImage(img));
    label_view_->repaint();
}
