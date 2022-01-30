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
#include "qlabel.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {

    connect(&cbfile, &CBFile::extractPage, &comicBook, &ComicBook::setPage, Qt::QueuedConnection);
    connect(&cbfile, &CBFile::totalPages, &comicBook, &ComicBook::setSize, Qt::QueuedConnection);

    // menu bar
    createActions();
    updateActions();

    imageLabel = new QLabel(this);
    imageLabel1 = new QLabel(imageLabel);
    imageLabel2 = new QLabel(imageLabel);

    extractLabel = new QLabel(this);
    extractLabel->setVisible(false);

    qvLayout = new QVBoxLayout();

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
    clearLayout();
    delete qvLayout;
    delete extractLabel;
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

    // comicBook.setTargetWindowSize(scrollArea->width() / (doublePageFlag ? 2 : 1), scrollArea->height());

    comicBook.setName(fileInfo.baseName());

    // extract file to comicbook

    QtConcurrent::run([&] { cbfile.extract(fileName, extention); });

    // initialize
    clearLayout();

    scrollArea->takeWidget();
    scrollArea->setWidget(imageLabel);
    organizeFlag = false;

    // load first image
    scrollArea->setVisible(true);
    currentPage = 0;
    scalePageAround(currentPage, SCALE_TO_WINDOW);
    showImage();
}

void MainWindow::organize() {
    //    if (organizeAct->isChecked()) {
    //        scrollArea->takeWidget();
    //        scrollArea->setWidget(extractLabel);
    //        extractLabel->setVisible(true);
    //        setWindowTitle(QString("CBR - %1 Extracting...").arg(comicBook.getName()));
    //        subComicBook.clear();
    //        subComicBook.resize(comicBook.getSize(), false);

    //        if (!organizeFlag) {
    //            constexpr int labelWidth = 150;
    //            constexpr int labelHeight = 200;
    //            constexpr int spacing = 18;
    //            int colNum = (scrollArea->width()) / (labelWidth + spacing);
    //            colNum = colNum > 0 ? colNum : 1;
    //            int lineNum = (int)comicBook.getSize() / colNum;
    //            SmartImage im;
    //            im.setFilter(filter);
    //            int page = 0;
    //            for (int i = 0; i < lineNum; ++i) {
    //                qhLayoutVtr.push_back(new QHBoxLayout);
    //                for (int j = 0; j < colNum; ++j, ++page) {
    //                    im.loadpage(comicBook.getPage(page));
    //                    im.fitToWindow(labelWidth, labelHeight * SMARTLABEL_PROP);
    //                    qLabelVtr.push_back(new SmartLabel(extractLabel, im.getPixmap(), page, &subComicBook[page]));
    //                    qLabelVtr[page]->resize(labelWidth, labelHeight);
    //                    qhLayoutVtr[i]->addWidget(qLabelVtr[page]);
    //                }
    //                qhLayoutVtr[i]->setSpacing(spacing);
    //                qvLayout->addLayout(qhLayoutVtr[i]);
    //            }
    //            if (comicBook.getSize() % colNum != 0) {
    //                qhLayoutVtr.push_back(new QHBoxLayout);
    //                for (int j = 0; j < colNum; ++j, ++page) {
    //                    if (page < comicBook.getSize()) {
    //                        im.loadpage(comicBook.getPage(page));
    //                        im.fitToWindow(labelWidth, labelHeight * SMARTLABEL_PROP);
    //                        qLabelVtr.push_back(new SmartLabel(extractLabel, im.getPixmap(), page, &subComicBook[page]));
    //                        qLabelVtr[page]->resize(labelWidth, labelHeight);
    //                        qhLayoutVtr[qhLayoutVtr.size() - 1]->addWidget(qLabelVtr[page]);
    //                    } else {
    //                        qLabelVtr.push_back(new SmartLabel(extractLabel));
    //                        qLabelVtr[page]->resize(labelWidth, labelHeight);
    //                        qhLayoutVtr[qhLayoutVtr.size() - 1]->addWidget(qLabelVtr[page]);
    //                    }
    //                }
    //                qhLayoutVtr[qhLayoutVtr.size() - 1]->setSpacing(spacing);
    //                qvLayout->addLayout(qhLayoutVtr[qhLayoutVtr.size() - 1]);
    //            }
    //            extractLabel->resize((labelWidth + spacing) * colNum, (lineNum + (comicBook.getSize() % colNum != 0)) * (labelHeight + 12));
    //            extractLabel->setLayout(qvLayout);
    //            organizeFlag = true;
    //        }
    //    } else {
    //        scrollArea->takeWidget();
    //        scrollArea->setWidget(imageLabel);

    //        updateTitle();
    //    }
    //    updateActions();
}

