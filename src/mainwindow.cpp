#include "mainwindow.h"

#include <QApplication>
#include <QClipboard>
#include <QColorSpace>
#include <QDir>
#include <QFileDialog>
#include <QImageReader>
#include <QImageWriter>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QMimeData>
#include <QMouseEvent>
#include <QPainter>
#include <QScreen>
#include <QScrollArea>
#include <QScrollBar>
#include <QStandardPaths>
#include <QStatusBar>

#include "Magick++.h"
#include "bitarchiveinfo.hpp"
#include "bitexception.hpp"
#include "qlabel.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {

    bit7z::Bit7zLibrary lib(L"7z.dll");
    image.filterType(filter); // default imagisk filter

    // menu bar
    createActions();
    updateActions();

    imageLabel = new QLabel(this);

    scrollArea = new QScrollArea(this);
    // scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget(imageLabel);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    scrollArea->setVisible(false);
    setCentralWidget(scrollArea);

    resize(QGuiApplication::primaryScreen()->availableSize() * 3 / 5);
    showMaximized();
}

MainWindow::~MainWindow() {}

void MainWindow::open() {
    // open file dialog
    QString fileName = QFileDialog::getOpenFileName(this, "Open File", "", "Comic files (*.cbz *.cbr)");
    if (fileName.isEmpty() || fileName.isNull())
        return;
    fileInfo = QFileInfo(fileName);
    QString extention = fileInfo.suffix();

    // load extractor
    bit7z::BitExtractor *extractor = nullptr;
    bit7z::Bit7zLibrary lib{L"7z.dll"};
    if (!QString::compare(extention, "cbz", Qt::CaseInsensitive))
        extractor = new bit7z::BitExtractor{lib, bit7z::BitFormat::Zip};
    else if (!QString::compare(extention, "cbr", Qt::CaseInsensitive))
        extractor = new bit7z::BitExtractor{lib, bit7z::BitFormat::Rar};
    else
        throw(bit7z::BitException::runtime_error("unknown file type " + extention.toStdString()));

    // decompression
    std::map<std::wstring, std::vector<byte_t>> filename_filedata_map;
    extractor->extract(fileName.toStdWString(), filename_filedata_map);
    for (auto it = filename_filedata_map.begin(); it != filename_filedata_map.end(); ++it)
        comicbook.append_content(it->second);

    // load first image
    current = 0;
    blobRead(current);
    scrollArea->setVisible(true);
    loadScaledImage(0);

    if (extractor != nullptr)
        delete extractor;
}

void MainWindow::blobRead(size_t idx) { blob.update(comicbook.get_content()[idx].data(), comicbook.get_content()[idx].size()); }

void MainWindow::loadImageFromBlob(const Magick::Blob &blob) {
    pixmap.loadFromData((const uchar *)blob.data(), (unsigned int)blob.length());
    imageLabel->setPixmap(pixmap);
    imageLabel->adjustSize();

    updateActions();
    updateTitle();
}

void MainWindow::loadScaledImage(double factor) {
    image.read(blob);
    if (factor == SCALE_TO_WINDOW) {

        int w = scrollArea->width();
        int h = scrollArea->height();
        int image_w = (int)image.columns();
        int image_h = (int)image.rows();

        double scaleFactor_w = (double)w / image_w * 0.99;
        double scaleFactor_h = (double)h / image_h * 0.99;
        scaleFactor = std::min(scaleFactor_w, scaleFactor_h);
    } else {
        scaleFactor *= factor;
    }

    image.zoom(Magick::Geometry(image.columns() * scaleFactor, image.rows() * scaleFactor));
    image.write(&scaled_blob);
    loadImageFromBlob(scaled_blob);

    adjustScrollBar(scrollArea->horizontalScrollBar(), factor);
    adjustScrollBar(scrollArea->verticalScrollBar(), factor);

    zoomInAct->setEnabled(scaleFactor < 5.0);
    zoomOutAct->setEnabled(scaleFactor > 0.2);
}

void MainWindow::nextPage() { changePage(1); }
void MainWindow::lastPage() { changePage(-1); }

void MainWindow::mousePressEvent(QMouseEvent *event) {
    auto x = event->position().x();
    auto y = event->position().y();
    if (scrollArea->geometry().contains(x, y)) {
        if (x < scrollArea->width() / 3)
            lastPage();
        else if (x > scrollArea->width() / 3 * 2)
            nextPage();
    }
}

