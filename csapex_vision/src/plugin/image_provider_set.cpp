/// HEADER
#include "image_provider_set.h"

/// SYSTEM
#include <QHBoxLayout>

using namespace csapex;

ImageProviderSet::ImageProviderSet()
    : next_frame(-1)
{
    state.playing_ = true;
    state.current_frame = 0;
}

ImageProviderSet::~ImageProviderSet()
{
}

void ImageProviderSet::insert(QBoxLayout* layout)
{
    slider_ = new QSlider(Qt::Horizontal);
    slider_->setMaximum(frames_);

    play_pause_ = new QPushButton(tr("Play / Pause"));
    play_pause_->setCheckable(true);
    play_pause_->setChecked(state.playing_);

    layout->addWidget(play_pause_);
    layout->addWidget(slider_);


    QObject::connect(slider_, SIGNAL(sliderMoved(int)), this, SLOT(showFrame()));
    QObject::connect(slider_, SIGNAL(valueChanged(int)), this, SLOT(showFrame()));
    QObject::connect(play_pause_, SIGNAL(toggled(bool)), this, SLOT(setPlaying(bool)));
}

void ImageProviderSet::update_gui(QFrame* additional_holder)
{
    QHBoxLayout* layout = new QHBoxLayout(additional_holder);

    insert(layout);

    additional_holder->setLayout(layout);
}

void ImageProviderSet::next(cv::Mat& img, cv::Mat& mask)
{
    cv::Mat i;
    if(state.playing_ || next_frame != -1) {
        reallyNext(i, mask);
        next_frame = -1;

    } else {
        i = last_frame_;
    }
    i.copyTo(img);
}

void ImageProviderSet::setPlaying(bool playing)
{
    state.playing_ = playing;
    if(play_pause_->isChecked() != playing) {
        play_pause_->setChecked(playing);
    }
}

void ImageProviderSet::showFrame()
{
    int frame = slider_->value();

    if(frame == state.current_frame) {
        return;
    }
    std::cout << "skip to frame " << frame << " / " << frames_ << std::endl;

    next_frame = frame;
}

Memento::Ptr ImageProviderSet::getState() const
{
    return boost::shared_ptr<State>(new State(state));
}

void ImageProviderSet::setState(Memento::Ptr memento)
{
    boost::shared_ptr<State> m = boost::dynamic_pointer_cast<State> (memento);
    assert(m.get());

    state = *m;

    state.current_frame = 0;
    slider_->setValue(m->current_frame);
    setPlaying(state.playing_);
}