void MainWindow::clearLayout() {
    for (auto &it : qhLayoutVtr) {
        delete it;
    }
    qhLayoutVtr.clear();

    for (auto &it : qLabelVtr) {
        delete it;
    }
    qLabelVtr.clear();
}

void MainWindow::selectAll() {

    for (int i = 0; i < comicBook.getSize(); ++i) {
        qLabelVtr[i]->setChosen(selectAllAct->isChecked());
    }
}

void MainWindow::extract() {}

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
    auto x = event->position().x();
    auto y = event->position().y();
    if (!organizeAct->isChecked() && scrollArea->geometry().contains(x, y)) {
        if (x < scrollArea->width() / 3)
            lastPage();
        else if (x > scrollArea->width() / 3 * 2)
            nextPage();
    }
}

void MainWindow::changePage(int toPage) {
    if (toPage < 0)
        return;
    if (toPage >= (int)comicBook.getSize())
        return;
    currentPage = toPage;
    scalePageAround(currentPage, 1.0);
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

    extractMenu = fileMenu->addMenu(tr("Extract Menu"));
    extractMenu->setEnabled(false);

    organizeAct = extractMenu->addAction(tr("O&rganize"), this, &MainWindow::organize);
    organizeAct->setEnabled(true);
    organizeAct->setCheckable(true);
    organizeAct->setShortcut(QKeySequence::New);

    selectAllAct = extractMenu->addAction(tr("&Select All"), this, &MainWindow::selectAll);
    selectAllAct->setEnabled(false);
    selectAllAct->setCheckable(true);
    selectAllAct->setShortcut(QKeySequence::SelectAll);

    extractAct = extractMenu->addAction(tr("&Extract"), this, &MainWindow::extract);
    extractAct->setEnabled(false);
    extractAct->setShortcut(QKeySequence::New);

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
    bool empty = comicBook.empty();
    if (organizeAct->isChecked()) {
        zoomInAct->setEnabled(false);
        zoomOutAct->setEnabled(false);
        normalSizeAct->setEnabled(false);
        nextPageAct->setEnabled(false);
        lastPageAct->setEnabled(false);
        doublePageAct->setEnabled(false);
        selectAllAct->setEnabled(true);
        extractAct->setEnabled(true);
    } else {
        doublePageAct->setEnabled(true);
        selectAllAct->setChecked(false);
        selectAllAct->setEnabled(false);
        extractAct->setEnabled(false);
        extractMenu->setEnabled(!empty);
        zoomInAct->setEnabled(!empty);
        zoomOutAct->setEnabled(!empty);
        normalSizeAct->setEnabled(!empty);
        nextPageAct->setEnabled(!empty && currentPage != comicBook.getSize() - 1);
        lastPageAct->setEnabled(!empty && currentPage != 0);
    }
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
    if (!comicBook.empty()) {
        if (doublePageFlag) {
            currentPage = (int)(currentPage / 2) * 2;
        } else {
            imageLabel2->setGeometry(imageLabel1->width() + 6, 0, 0, 0);
        }
        scalePageAround(currentPage, SCALE_TO_WINDOW);
        showImage();
    }
}
