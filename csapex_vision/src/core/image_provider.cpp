/// HEADER
#include <csapex_vision/image_provider.h>

/// COMPONENT
#include <csapex_vision/cv_mat_message.h>
#include <utils_param/parameter_factory.h>

/// SYSTEM
#include <boost/filesystem.hpp>

namespace bfs = boost::filesystem;

using namespace csapex;

std::map<std::string, ImageProvider::ProviderConstructor> ImageProvider::plugins;

ImageProvider::ImageProvider()
{
    setType(connection_types::CvMatMessage::make());

    state.addParameter(param::ParameterFactory::declareBool("playback/resend", true));
}

ImageProvider::~ImageProvider()
{;
}

bool ImageProvider::canHandle(const std::string& path,
                              boost::function<bool (ImageProvider*)> reference)
{
    bfs::path file(path);

    bool is_dir = bfs::is_directory(file) ;
    std::string ext = is_dir ? ".DIRECTORY" : file.extension().string();

    std::map<std::string, ProviderConstructor>::iterator handler = plugins.find(ext);

    if(handler == plugins.end()) {
        return false;
    }

    return reference(handler->second(""));
}


bool ImageProvider::canHandle(const std::string& path)
{
    bfs::path file(path);

    bool is_dir = bfs::is_directory(file) ;
    std::string ext = is_dir ? ".DIRECTORY" : file.extension().string();

    std::map<std::string, ProviderConstructor>::iterator handler = plugins.find(ext);

    return handler != plugins.end();
}

ImageProvider* ImageProvider::create(const std::string& path)
{
    bfs::path file(path);

    bool is_dir = bfs::is_directory(file) ;
    std::string ext = is_dir ? ".DIRECTORY" : file.extension().string();
    ProviderConstructor constructor = plugins[ext];

    std::cout << "extension is " << ext << std::endl;

    if(constructor.empty()) {
        return NULL;
    } else {
        ImageProvider* result = constructor(path);
        result->init();
        return result;
    }
}

void ImageProvider::init()
{
    doInit();
}

connection_types::Message::Ptr ImageProvider::next()
{
    cv::Mat mask;

    connection_types::CvMatMessage::Ptr msg(new connection_types::CvMatMessage(enc::unknown));

    next(msg->value, mask);

    msg->setEncoding((msg->value.channels() == 1) ? enc::mono : enc::bgr);

    return msg;
}

std::vector<std::string> ImageProvider::getExtensions() const
{
    return std::vector<std::string> ();
}

int ImageProvider::sleepTime()
{
    return 100;
}

void ImageProvider::enableBorder(bool border)
{

}


Memento::Ptr ImageProvider::getState() const
{
    GenericState::Ptr r(new GenericState(state));
    return r;
}

void ImageProvider::setState(Memento::Ptr memento)
{
    boost::shared_ptr<GenericState> m = boost::dynamic_pointer_cast<GenericState> (memento);
    if(m) {
        state.setFrom(*m);
    }
}
