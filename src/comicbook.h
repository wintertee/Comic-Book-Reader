#ifndef COMICBOOK_H
#define COMICBOOK_H

#include "Magick++.h"
#include "smartimage.h"
#include <QFuture>
#include <QObject>
#include <QString>
#include <map>
#include <string>
#include <vector>

/// To initialize a ComicBook instance:
/// 1. call setSize() to allocate enough memory
/// 2. call setTargetWindowSize() to
/// 2. call setPage() for each page to load page into ComicBook
/// 3.
class ComicBook : public QObject {

    Q_OBJECT

  public:
    ComicBook();
    ~ComicBook();

    /// return content.data()
    const SmartImage *getPage(unsigned int) const;

    /// return content.size()
    unsigned int getSize() const;

    /// return content.empty()
    bool empty() const;

    const QString &getName() const;

    // void setTargetWindowSize(int win_w, int win_h);

    void setFilter(const Magick::FilterType &filter);

    void reset();

    void scalePageAround(unsigned int pageIdx, double scaleFactor);
    void scalePageAround(unsigned int pageIdx, int win_w, int win_h);

    void scalePage(unsigned int pageIdx, double scale);
    void scalePage(unsigned int pageIdx, int win_w, int win_h);

    void setName(const QString &name);

    const static unsigned int cacheScaleRange = 3;

  public slots:
    void setSize(unsigned int size);
    void setPage(std::vector<unsigned char> *page, int pageIdx);

  private:
    void waitUntilPageAvailable(unsigned int PageIdx) const;

    /// pointers to each page
    std::vector<SmartImage *> pages;
    std::vector<QFuture<void>> qFutures;
    bool stopScalingFlage = false;

    /// book name
    QString name;

    Magick::FilterType filter;
    unsigned int size;

    // int win_w = 0, win_h = 0;
};

#endif // COMICBOOK_H
