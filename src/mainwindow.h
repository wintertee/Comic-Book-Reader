#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// disable minwindef.h
#define NOMINMAX

#define SCALE_TO_WINDOW 0

#include <QFileInfo>
#include <QMainWindow>

#include "Magick++.h"
#include "bitextractor.hpp"
#include "comicbook.h"

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
    void setFilter(Magick::FilterType newFilter);
    void about();

  private:
    void blobRead(size_t idx);
    void loadImageFromBlob(const Magick::Blob &blob);
    void loadScaledImage(double factor);

    void changePage(int relative_to);

    void createActions();
    void updateActions();
    void updateTitle();

    void adjustScrollBar(QScrollBar *scrollBar, double factor);

    // widgets
    QLabel *imageLabel;
    QScrollArea *scrollArea;
    QPixmap pixmap;

    // data
    ComicBook comicbook;
    Magick::Blob blob, scaled_blob;
    Magick::Image image;

    // cbz file info
    QFileInfo fileInfo;

    // parameters
    double scaleFactor = 1;
    size_t current;
    Magick::FilterType filter = Magick::LanczosFilter;

    // menubar
    QMenu *fileMenu;

    QAction *nextPageAct;
    QAction *lastPageAct;
    QAction *zoomInAct;
    QAction *zoomOutAct;
    QAction *normalSizeAct;
    QAction *fullScreenAct;

    QAction *gaussianFilterAct; // Zoom in for image
    QAction *boxFilterAct; // Zoom in for text
    QAction *sincFilterAct; // Zoom out for text
    QAction *lanczosFilterAct; // Zoom out for image
    QActionGroup* filterGroup;
};
#endif // MAINWINDOW_H
