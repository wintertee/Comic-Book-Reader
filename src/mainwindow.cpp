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

    img.setFilter(filter);
    img2.setFilter(filter);

    // menu bar
    createActions();
    updateActions();

    imageLabel = new QLabel(this);
    imageLabel1 = new QLabel(imageLabel);
    imageLabel2 = new QLabel(imageLabel);

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
    delete imageLabel1;
    delete imageLabel2;
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

void MainWindow::loadImage() {
    img.loadpage(comicbook.getPage(currentPage));
    if (doublePageFlag && (currentPage < (unsigned int)comicbook.size() - 1)) {
        img2.loadpage(comicbook.getPage(currentPage + 1));
    }
}

void MainWindow::scaleImage(double factor) {

    if (factor == SCALE_TO_WINDOW) {
        if (doublePageFlag && !img2.empty()) {
            scaleFactor = img.fitToWindow(scrollArea->width() / 2, scrollArea->height());
            scaleFactor2 = img2.fitToWindow(scrollArea->width() / 2, scrollArea->height());
        } else {
            scaleFactor = img.fitToWindow(scrollArea->width(), scrollArea->height());
        }

    } else {
        scaleFactor *= factor;
        img.setScaleFactor(scaleFactor);
        if (doublePageFlag && !img2.empty()) {
            scaleFactor2 *= factor;
            img2.setScaleFactor(scaleFactor2);
        }
    }

    adjustScrollBar(scrollArea->horizontalScrollBar(), factor);
    adjustScrollBar(scrollArea->verticalScrollBar(), factor);

    zoomInAct->setEnabled(scaleFactor < 5.0 && scaleFactor2 < 5.0);
    zoomOutAct->setEnabled(scaleFactor > 0.2 && scaleFactor2 > 0.2);
}

void MainWindow::showImage() {
    imageLabel1->setPixmap(img.getPixmap());
    imageLabel1->adjustSize();
    if (doublePageFlag && (currentPage < (unsigned int)comicbook.size() - 1)) { // normal case for double page
        imageLabel2->setPixmap(img2.getPixmap());
        imageLabel2->adjustSize();
        imageLabel->resize(imageLabel1->width() + imageLabel2->width() + 6, imageLabel1->height());
        imageLabel1->setGeometry(0, 0, imageLabel1->width(), imageLabel1->height());
        imageLabel2->setGeometry(imageLabel1->width() + 6, 0, imageLabel2->width(), imageLabel2->height());
    } else if (doublePageFlag && !img2.empty()) { // show the last page with odd number
        imageLabel2->setPixmap(img2.getPixmap());
        imageLabel2->adjustSize();
        imageLabel->resize(imageLabel1->width() + imageLabel2->width() + 6, imageLabel1->height());
        imageLabel1->setGeometry(0, 0, imageLabel1->width(), imageLabel1->height());
        imageLabel2->setGeometry(imageLabel1->width() + 6, 0, 0, 0);
    } else if (doublePageFlag) { // the file has only one page
        imageLabel->resize(imageLabel1->width() + imageLabel1->width() + 6, imageLabel1->height());
        imageLabel1->setGeometry(0, 0, imageLabel1->width(), imageLabel1->height());
        imageLabel2->setGeometry(imageLabel1->width() + 6, 0, 0, 0);
    } else {
        imageLabel->resize(imageLabel1->width(), imageLabel1->height());
    }

    updateActions();
    updateTitle();
}

void MainWindow::nextPage() {
    if (doublePageFlag)
        changePage(currentPage + 2);
    else
        changePage(currentPage + 1);
}
void MainWindow::lastPage() {
    if (doublePageFlag)
        changePage(currentPage - 2);
    else
        changePage(currentPage - 1);
}

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
    if (doublePageFlag) {
        img2.setScaleFactor(scaleFactor2);
    }
    showImage();
}

void MainWindow::setFilter(Magick::FilterType newFilter) {
    filter = newFilter;
    img.setFilter(filter);
    img2.setFilter(filter);
}

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

    doublePageAct = viewMenu->addAction(tr("&Double Page"), this, &MainWindow::doublePage);
    doublePageAct->setEnabled(true);
    doublePageAct->setCheckable(true);
    doublePageAct->setShortcut(tr("Ctrl+W"));

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

void MainWindow::doublePage() {
    doublePageFlag = !doublePageFlag;
    if (!comicbook.empty()) {
        if (doublePageFlag) {
            currentPage = (int)(currentPage / 2) * 2;
        } else {
            imageLabel2->setGeometry(imageLabel1->width() + 6, 0, 0, 0);
        }
        loadImage();
        scaleImage(SCALE_TO_WINDOW);
        showImage();
    }
}
