#include "cbfile.h"
#include <QDebug>
#include <QFileInfo>
#include <QMessageBox>
#include <vector>

CBFile::CBFile() {}

CBFile::~CBFile() {}

void CBFile::extract(const QString &fileName, const QString &extention) {
    std::wstring wFileName = fileName.toStdWString();
    bit7z::BitExtractor const *extractor;
    bit7z::BitArchiveInfo *archiveInfo;

    if (!QString::compare(extention, "cbz", Qt::CaseInsensitive)) {
        extractor = &zipExtractor;
        archiveInfo = new bit7z::BitArchiveInfo{lib, wFileName, bit7z::BitFormat::Zip};
    } else if (!QString::compare(extention, "cbr", Qt::CaseInsensitive)) {
        extractor = &rarExtractor;
        archiveInfo = new bit7z::BitArchiveInfo{lib, wFileName, bit7z::BitFormat::Rar};
    } else
        throw(std::runtime_error("unknown file type " + extention.toStdString()));

    emit totalPages(archiveInfo->filesCount());
    qDebug() << "emit total pages" << archiveInfo->filesCount();

    for (auto &item : archiveInfo->items()) {
        if (!item.isDir()) {
            auto *out_buffer = new std::vector<unsigned char>;
            extractor->extract(wFileName, *out_buffer, item.index());
            emit extractPage(out_buffer);
            qDebug() << "emit extractPage" << out_buffer;
        }
    }
}
