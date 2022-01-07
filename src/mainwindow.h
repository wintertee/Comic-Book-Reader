#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define NOMINMAX
// disable minwindef.h

#include <QFileInfo>
#include <QImage>
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
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

  public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

  private slots:
    void open();
    void zoomIn();
    void nextPage();
    void lastPage();
    void zoomOut();
    void normalSize();
    void fullScreen();
    void setFilter(Magick::FilterType newFilter);
    void about();

  private:
    void changePage(int relative_to);
    void createActions();
    void createMenus();
    void updateActions();
    bool saveFile(const QString &fileName);
    void loadImageFromBlob(const Magick::Blob &blob);
    void loadScaledImage(double factor);
    void adjustScrollBar(QScrollBar *scrollBar, double factor);
    void blobRead(size_t idx);

    ComicBook comicbook;
    Magick::Blob blob, scaled_blob;
    Magick::Image image;
    QLabel *imageLabel;
    QPixmap pixmap;
    QFileInfo fileInfo;
    QScrollArea *scrollArea;
    double scaleFactor = 1;
    size_t current;
    Magick::FilterType filter = Magick::LanczosFilter;

    QMenu *fileMenu;

    QAction *nextPageAct;
    QAction *lastPageAct;
    QAction *zoomInAct;
    QAction *zoomOutAct;
    QAction *normalSizeAct;
    QAction *fullScreenAct;

    QAction *pointFilterAct;
    QAction *boxFilterAct;
    QAction *triangleFilterAct;
    QAction *sincFilterAct;
    QAction *lanczosFilterAct;
};
#endif // MAINWINDOW_H
