#include "comicbook.h"
#include <QCoreApplication>
#include <QDebug>
#include <QThread>
#include <QTime>

ComicBook::ComicBook(QString name) : name(name), size(0) {}

void ComicBook::appendPage(std::vector<unsigned char> *page) {
    pages.push_back(page);
    qDebug() << "append page" << pages.size();
}

void ComicBook::setSize(unsigned int size) {
    qDebug() << "recive total pages" << size;
    this->size = size;
    pages.clear();
    pages.reserve(size);
    qDebug() << "setSize" << size;
}

const std::vector<unsigned char> *ComicBook::getPage(unsigned int index) const {
    while (index >= pages.size() || size == 0) {
        QTime dieTime = QTime::currentTime().addSecs(1);
        while (QTime::currentTime() < dieTime)
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
        qDebug() << "waiting for 1000ms";
    }
    return pages[index];
}

unsigned int ComicBook::getSize() const { return size; }

bool ComicBook::empty() const { return pages.empty(); }

const QString &ComicBook::getName() const { return name; };
