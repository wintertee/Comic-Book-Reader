#include "mainwindow.h"

#include <QActionGroup>
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
    img.setFilter(filter);

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

MainWindow::~MainWindow() {
    delete imageLabel;
    delete scrollArea;
}

void MainWindow::open() {
    // open file dialog
    QString fileName = QFileDialog::getOpenFileName(this, "Open File", "", "Comic book files (*.cbz *.cbr)");
    if (fileName.isEmpty() || fileName.isNull())
        return;

    // extract file to comicbook
    cbfile.extract(fileName, comicbook);

    // load first image
    scrollArea->setVisible(true);
    currentPage = 0;
    loadImage();
    scaleImage(SCALE_TO_WINDOW);
    showImage();
}

void MainWindow::loadImage() { img.loadpage(comicbook.getPage(currentPage)); }

void MainWindow::scaleImage(double factor) {

    if (factor == SCALE_TO_WINDOW) {
        scaleFactor = img.fitToWindow(scrollArea->width(), scrollArea->height());
    } else {
        scaleFactor *= factor;
        img.setScaleFactor(scaleFactor);
    }

    adjustScrollBar(scrollArea->horizontalScrollBar(), factor);
    adjustScrollBar(scrollArea->verticalScrollBar(), factor);

    zoomInAct->setEnabled(scaleFactor < 5.0);
    zoomOutAct->setEnabled(scaleFactor > 0.2);
}

void MainWindow::showImage() {
    imageLabel->setPixmap(img.getPixmap());
    imageLabel->adjustSize();

    updateActions();
    updateTitle();
}

void MainWindow::nextPage() { changePage(currentPage + 1); }
void MainWindow::lastPage() { changePage(currentPage - 1); }

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

void MainWindow::changePage(int toPage) {
    if (toPage < 0)
        return;
    if (toPage >= (int)comicbook.size())
        return;
    currentPage = toPage;
    loadImage();
    scaleImage(SCALE_TO_WINDOW);
    showImage();
}

void MainWindow::zoomIn() {
    scaleImage(1.25);
    showImage();
}

void MainWindow::zoomOut() {
    scaleImage(0.8);
    showImage();
}

void MainWindow::normalSize() {
    scaleFactor = 1.0;
    img.setScaleFactor(scaleFactor);
    showImage();
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

    gaussianFilterAct = filterMenu->addAction(tr("Zoom in for image(GaussianFilter)"), this, [=]() { MainWindow::setFilter(Magick::GaussianFilter); });
    boxFilterAct = filterMenu->addAction(tr("Zoom in for text(BoxFilter)"), this, [=]() { MainWindow::setFilter(Magick::BoxFilter); });
    sincFilterAct = filterMenu->addAction(tr("Zoom out for text(SincFilter)"), this, [=]() { MainWindow::setFilter(Magick::SincFilter); });
    lanczosFilterAct = filterMenu->addAction(tr("Zoom out for image(LanczosFilter)"), this, [=]() { MainWindow::setFilter(Magick::LanczosFilter); });

    filterGroup = new QActionGroup(this);
    filterGroup->addAction(gaussianFilterAct);
    filterGroup->addAction(boxFilterAct);
    filterGroup->addAction(sincFilterAct);
    filterGroup->addAction(lanczosFilterAct);
    gaussianFilterAct->setCheckable(true);
    boxFilterAct->setCheckable(true);
    sincFilterAct->setCheckable(true);
    lanczosFilterAct->setCheckable(true);
    lanczosFilterAct->setChecked(true);

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));

    helpMenu->addAction(tr("&About"), this, &MainWindow::about);
    helpMenu->addAction(tr("About &Qt"), this, &QApplication::aboutQt);
}

void MainWindow::updateActions() {
    bool empty = comicbook.empty();
    zoomInAct->setEnabled(!empty);
    zoomOutAct->setEnabled(!empty);
    normalSizeAct->setEnabled(!empty);
    nextPageAct->setEnabled(!empty && currentPage != comicbook.size() - 1);
    lastPageAct->setEnabled(!empty && currentPage != 0);
}

void MainWindow::updateTitle() {
    setWindowTitle(
        QString("CBR - %1 (%2/%3)    %4 %")
            .arg(comicbook.getName(), QString::number(currentPage + 1), QString::number(comicbook.size()), QString::number(scaleFactor * 100, 'f', 2)));
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
