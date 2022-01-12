#ifndef CBFILE_H
#define CBFILE_H

#include "bitexception.hpp"
#include "bitextractor.hpp"
#include "comicbook.h"
#include <QFileInfo>
#include <QString>

/// CBFile is to extract/compress comic book file
class CBFile {

  public:
    CBFile();
    ~CBFile();
    void extract(const QString &fileName, ComicBook &comicBook);

  private:
    bit7z::Bit7zLibrary lib{L"7z.dll"};
    bit7z::BitExtractor *zipExtractor = nullptr;
    bit7z::BitExtractor *rarExtractor = nullptr;
};

#endif // CBFILE_H