void MainWindow::changePage(int relative_to) {
    int target_index = current + relative_to;
    if (target_index < 0)
        return;
    if (target_index >= (int)comicbook.size())
        return;
    current = target_index;
    blobRead(current);
    loadScaledImage(SCALE_TO_WINDOW);
}

void MainWindow::zoomIn() { loadScaledImage(1.25); }

void MainWindow::zoomOut() { loadScaledImage(0.8); }

void MainWindow::normalSize() {
    scaleFactor = 1.0;
    loadImageFromBlob(blob);
}

void MainWindow::setFilter(Magick::FilterType newFilter) { filter = newFilter; }

void MainWindow::about() { QMessageBox::about(this, tr("About CBR"), tr("CBR is Comic Book Reader")); }

void MainWindow::createActions() {
    fileMenu = menuBar()->addMenu(tr("&File"));

    QAction *openAct = fileMenu->addAction(tr("&Open..."), this, &MainWindow::open);
    openAct->setShortcut(QKeySequence::Open);

    fileMenu->addSeparator();

    QAction *exitAct = fileMenu->addAction(tr("E&xit"), this, &QWidget::close);
    exitAct->setShortcut(tr("Ctrl+Q"));

    QMenu *viewMenu = menuBar()->addMenu(tr("&View"));

    nextPageAct = viewMenu->addAction(tr("Next Page"), this, &MainWindow::nextPage);
    nextPageAct->setShortcut(QKeySequence::MoveToNextChar);

    lastPageAct = viewMenu->addAction(tr("Last page"), this, &MainWindow::lastPage);
    lastPageAct->setShortcut(QKeySequence::MoveToPreviousChar);

    viewMenu->addSeparator();

    zoomInAct = viewMenu->addAction(tr("Zoom &In (25%)"), this, &MainWindow::zoomIn);
    zoomInAct->setShortcut(QKeySequence::ZoomIn);

    zoomOutAct = viewMenu->addAction(tr("Zoom &Out (25%)"), this, &MainWindow::zoomOut);
    zoomOutAct->setShortcut(QKeySequence::ZoomOut);

    normalSizeAct = viewMenu->addAction(tr("&Normal Size"), this, &MainWindow::normalSize);
    normalSizeAct->setShortcut(tr("Ctrl+S"));

    fullScreenAct = viewMenu->addAction(tr("&Full Screen"), this, &MainWindow::fullScreen);
    fullScreenAct->setEnabled(true);
    fullScreenAct->setCheckable(true);
    fullScreenAct->setShortcut(QKeySequence::FullScreen);

    viewMenu->addSeparator();

    QMenu *filterMenu = viewMenu->addMenu(tr("&Filter"));

    pointFilterAct = filterMenu->addAction(tr("PointFilter"), this, [=]() { MainWindow::setFilter(Magick::PointFilter); });
    boxFilterAct = filterMenu->addAction(tr("BoxFilter"), this, [=]() { MainWindow::setFilter(Magick::BoxFilter); });
    triangleFilterAct = filterMenu->addAction(tr("TriangleFilter"), this, [=]() { MainWindow::setFilter(Magick::TriangleFilter); });
    sincFilterAct = filterMenu->addAction(tr("SincFilter"), this, [=]() { MainWindow::setFilter(Magick::SincFilter); });
    lanczosFilterAct = filterMenu->addAction(tr("LanczosFilter"), this, [=]() { MainWindow::setFilter(Magick::LanczosFilter); });

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));

    helpMenu->addAction(tr("&About"), this, &MainWindow::about);
    helpMenu->addAction(tr("About &Qt"), this, &QApplication::aboutQt);
}

void MainWindow::updateActions() {
    bool empty = comicbook.empty();
    zoomInAct->setEnabled(!empty);
    zoomOutAct->setEnabled(!empty);
    normalSizeAct->setEnabled(!empty);
    nextPageAct->setEnabled(!empty && current != comicbook.size() - 1);
    lastPageAct->setEnabled(!empty && current != 0);
}

void MainWindow::updateTitle() {
    setWindowTitle(QString("CBR - %1 (%2/%3)    %4 %")
                       .arg(fileInfo.baseName(), QString::number(current + 1), QString::number(comicbook.size()), QString::number(scaleFactor * 100, 'f', 2)));
}

void MainWindow::adjustScrollBar(QScrollBar *scrollBar, double factor) {
    scrollBar->setValue(int(factor * scrollBar->value() + ((factor - 1) * scrollBar->pageStep() / 2)));
}

void MainWindow::fullScreen() {
    bool fullScreen = fullScreenAct->isChecked();
    if (fullScreen)
        showFullScreen();
    else
        showMaximized();
}
