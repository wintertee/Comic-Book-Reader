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
/// 2. call setName() to set its name (optional)
/// 3. call setPage() for each page to load page into ComicBook
/// 4. call scalePageAround(pageIdx) to generate scaled images indexed around pageIdx.
/// 5. call getPage()->getPixmap() to get final scaled QPixmap object.
class ComicBook : public QObject {

    Q_OBJECT

  public:
    ComicBook();
    ~ComicBook();

    void reset();

    SmartImage *getPage(unsigned int) const;

    unsigned int getSize() const;

    const QString &getName() const;

    void setFilter(const Magick::FilterType &filter);

    /// if range<0, scall all pages
    void scalePageAround(unsigned int pageIdx, double scaleFactor, int range = 4);
    void scalePageAround(unsigned int pageIdx, int win_w, int win_h, int range = 4);

    void setName(const QString &name);

  public slots:
    void setSize(unsigned int size);
    void setPage(std::vector<unsigned char> *page, int pageIdx, QString pageName);

  private:
    void waitUntilPageAvailable(unsigned int PageIdx) const;

    void scalePage(unsigned int pageIdx, double scale);
    void scalePage(unsigned int pageIdx, int win_w, int win_h);

    /// pointers to each page
    std::vector<SmartImage *> pages;

    /// status of page-loading/scaling for each page
    std::vector<QFuture<void>> qFutures;

    /// book name
    QString name;

    unsigned int size;
};

#endif // COMICBOOK_H
