#include "comicbook.h"
#include <QCoreApplication>
#include <QtConcurrent>

ComicBook::ComicBook() { reset(); }
ComicBook::~ComicBook() { reset(); }

void ComicBook::waitUntilPageAvailable(unsigned int pageIdx) const {
    while (pageIdx >= size || qFutures[pageIdx].isCanceled() || !qFutures[pageIdx].isFinished()) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
}

void ComicBook::setPage(std::vector<unsigned char> *page, int pageIdx, QString pageName) {
    qFutures[pageIdx] = QtConcurrent::run([this, page, pageIdx, pageName] {
        if (pages[pageIdx] != nullptr) {
            delete pages[pageIdx];
        }
        pages[pageIdx] = new SmartImage();
        pages[pageIdx]->loadpage(page, pageName);
        pages[pageIdx]->setScaleFactor(1.0);
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

SmartImage *ComicBook::getPage(unsigned int index) const {
    waitUntilPageAvailable(index);
    return pages[index];
}

void ComicBook::scalePageAround(unsigned int pageIdx, double scaleFactor, int range) {
    if (range < 0) {
        for (unsigned int i = 0; i < getSize(); ++i) {
            waitUntilPageAvailable(i);
            qFutures[pageIdx].then([&] { scalePage(i, scaleFactor); });
        }
    } else {
        waitUntilPageAvailable(pageIdx);
        qFutures[pageIdx].then([&] { scalePage(pageIdx, scaleFactor); });
        if (range != 0) {
            unsigned int startIdx = std::max(pageIdx, static_cast<unsigned int>(range)) - range;
            for (unsigned int i = startIdx; i < pageIdx; ++i) {
                waitUntilPageAvailable(i);
                qFutures[pageIdx].then([&] { scalePage(i, scaleFactor); });
            }
            unsigned int endIdx = std::min(pageIdx + range, getSize());
            for (unsigned int i = pageIdx + 1; i < endIdx; ++i) {
                waitUntilPageAvailable(i);
                qFutures[pageIdx].then([&] { scalePage(i, scaleFactor); });
            }
        }
    }
}

void ComicBook::scalePageAround(unsigned int pageIdx, int win_w, int win_h, int range) {
    if (range < 0) {
        for (unsigned int i = 0; i < getSize(); ++i) {
            waitUntilPageAvailable(i);
            qFutures[pageIdx].then([&] { scalePage(i, win_w, win_h); });
        }
    } else {
        waitUntilPageAvailable(pageIdx);
        qFutures[pageIdx].then([&] { scalePage(pageIdx, win_w, win_h); });
        if (range != 0) {
            unsigned int startIdx = range < 0 ? 0 : std::max(pageIdx, static_cast<unsigned int>(range)) - range;
            for (unsigned int i = startIdx; i < pageIdx; ++i) {
                waitUntilPageAvailable(i);
                qFutures[pageIdx].then([&] { scalePage(i, win_w, win_h); });
            }
            unsigned int endIdx = range < 0 ? getSize() : std::min(pageIdx + range, getSize());
            for (unsigned int i = pageIdx + 1; i < endIdx; ++i) {
                waitUntilPageAvailable(i);
                qFutures[pageIdx].then([&] { scalePage(i, win_w, win_h); });
            }
        }
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

const QString &ComicBook::getName() const { return name; };
