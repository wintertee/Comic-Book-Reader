#ifndef COMICBOOK_H
#define COMICBOOK_H

#include <QString>
#include <map>
#include <string>
#include <vector>

extern std::vector<bool> subComicBook;

class ComicBook {
  public:
    ComicBook();

    /// init pages
    void initPages();

    /// return content.data()
    const std::vector<unsigned char> *getPage(unsigned int) const;

    /// return content.size()
    size_t size() const;

    /// return content.empty()
    bool empty() const;

    const QString &getName() const;

    friend class CBFile;

  private:
    /// (filename, data) pair
    std::map<std::wstring, std::vector<unsigned char>> content;

    /// pointers to each page
    std::vector<std::vector<unsigned char> *> pages;

    /// book name
    QString name;
};

#endif // COMICBOOK_H
