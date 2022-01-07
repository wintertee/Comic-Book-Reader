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
    ComicBook comicbook;
    createActions();

    imageLabel = new QLabel(this);
    imageLabel->setBackgroundRole(QPalette::Base);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setScaledContents(true);

    scrollArea = new QScrollArea(this);
    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget(imageLabel);
    scrollArea->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    scrollArea->setVisible(false);
    setCentralWidget(scrollArea);

    resize(QGuiApplication::primaryScreen()->availableSize() * 3 / 5);
}

MainWindow::~MainWindow() {}

void MainWindow::createActions() {
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));

    QAction *openAct = fileMenu->addAction(tr("&Open..."), this, &MainWindow::open);
    openAct->setShortcut(QKeySequence::Open);

    fileMenu->addSeparator();

    QAction *exitAct = fileMenu->addAction(tr("E&xit"), this, &QWidget::close);
    exitAct->setShortcut(tr("Ctrl+Q"));

    QMenu *viewMenu = menuBar()->addMenu(tr("&View"));

    nextPageAct = viewMenu->addAction(tr("Next Page"), this, &MainWindow::nextPage);
    nextPageAct->setShortcut(QKeySequence::MoveToNextChar);
    nextPageAct->setEnabled(false);

    lastPageAct = viewMenu->addAction(tr("Last page"), this, &MainWindow::lastPage);
    lastPageAct->setShortcut(QKeySequence::MoveToPreviousChar);
    lastPageAct->setEnabled(false);

    viewMenu->addSeparator();

    zoomInAct = viewMenu->addAction(tr("Zoom &In (25%)"), this, &MainWindow::zoomIn);
    zoomInAct->setShortcut(QKeySequence::ZoomIn);
    zoomInAct->setEnabled(false);

    zoomOutAct = viewMenu->addAction(tr("Zoom &Out (25%)"), this, &MainWindow::zoomOut);
    zoomOutAct->setShortcut(QKeySequence::ZoomOut);
    zoomOutAct->setEnabled(false);

    normalSizeAct = viewMenu->addAction(tr("&Normal Size"), this, &MainWindow::normalSize);
    normalSizeAct->setShortcut(tr("Ctrl+S"));
    normalSizeAct->setEnabled(false);

    QMenu *filterMenu = viewMenu->addMenu(tr("&Filter"));

    pointFilterAct = filterMenu->addAction(tr("PointFilter"), this, [=]() { MainWindow::setFilter(Magick::PointFilter); });

    boxFilterAct = filterMenu->addAction(tr("BoxFilter"), this, [=]() { MainWindow::setFilter(Magick::BoxFilter); });

    triangleFilterAct = filterMenu->addAction(tr("TriangleFilter"), this, [=]() { MainWindow::setFilter(Magick::TriangleFilter); });

    sincFilterAct = filterMenu->addAction(tr("SincFilter"), this, [=]() { MainWindow::setFilter(Magick::SincFilter); });

    lanczosFilterAct = filterMenu->addAction(tr("LanczosFilter"), this, [=]() { MainWindow::setFilter(Magick::LanczosFilter); });

    viewMenu->addSeparator();

    fitToWindowAct = viewMenu->addAction(tr("&Fit to Window"), this, &MainWindow::fitToWindow);
    fitToWindowAct->setEnabled(false);
    fitToWindowAct->setCheckable(true);
    fitToWindowAct->setShortcut(tr("Ctrl+F"));

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));

    helpMenu->addAction(tr("&About"), this, &MainWindow::about);
    helpMenu->addAction(tr("About &Qt"), this, &QApplication::aboutQt);
}

