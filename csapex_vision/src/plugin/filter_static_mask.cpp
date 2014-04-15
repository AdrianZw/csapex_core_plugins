/// HEADER
#include "filter_static_mask.h"

/// PROJECT


/// SYSTEM
#include <QPixmap>
#include <QGraphicsSceneEvent>
#include <utils_qt/QtCvImageConverter.h>
#include <QKeyEvent>
#include <QPushButton>
#include <csapex/utility/register_apex_plugin.h>

CSAPEX_REGISTER_CLASS(csapex::FilterStaticMask, csapex::Node)

using namespace csapex;

ModalPainter::ModalPainter()
    : dragging(false)
{
}

ModalPainter::~ModalPainter()
{
    delete modal;
}

void ModalPainter::run()
{
    modal = new QDialog();
    modal->setModal(true);
    modal->resize(400, 400);

    reset = new QPushButton("reset");
    keep = new QPushButton("keep");

    view = new QGraphicsView;
    scene = new QGraphicsScene;
    view->setScene(scene);

    scene->installEventFilter(this);

    QBoxLayout* layout = new QVBoxLayout;
    QBoxLayout* sublayout = new QHBoxLayout;
    layout->addWidget(view);
    layout->addLayout(sublayout);
    sublayout->addWidget(reset);
    sublayout->addWidget(keep);

    modal->setLayout(layout);

    QObject::connect(reset, SIGNAL(clicked()), this, SLOT(reset_mask()));
    QObject::connect(keep, SIGNAL(clicked()), this, SLOT(publish_mask()));

    modal->show();
}

void ModalPainter::reset_mask()
{
    mask.release();
    mask_backup.release();
}

void ModalPainter::publish_mask()
{
    Q_EMIT modal->close();
    Q_EMIT new_mask(mask);
}

bool ModalPainter::eventFilter(QObject* obj, QEvent* event)
{
    if(event->type() == QEvent::KeyPress) {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if(keyEvent->key() == Qt::Key_Return) {
            Q_EMIT keep->click();
            return true;
        } else
            return false;
    }

    switch(event->type()) {
    case QEvent::GraphicsSceneMousePress: {
        QGraphicsSceneMouseEvent* me = static_cast<QGraphicsSceneMouseEvent*>(event);

        if(dragging) {
            dragging = false;
            mask_backup.copyTo(mask);
            break;

        } else  {
            mask.copyTo(mask_backup);

            dragging = true;

            start_drag = me->scenePos();
            start_btn = me->button();
        }
        break;
    }
    case QEvent::GraphicsSceneMouseMove: {
        if(dragging) {
            QGraphicsSceneMouseEvent* me = static_cast<QGraphicsSceneMouseEvent*>(event);

            QPointF current = me->scenePos();
            cv::Rect rec(cv::Point(start_drag.x(), start_drag.y()),
                         cv::Point(current.x(), current.y()));
            mask_backup.copyTo(mask);
            cv::rectangle(mask, rec, cv::Scalar::all(0), 2);
        }
        break;
    }

    case QEvent::GraphicsSceneMouseRelease: {
        QGraphicsSceneMouseEvent* me = static_cast<QGraphicsSceneMouseEvent*>(event);

        mask_backup.copyTo(mask);

        if(me->button() == start_btn) {

            QPointF stop_drag = me->scenePos();

            cv::Rect rec(cv::Point(start_drag.x(), start_drag.y()),
                         cv::Point(stop_drag.x(), stop_drag.y()));

            cv::rectangle(mask, rec, cv::Scalar::all(0), CV_FILLED);
        }

        dragging = false;
        break;
    }

    default:
        break;
    }

    return QObject::eventFilter(obj, event);
}

void ModalPainter::setMask(cv::Mat mask)
{
    if(!mask.empty()) {
        mask.copyTo(this->mask);
    }
}

void ModalPainter::input(cv::Mat img)
{
    cv::Mat masked;
    if(mask.empty()) {
        mask = cv::Mat(img.rows, img.cols, CV_8UC1, cv::Scalar(255));
    }
    img.copyTo(masked, mask);

    QSharedPointer<QImage> image = QtCvImageConverter::Converter<QImage, QSharedPointer>::mat2QImage(masked);
    QGraphicsScene* scene = view->scene();
    scene->clear();
    scene->setSceneRect(0, 0, img.cols, img.rows);
    scene->addPixmap(QPixmap::fromImage(*image));

    view->fitInView(scene->itemsBoundingRect() ,Qt::KeepAspectRatio);
}

FilterStaticMask::FilterStaticMask()
    : painter(NULL), state(this)
{
}

FilterStaticMask::~FilterStaticMask()
{
    delete painter;
}

void FilterStaticMask::State::writeYaml(YAML::Emitter& out) const {
    out << YAML::Key << "rows" << YAML::Value << mask_.rows;
    out << YAML::Key << "cols" << YAML::Value << mask_.cols;

    assert(mask_.type() == CV_8UC1);

    QByteArray raw = qCompress(mask_.data, mask_.rows * mask_.cols);
    out << YAML::Key << "rawdata" << YAML::Value << raw.toBase64().data();
 }

void FilterStaticMask::State::readYaml(const YAML::Node& node) {
    if(exists(node, "rawdata")){
        int rows, cols;
        node["rows"] >> rows;
        node["cols"] >> cols;

        std::string data;
        node["rawdata"] >> data;
        QByteArray raw = qUncompress(QByteArray::fromBase64(data.data()));

        cv::Mat(rows, cols, CV_8UC1, raw.data()).copyTo(mask_);

    } else  if(exists(node, "mask")) {
        std::string file;
        node["mask"] >> file;
        mask_ = cv::imread(file, 0);
    }
}

void FilterStaticMask::showPainter()
{
    painter = new ModalPainter;
    painter->run();

    if(!state.mask_.empty()) {
        painter->setMask(state.mask_);
    }

    QObject::connect(this, SIGNAL(input(cv::Mat)), painter, SLOT(input(cv::Mat)));
    QObject::connect(painter, SIGNAL(new_mask(cv::Mat)), this, SLOT(new_mask(cv::Mat)));
}

void FilterStaticMask::new_mask(cv::Mat mask)
{
    mask.copyTo(state.mask_);
}

void FilterStaticMask::filter(cv::Mat& img, cv::Mat& mask)
{
    Q_EMIT input(img);

    if(state.mask_.size != img.size) {
        setError(true, "The mask has not the same size as the image size", EL_WARNING);
        return;
    }

    if(!state.mask_.empty()) {
        if(mask.empty() || mask.size != state.mask_.size) {
            state.mask_.copyTo(mask);
        } else {
            mask = cv::min(mask, state.mask_);
        }
    }
}

void FilterStaticMask::insert(QBoxLayout* layout)
{
    btn = new QPushButton("create mask");

    layout->addWidget(btn);

    QObject::connect(btn, SIGNAL(clicked()), this, SLOT(showPainter()), Qt::DirectConnection);
}

Memento::Ptr FilterStaticMask::getState() const
{
    return boost::shared_ptr<State>(new State(state));
}

void FilterStaticMask::setState(Memento::Ptr memento)
{
    boost::shared_ptr<State> m = boost::dynamic_pointer_cast<State> (memento);
    assert(m.get());

    state = *m;

    if(painter) {
        painter->setMask(state.mask_);
    }

    Q_EMIT filter_changed();
}
