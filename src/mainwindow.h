#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// disable minwindef.h
#define NOMINMAX

#include <QFileInfo>
#include <QLayout>
#include <QMainWindow>
#include <vector>

#include "Magick++.h"
#include "bitextractor.hpp"
#include "cbfile.h"
#include "comicbook.h"
#include "smartimage.h"
#include "smartlabel.h"

QT_BEGIN_NAMESPACE
class QAction;
class QLabel;
class QMenu;
class QScrollArea;
class QScrollBar;
class QActionGroup;
QT_END_NAMESPACE

// windows_width, windows_height
#define MAINWINDOW_GET_SCROLLAREA_GEO scrollArea->width() / (doublePageFlag ? 2 : 1), scrollArea->height()

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
    void organize();
    void selectAll();
    void extract();

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

    void clearLayout();

    CBFile cbfile;

    // widgets
    QLabel *imageLabel;
    QLabel *imageLabel1;
    QLabel *imageLabel2;
    QLabel *extractLabel;
    QScrollArea *scrollArea;

    // data
    ComicBook comicBook;
    std::vector<int> subComicBook;

    std::vector<QHBoxLayout *> qhLayoutVtr;
    QVBoxLayout *qvLayout;
    std::vector<SmartLabel *> qLabelVtr;

    // parameters

    constexpr const static double SCALE_TO_WINDOW = 0.0;
    double scaleFactor = 1.0;
    unsigned int currentPage;
    Magick::FilterType filter = Magick::LanczosFilter;
    bool doublePageFlag = false;
    bool organizeFlag = false;

    // menubar
    QMenu *fileMenu;
    QMenu *extractMenu;

    QAction *organizeAct;
    QAction *selectAllAct;
    QAction *extractAct;
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
