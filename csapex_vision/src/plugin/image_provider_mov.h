#ifndef IMAGE_PROVIDER_BAG_H
#define IMAGE_PROVIDER_BAG_H

/// COMPONENT
#include "image_provider_set.h"

namespace csapex
{

class ImageProviderMov : public ImageProviderSet
{
public:
    ImageProviderMov();
    void load(const std::string& movie_file);

    ~ImageProviderMov();

public:
    virtual bool hasNext();
    virtual void reallyNext(cv::Mat& img, cv::Mat& mask);

    std::vector<std::string> getExtensions() const;

private:
    void display();

private:
    cv::VideoCapture capture_;
};

} /// NAMESPACE

#endif // IMAGE_PROVIDER_BAG_H
