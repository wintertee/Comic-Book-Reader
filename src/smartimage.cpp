#include "smartimage.h"
#include <QDir>
#include <QFile>

SmartImage::SmartImage() { setFilter(filter); }

void SmartImage::loadpage(const std::vector<unsigned char> *page, QString name) {
    blob.update(const_cast<void *>(reinterpret_cast<const void *>(page->data())), page->size());
    this->name = name;
    image.read(blob);
    im_w = static_cast<int>(image.columns());
    im_h = static_cast<int>(image.rows());
    dirtyImage = false;
    dirtyPixmap = true;
}

void SmartImage::setScaleFactor(double scaleFactor) {
    isNowFittedToWindow = false;
    if (!dirtyPixmap && this->scaleFactor == scaleFactor)
        return;
    this->scaleFactor = scaleFactor;
    if (scaleFactor == 1.0) {
        scaled_blob.update(const_cast<void *>(blob.data()), blob.length());
    } else {
        scaleImage();
    }
    genPixmap(scaled_blob);
    dirtyPixmap = false;
}

void SmartImage::fitToWindow(int win_w, int win_h) {
    if (isNowFittedToWindow && win_w == this->win_w && win_h == this->win_h) {
        return;
    }
    this->win_w = win_w;
    this->win_h = win_h;

    double scaleFactor_w = (double)win_w / im_w;
    double scaleFactor_h = (double)win_h / im_h;
    double minScaleFactor = std::min(scaleFactor_w, scaleFactor_h) * 0.99;
    setScaleFactor(minScaleFactor);
    isNowFittedToWindow = true;
}

double SmartImage::getScaleFactor() const { return scaleFactor; }

void SmartImage::setFilter(Magick::FilterType filter) {
    if (this->filter != filter) {
        dirtyPixmap = true; // should use new filter to re-generate new pixmap
        this->filter = filter;
    }
    image.filterType(filter);
}

const QPixmap &SmartImage::getPixmap() const { return pixmap; }

void SmartImage::genPixmap(Magick::Blob &blob) {
    pixmap.loadFromData(reinterpret_cast<const unsigned char *>(blob.data()), static_cast<unsigned int>(blob.length()));
}

void SmartImage::scaleImage() {
    if (dirtyImage) {
        image.read(blob);
    }
    image.zoom(Magick::Geometry(image.columns() * scaleFactor, image.rows() * scaleFactor));
    dirtyImage = true;
    image.write(&scaled_blob);
}

void SmartImage::save(QString &relativePath) {
    QFile imageFile(relativePath + "/" + name);
    imageFile.open(QIODevice::WriteOnly);
    imageFile.write(reinterpret_cast<const char *>(blob.data()), blob.length());
    imageFile.close();
}
