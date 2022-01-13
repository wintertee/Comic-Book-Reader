#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// disable minwindef.h
#define NOMINMAX

#define SCALE_TO_WINDOW 0

#include <QFileInfo>
#include <QMainWindow>

#include "Magick++.h"
#include "bitextractor.hpp"
#include "cbfile.h"
#include "comicbook.h"
#include "smartimage.h"

QT_BEGIN_NAMESPACE
class QAction;
class QLabel;
class QMenu;
class QScrollArea;
class QScrollBar;
class QActionGroup;
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

  public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

  protected:
    void mousePressEvent(QMouseEvent *event);

  private slots:
    /// open a cbr or cbz file and extract to comicbook
    void open();

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
    void loadImage();
    void scaleImage(double factor);
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
    ComicBook comicbook;
    SmartImage img;
    SmartImage img2;

    // parameters
    double scaleFactor = 1;
    double scaleFactor2 = 1;
    unsigned int currentPage;
    Magick::FilterType filter = Magick::LanczosFilter;
    bool doublePageFlag = false;

    // menubar
    QMenu *fileMenu;

    QAction *nextPageAct;
    QAction *lastPageAct;
    QAction *zoomInAct;
    QAction *zoomOutAct;
    QAction *normalSizeAct;
    QAction *fullScreenAct;
    QAction *doublePageAct;

    QAction *gaussianFilterAct; // Zoom in for image
    QAction *boxFilterAct;      // Zoom in for text
    QAction *sincFilterAct;     // Zoom out for text
    QAction *lanczosFilterAct;  // Zoom out for image
    QActionGroup *filterGroup;
};
#endif // MAINWINDOW_H
