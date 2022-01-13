#include "smartimage.h"
#include <QDebug>

SmartImage::SmartImage() {}

void SmartImage::loadpage(const std::vector<unsigned char> *page) {
    blob.update(const_cast<void *>(reinterpret_cast<const void *>(page->data())), page->size());
    dirtyPixmap = true;
    dirtyImW = true;
    emptyIm = false;
}

void SmartImage::setScaleFactor(double scaleFactor) {
    if (!dirtyPixmap && this->scaleFactor == scaleFactor)
        return;
    this->scaleFactor = scaleFactor;
    if (scaleFactor == 1.0) {
        scaled_blob.update(const_cast<void *>(blob.data()), blob.length());
    } else {
        scaleImage();
    }
    loadPixmap(scaled_blob);
    dirtyPixmap = false;
}

double SmartImage::fitToWindow(int win_w, int win_h) {
    if (dirtyImW) {
        readGeo();
    }
    double scaleFactor_w = (double)win_w / im_w;
    double scaleFactor_h = (double)win_h / im_h;
    double scaleFactor = std::min(scaleFactor_w, scaleFactor_h) * 0.99;
    setScaleFactor(scaleFactor);
    return scaleFactor;
}

void SmartImage::setFilter(Magick::FilterType filter) { this->filter = filter; }

const QPixmap &SmartImage::getPixmap() { return pixmap; }

void SmartImage::loadPixmap(Magick::Blob &blob) {
    pixmap.loadFromData(reinterpret_cast<const unsigned char *>(blob.data()), static_cast<unsigned int>(blob.length()));
}

void SmartImage::scaleImage() {
    Magick::Image image;
    image.read(blob);
    image.filterType(filter);
    image.zoom(Magick::Geometry(image.columns() * scaleFactor, image.rows() * scaleFactor));
    image.write(&scaled_blob);
}

void SmartImage::readGeo() {
    Magick::Image image;
    image.read(blob);
    im_w = static_cast<int>(image.columns());
    im_h = static_cast<int>(image.rows());
    dirtyImW = false;
}
