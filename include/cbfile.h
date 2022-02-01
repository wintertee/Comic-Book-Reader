#ifndef CBFILE_H
#define CBFILE_H

#include "bitarchiveinfo.hpp"
#include "bitcompressor.hpp"
#include "bitexception.hpp"
#include "bitextractor.hpp"
#include "comicbook.h"
#include <QFileInfo>
#include <QObject>
#include <QString>

/// CBFile is to extract/compress comic book file
class CBFile : public QObject {

    Q_OBJECT

  public:
    CBFile();
    ~CBFile();

    /// First send totalPages signal, then extract QComicBook by sending extractPage signal page by page.
    void extract(const QString &fileName, const QString &extention);
    void compress(const QString &inDir, const QString &outArchive);

  signals:

    /// send page and its index after extracted. The slot function should the free page pointer.
    void extractPage(std::vector<unsigned char> *page, int pageIdx, QString pageName);

    /// send totalpages of a file.
    void totalPages(unsigned int);

  private:
    bit7z::Bit7zLibrary lib{L"7z.dll"};
    bit7z::BitExtractor zipExtractor{lib, bit7z::BitFormat::Zip};
    bit7z::BitExtractor rarExtractor{lib, bit7z::BitFormat::Rar};
    bit7z::BitCompressor zipCompressor{lib, bit7z::BitFormat::Zip};
};

#endif // CBFILE_H
