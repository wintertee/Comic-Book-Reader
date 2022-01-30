#ifndef SMARTIMAGE_H
#define SMARTIMAGE_H

#include "Magick++.h"
#include <QPixmap>

/// For each instance, call loadpage() first to read original image data.
/// Then call either setScaleFactor() or fitToWindow() to generate scaled image and pixmap
/// Finally call getPixmap() to get final image in QPixmap format.
class SmartImage {
  public:
    SmartImage();
    /// load page to blob
    void loadpage(const std::vector<unsigned char> *page);
    /// set scaleFactor and generate pixmap
    void setScaleFactor(double scaleFactor);
    /// calculate scaleFactor to fit to window size
    void fitToWindow(int win_w, int win_h);
    double getScaleFactor() const;
    void setFilter(Magick::FilterType filter);
    const QPixmap &getPixmap() const;
    bool empty() const;

  private:
    /// generate pixmap from blob
    void genPixmap(Magick::Blob &blob);
    /// scale blob to scaledBlod by scaleFactor
    void scaleImage();
    /// read blob geometry
    //    void readGeo();

    // data
    Magick::Blob blob, scaled_blob;
    Magick::Image image;
    QPixmap pixmap;
    /// pixmap is dirty
    bool dirtyPixmap = true;
    bool dirtyImage = true;

    bool isNowFittedToWindow = false;
    int win_w = 0, win_h = 0;

    // param
    double scaleFactor = 1.0;
    int im_w = 0, im_h = 0;
    /// image is empty
};

#endif // SMARTIMAGE_H
