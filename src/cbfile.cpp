#include "cbfile.h"
#include <QFileInfo>
#include <QMessageBox>

CBFile::CBFile() {}

CBFile::~CBFile() {
    if (zipExtractor != nullptr)
        delete zipExtractor;
    if (rarExtractor != nullptr)
        delete rarExtractor;
}

void CBFile::extract(const QString &fileName, ComicBook &comicBook) {
    bit7z::BitExtractor *extractor;
    QFileInfo fileInfo(fileName);
    QString extention = fileInfo.suffix();

    if (!QString::compare(extention, "cbz", Qt::CaseInsensitive)) {
        if (zipExtractor == nullptr)
            zipExtractor = new bit7z::BitExtractor{lib, bit7z::BitFormat::Zip};
        extractor = zipExtractor;
    } else if (!QString::compare(extention, "cbr", Qt::CaseInsensitive)) {
        if (rarExtractor == nullptr)
            rarExtractor = new bit7z::BitExtractor{lib, bit7z::BitFormat::Rar};
        extractor = rarExtractor;
    } else
        throw(std::runtime_error("unknown file type " + extention.toStdString()));

    extractor->extract(fileName.toStdWString(), comicBook.content);
    comicBook.initPages();
    comicBook.name = fileInfo.baseName();
}
