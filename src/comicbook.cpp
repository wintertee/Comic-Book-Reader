#include "comicbook.h"
#include <QDebug>

ComicBook::ComicBook() {}

void ComicBook::initPages() {
    pages.clear();
    pages.reserve(content.size());
    for (auto it = content.begin(); it != content.end(); ++it)
        pages.emplace_back(&it->second);
}

const std::vector<unsigned char> *ComicBook::getPage(unsigned int index) const { return pages[index]; }

size_t ComicBook::size() const { return pages.size(); }

bool ComicBook::empty() const { return pages.empty(); }

const QString &ComicBook::getName() const { return name; };
