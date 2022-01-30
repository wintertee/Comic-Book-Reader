#include "comicbook.h"
#include <QCoreApplication>
#include <QDebug>
#include <QtConcurrent>

ComicBook::ComicBook() { reset(); }
ComicBook::~ComicBook() { reset(); }

void ComicBook::waitUntilPageAvailable(unsigned int pageIdx) const {
    while (pageIdx >= size || qFutures[pageIdx].isCanceled() || !qFutures[pageIdx].isFinished()) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
}

void ComicBook::setPage(std::vector<unsigned char> *page, int pageIdx) {
    qFutures[pageIdx] = QtConcurrent::run([this, page, pageIdx] {
        if (pages[pageIdx] != nullptr) {
            delete pages[pageIdx];
        }
        pages[pageIdx] = new SmartImage();
        pages[pageIdx]->loadpage(page);
        pages[pageIdx]->setScaleFactor(0.5);
        delete page;
    });
}

void ComicBook::setSize(unsigned int size) {
    pages.assign(size, nullptr);
    QFuture<void> qFuture;
    qFutures.assign(size, qFuture);
    this->size = size;
}

void ComicBook::setName(const QString &name) { this->name = name; }

void ComicBook::reset() {
    setName("");
    for (auto &page : pages) {
        if (page != nullptr) {
            delete page;
        }
    }
    setSize(0);
}

const SmartImage *ComicBook::getPage(unsigned int index) const {
    waitUntilPageAvailable(index);
    return pages[index];
}

void ComicBook::scalePageAround(unsigned int pageIdx, double scaleFactor) {
    waitUntilPageAvailable(pageIdx);
    qFutures[pageIdx].then([&] { scalePage(pageIdx, scaleFactor); });
    for (unsigned int i = std::max(pageIdx, cacheScaleRange) - cacheScaleRange; i < pageIdx; ++i) {
        waitUntilPageAvailable(i);
        qFutures[pageIdx].then([&] { scalePage(i, scaleFactor); });
    }
    for (unsigned int i = pageIdx + 1; i < std::min(pageIdx + cacheScaleRange, getSize()); ++i) {
        waitUntilPageAvailable(i);
        qFutures[pageIdx].then([&] { scalePage(i, scaleFactor); });
    }
}

void ComicBook::scalePageAround(unsigned int pageIdx, int win_w, int win_h) {
    waitUntilPageAvailable(pageIdx);
    qFutures[pageIdx].then([&] { scalePage(pageIdx, win_w, win_h); });
    for (unsigned int i = std::max(pageIdx, cacheScaleRange) - cacheScaleRange; i < pageIdx; ++i) {
        waitUntilPageAvailable(i);
        qFutures[pageIdx].then([&] { scalePage(i, win_w, win_h); });
    }
    for (unsigned int i = pageIdx + 1; i < std::min(pageIdx + cacheScaleRange, getSize()); ++i) {
        waitUntilPageAvailable(i);
        qFutures[pageIdx].then([&] { scalePage(i, win_w, win_h); });
    }
}

void ComicBook::scalePage(unsigned int pageIdx, double scaleFactor) { pages[pageIdx]->setScaleFactor(scaleFactor); }

void ComicBook::scalePage(unsigned int pageIdx, int win_w, int win_h) { pages[pageIdx]->fitToWindow(win_w, win_h); }

void ComicBook::setFilter(const Magick::FilterType &filter) {
    for (unsigned int pageIdx = 0; pageIdx < getSize(); ++pageIdx) {
        waitUntilPageAvailable(pageIdx);
        qFutures[pageIdx].then([&] { pages[pageIdx]->setFilter(filter); });
    }
}

unsigned int ComicBook::getSize() const { return size; }

bool ComicBook::empty() const { return pages.empty(); }

const QString &ComicBook::getName() const { return name; };
