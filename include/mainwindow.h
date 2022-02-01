#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// disable minwindef.h
#define NOMINMAX

#include <QAction>
#include <QActionGroup>
#include <QFileInfo>
#include <QLayout>
#include <QMainWindow>
#include <QMenu>
#include <QScrollArea>
#include <QScrollBar>
#include <vector>

#include "Magick++.h"
#include "bitextractor.hpp"
#include "cbfile.h"
#include "comicbook.h"
#include "smartimage.h"
#include "smartlabel.h"

// windows_width, windows_height
#define MAINWINDOW_GET_SCROLLAREA_GEO scrollArea->width() / (doublePageFlag ? 2 : 1), scrollArea->height()

class MainWindow : public QMainWindow {
    Q_OBJECT

  public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

  protected:
    void mousePressEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent *event);

  private slots:
    /// open a cbr or cbz file and extract to comicbook
    void open();
    void organize();
    void extract(std::vector<int> chosenFlag);

    void nextPage();
    void lastPage();

    void zoomIn();
    void zoomOut();
    /// load original image (scale 100%)
    void normalSize();

    void fullScreen();
    void doublePage();
    void setFilter(Magick::FilterType newFilter);
    void about();

  private:
    void scalePageAround(unsigned int pageIdx, double factor);
    void showImage();

    void changePage(int relative_to);

    void createActions();
    void updateActions();
    void updateTitle();

    void adjustScrollBar(QScrollBar *scrollBar, double factor);

    CBFile cbfile;

    // widgets
    QLabel *imageLabel;
    QLabel *imageLabel1;
    QLabel *imageLabel2;
    QScrollArea *scrollArea;

    // data
    ComicBook comicBook;

    // parameters

    constexpr const static double SCALE_TO_WINDOW = 0.0;
    double scaleFactor = 1.0;
    unsigned int currentPage;
    Magick::FilterType filter = Magick::LanczosFilter;
    bool doublePageFlag = false;
    bool organizeFlag = false;

    // menubar
    QMenu *fileMenu;

    QAction *organizeAct;
    QAction *nextPageAct;
    QAction *lastPageAct;
    QAction *zoomInAct;
    QAction *zoomOutAct;
    QAction *normalSizeAct;
    QAction *fullScreenAct;
    QAction *fitToWindowAct;
    QAction *doublePageAct;

    QAction *gaussianFilterAct; // Zoom in for image
    QAction *boxFilterAct;      // Zoom in for text
    QAction *sincFilterAct;     // Zoom out for text
    QAction *lanczosFilterAct;  // Zoom out for image
    QActionGroup *filterGroup;
};
#endif // MAINWINDOW_H