void MainWindow::open() {
    bit7z::BitExtractor *extractor = nullptr;

    QString fileName = QFileDialog::getOpenFileName(this, "Open File", "", "Comic files (*.cbz *.cbr)");
    fileInfo = QFileInfo(fileName);
    QString extention = fileInfo.suffix();

    bit7z::Bit7zLibrary lib{L"7z.dll"};
    if (!QString::compare(extention, "cbz", Qt::CaseInsensitive))
        extractor = new bit7z::BitExtractor{lib, bit7z::BitFormat::Zip};
    else if (!QString::compare(extention, "cbr", Qt::CaseInsensitive))
        extractor = new bit7z::BitExtractor{lib, bit7z::BitFormat::Rar};
    else
        throw(bit7z::BitException::runtime_error("unknown file type " + extention.toStdString()));

    extractor->extract(fileName.toStdWString(), comicbook.map);
    comicbook.it = comicbook.map.begin();
    size = comicbook.map.size();
    current = 1;
    updateImage();

    if (extractor != nullptr)
        delete extractor;
}

void MainWindow::updateImage() {
    image.loadFromData(comicbook.it->second.data(), comicbook.it->second.size());
    if (image.colorSpace().isValid())
        image.convertToColorSpace(QColorSpace::SRgb);
    imageLabel->setPixmap(QPixmap::fromImage(image));
    scaleFactor = 1.0;

    scrollArea->setVisible(true);
    fitToWindowAct->setEnabled(true);
    updateActions();

    if (!fitToWindowAct->isChecked())
        imageLabel->adjustSize();

    setWindowTitle(QString("CBR - %1 (%2/%3)").arg(fileInfo.baseName(), QString::number(current), QString::number(size)));
}

void MainWindow::nextPage() {
    comicbook.it++;
    current++;
    updateImage();
}
void MainWindow::lastPage() {
    comicbook.it--;
    current--;
    updateImage();
}

void MainWindow::zoomIn() { scaleImage(1.25); }

void MainWindow::zoomOut() { scaleImage(0.8); }

void MainWindow::normalSize() {
    imageLabel->adjustSize();
    scaleFactor = 1.0;
}

void MainWindow::setFilter(Magick::FilterType newFilter) { filter = newFilter; }

void MainWindow::fitToWindow() {
    bool fitToWindow = fitToWindowAct->isChecked();
    scrollArea->setWidgetResizable(fitToWindow);
    if (!fitToWindow)
        normalSize();
    updateActions();
}

void MainWindow::about() { QMessageBox::about(this, tr("About CBR"), tr("CBR is Comic Book Reader")); }

void MainWindow::updateActions() {
    zoomInAct->setEnabled(!fitToWindowAct->isChecked());
    zoomOutAct->setEnabled(!fitToWindowAct->isChecked());
    normalSizeAct->setEnabled(!fitToWindowAct->isChecked());
    nextPageAct->setEnabled(current != size);
    lastPageAct->setEnabled(current != 1);
}

void MainWindow::scaleImage(double factor) {
    scaleFactor *= factor;

    Magick::Blob blob;
    blob.update(comicbook.it->second.data(), comicbook.it->second.size());
    Magick::Image im;
    im.read(blob);
    im.filterType(filter);
    im.zoom(Magick::Geometry(im.columns() * scaleFactor, im.rows() * scaleFactor));
    im.write(&blob);
    image.loadFromData((const uchar *)blob.data(), blob.length());
    imageLabel->setPixmap(QPixmap::fromImage(image));
    imageLabel->adjustSize();

    // imageLabel->resize(scaleFactor * imageLabel->pixmap(Qt::ReturnByValue).size());

    // adjustScrollBar(scrollArea->horizontalScrollBar(), factor);
    // adjustScrollBar(scrollArea->verticalScrollBar(), factor);

    zoomInAct->setEnabled(scaleFactor < 3.0);
    zoomOutAct->setEnabled(scaleFactor > 0.333);
}

void MainWindow::adjustScrollBar(QScrollBar *scrollBar, double factor) {
    scrollBar->setValue(int(factor * scrollBar->value() + ((factor - 1) * scrollBar->pageStep() / 2)));
}
