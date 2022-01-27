#ifndef COMICBOOK_H
#define COMICBOOK_H

#include <QObject>
#include <QString>
#include <map>
#include <string>
#include <vector>

class ComicBook : public QObject {

    Q_OBJECT

  public:
    ComicBook(QString name);

    /// return content.data()
    const std::vector<unsigned char> *getPage(unsigned int) const;

    /// return content.size()
    unsigned int getSize() const;

    /// return content.empty()
    bool empty() const;

    const QString &getName() const;

  public slots:
    void setSize(unsigned int size);
    void appendPage(std::vector<unsigned char> *page);

  private:
    /// pointers to each page
    std::vector<std::vector<unsigned char> *> pages;

    /// book name
    QString name;
    unsigned int size;
};

#endif // COMICBOOK_H
