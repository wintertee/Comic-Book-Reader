#ifndef SMARTIMAGE_H
#define SMARTIMAGE_H

#include "Magick++.h"
#include <QPixmap>

class SmartImage {
  public:
    SmartImage();
    /// load page to blob
    void loadpage(const std::vector<unsigned char> *page);
    /// set scaleFactor and generate pixmap
    void setScaleFactor(double scaleFactor);
    /// calculate scaleFactor to fit to window size
    double fitToWindow(int win_w, int win_h);
    void setFilter(Magick::FilterType filter);
    const QPixmap &getPixmap();
    bool empty(){return emptyIm;}

  private:
    /// load pixmap from blob
    void loadPixmap(Magick::Blob &blob);
    /// scale blob to scaledBlod by scaleFactor
    void scaleImage();
    /// read blob geometry
    void readGeo();

    // data
    Magick::Blob blob, scaled_blob;
    QPixmap pixmap;
    /// pixmap is dirty
    bool dirtyPixmap = false;

    // param
    double scaleFactor = 0;
    Magick::FilterType filter;
    int im_w = 0, im_h = 0;
    /// image geometry is dirty
    bool dirtyImW = false;
    /// image is empty
    bool emptyIm = true;
};

#endif // SMARTIMAGE_H
