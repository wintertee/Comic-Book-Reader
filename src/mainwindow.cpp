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
#include <QLayout>
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
#include <QtConcurrent>

#include "Magick++.h"
#include "bitarchiveinfo.hpp"
#include "bitexception.hpp"
#include "comicbook.h"
#include "extractdialog.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {

    connect(&cbfile, &CBFile::extractPage, &comicBook, &ComicBook::setPage, Qt::QueuedConnection);
    connect(&cbfile, &CBFile::totalPages, &comicBook, &ComicBook::setSize, Qt::QueuedConnection);

    // menu bar
    createActions();
    updateActions();

    imageLabel = new QLabel(this);
    imageLabel1 = new QLabel(imageLabel);
    imageLabel2 = new QLabel(imageLabel);

    scrollArea = new QScrollArea(this);
    scrollArea->setWidget(imageLabel);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    scrollArea->setVisible(false);
    setCentralWidget(scrollArea);

    resize(QGuiApplication::primaryScreen()->availableSize() * 3 / 5);
    showNormal();
}

MainWindow::~MainWindow() {
    delete imageLabel1;
    delete imageLabel2;
    delete imageLabel;
    delete scrollArea;
}

void MainWindow::open() {
    // open file dialog
    QString fileName = QFileDialog::getOpenFileName(this, "Open File", "", "Comic book files (*.cbz *.cbr)");
    if (fileName.isEmpty() || fileName.isNull())
        return;

    QFileInfo fileInfo(fileName);
    QString extention = fileInfo.suffix();

    comicBook.reset();

    comicBook.setName(fileInfo.baseName());

    // extract file to comicBook

    QtConcurrent::run([&] { cbfile.extract(fileName, extention); });

    // initialize

    scrollArea->setWidget(imageLabel);
    organizeFlag = false;

    // load first image
    scrollArea->setVisible(true);
    currentPage = 0;
    scalePageAround(currentPage, SCALE_TO_WINDOW);
    showImage();
}

void MainWindow::organize() {
    ExtractDialog extractDialog(&comicBook);
    auto extractDialogConnection = connect(&extractDialog, &ExtractDialog::extract, this, &MainWindow::extract);
    extractDialog.exec();
    disconnect(extractDialogConnection);
}

void MainWindow::extract(std::vector<int> chosenFlag) {

    QString tempDirName("temp");
    QDir tempDir(tempDirName);
    if (!tempDir.exists())
        tempDir.mkpath(".");

    QString archiveName =
        QFileDialog::getSaveFileName(this, "Export File", tempDir.currentPath() + "/" + comicBook.getName() + ".cbz", "Comic book files (*.cbz)");

    if (archiveName.isEmpty() || archiveName.isNull())
        return;

    QFile archiveFile(archiveName);
    if (archiveFile.exists())
        archiveFile.remove();

    for (unsigned int i = 0; i < chosenFlag.size(); ++i) {
        if (chosenFlag[i] == true) {
            comicBook.getPage(i)->save(tempDirName);
        }
    }

    cbfile.compress(tempDir.absolutePath(), archiveName);

    tempDir.removeRecursively();

    QMessageBox::information(nullptr, "CBR", "Successfully saved to " + archiveName);
}

void MainWindow::scalePageAround(unsigned int pageIdx, double factor) {

    if (factor == SCALE_TO_WINDOW) {
        comicBook.scalePageAround(pageIdx, MAINWINDOW_GET_SCROLLAREA_GEO);
        scaleFactor = comicBook.getPage(pageIdx)->getScaleFactor();
    } else {
        scaleFactor *= factor;
        comicBook.scalePageAround(pageIdx, scaleFactor);
    }

    adjustScrollBar(scrollArea->horizontalScrollBar(), factor);
    adjustScrollBar(scrollArea->verticalScrollBar(), factor);

    zoomInAct->setEnabled(scaleFactor < 5.0);
    zoomOutAct->setEnabled(scaleFactor > 0.2);
}

