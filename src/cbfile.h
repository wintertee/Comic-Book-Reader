#ifndef CBFILE_H
#define CBFILE_H

#include "bitarchiveinfo.hpp"
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
    void extract(const QString &fileName, const QString &extention);

  signals:
    // need slot free page pointer
    void extractPage(std::vector<unsigned char> *page, int pageIdx);
    void totalPages(unsigned int);

  private:
    bit7z::Bit7zLibrary lib{L"7z.dll"};
    bit7z::BitExtractor zipExtractor{lib, bit7z::BitFormat::Zip};
    bit7z::BitExtractor rarExtractor{lib, bit7z::BitFormat::Rar};
};

#endif // CBFILE_H