void MainWindow::showImage() {
    imageLabel1->setPixmap(comicBook.getPage(currentPage)->getPixmap());
    imageLabel1->adjustSize();
    if (doublePageFlag && (currentPage < (unsigned int)comicBook.getSize() - 1)) { // normal case for double page
        imageLabel2->setPixmap(comicBook.getPage(currentPage + 1)->getPixmap());
        imageLabel2->adjustSize();
        imageLabel->resize(imageLabel1->width() + imageLabel2->width() + 6, imageLabel1->height());
        imageLabel1->setGeometry(0, 0, imageLabel1->width(), imageLabel1->height());
        imageLabel2->setGeometry(imageLabel1->width() + 6, 0, imageLabel2->width(), imageLabel2->height());
        //    } else if (doublePageFlag && !img2.empty()) { // show the last page with odd number
        //        imageLabel2->setPixmap(img2.getPixmap());
        //        imageLabel2->adjustSize();
        //        imageLabel->resize(imageLabel1->width() + imageLabel2->width() + 6, imageLabel1->height());
        //        imageLabel1->setGeometry(0, 0, imageLabel1->width(), imageLabel1->height());
        //        imageLabel2->setGeometry(imageLabel1->width() + 6, 0, 0, 0);
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

void MainWindow::nextPage() { changePage(currentPage + (doublePageFlag ? 2 : 1)); }
void MainWindow::lastPage() { changePage(currentPage - (doublePageFlag ? 2 : 1)); }

void MainWindow::mousePressEvent(QMouseEvent *event) {
    QMainWindow::mousePressEvent(event);
    auto x = event->position().x();
    auto y = event->position().y();
    if (!organizeAct->isChecked() && scrollArea->geometry().contains(x, y)) {
        if (x < scrollArea->width() / 3)
            lastPage();
        else if (x > scrollArea->width() / 3 * 2)
            nextPage();
    }
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
    if (fitToWindowAct->isChecked()) {
        changePage(currentPage);
    }
}

void MainWindow::changePage(int toPage) {
    if (toPage < 0)
        return;
    if (toPage >= (int)comicBook.getSize())
        return;
    currentPage = toPage;
    scalePageAround(currentPage, fitToWindowAct->isChecked() ? SCALE_TO_WINDOW : 1.0);
    showImage();
}

void MainWindow::zoomIn() {
    scalePageAround(currentPage, 1.25);
    showImage();
}

void MainWindow::zoomOut() {
    scalePageAround(currentPage, 0.8);
    showImage();
}

void MainWindow::normalSize() {
    scaleFactor = 1.0;
    comicBook.scalePageAround(currentPage, scaleFactor);
    showImage();
}

void MainWindow::setFilter(Magick::FilterType newFilter) {
    filter = newFilter;
    comicBook.setFilter(filter);
}

void MainWindow::about() { QMessageBox::about(this, tr("About CBR"), tr("CBR is Comic Book Reader")); }

void MainWindow::createActions() {
    fileMenu = menuBar()->addMenu(tr("&File"));

    QAction *openAct = fileMenu->addAction(tr("O&pen..."), this, &MainWindow::open);
    openAct->setShortcut(QKeySequence::Open);

    organizeAct = fileMenu->addAction(tr("Export"), this, &MainWindow::organize);

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

    fitToWindowAct = viewMenu->addAction(tr("Fit to &Window"), [&]() { changePage(currentPage); });
    fitToWindowAct->setEnabled(true);
    fitToWindowAct->setCheckable(true);
    fitToWindowAct->setChecked(true);

    doublePageAct = viewMenu->addAction(tr("&Double Page"), this, &MainWindow::doublePage);
    doublePageAct->setEnabled(doublePageFlag);
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
    bool empty = (comicBook.getSize() == 0);
    doublePageAct->setEnabled(!empty);
    organizeAct->setEnabled(!empty);
    zoomInAct->setEnabled(!empty);
    zoomOutAct->setEnabled(!empty);
    normalSizeAct->setEnabled(!empty);
    nextPageAct->setEnabled(!empty && currentPage != comicBook.getSize() - (doublePageFlag ? 2 : 1));
    lastPageAct->setEnabled(!empty && currentPage != 0);
}

void MainWindow::updateTitle() {
    setWindowTitle(
        QString("CBR - %1 (%2/%3)    %4 %")
            .arg(comicBook.getName(), QString::number(currentPage + 1), QString::number(comicBook.getSize()), QString::number(scaleFactor * 100, 'f', 2)));
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
    if (comicBook.getSize() != 0) {
        if (doublePageFlag) {
            currentPage = (int)(currentPage / 2) * 2;
        } else {
            imageLabel2->setGeometry(imageLabel1->width() + 6, 0, 0, 0);
        }
        scalePageAround(currentPage, SCALE_TO_WINDOW);
        showImage();
    }
}
