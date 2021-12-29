#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFileInfo>
#include <QImage>
#include <QMainWindow>

#include "bitextractor.hpp"
#include "comicbook.h"

QT_BEGIN_NAMESPACE
class QAction;
class QLabel;
class QMenu;
class QScrollArea;
class QScrollBar;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
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
  void fitToWindow();
  void about();

private:
  void createActions();
  void createMenus();
  void updateActions();
  bool saveFile(const QString &fileName);
  void updateImage();
  void scaleImage(double factor);
  void adjustScrollBar(QScrollBar *scrollBar, double factor);

  ComicBook comicbook;
  QImage image;
  QLabel *imageLabel;
  QFileInfo fileInfo;
  QScrollArea *scrollArea;
  double scaleFactor = 1;
  int size, current;

  QAction *nextPageAct;
  QAction *lastPageAct;
  QAction *zoomInAct;
  QAction *zoomOutAct;
  QAction *normalSizeAct;
  QAction *fitToWindowAct;
};
#endif // MAINWINDOW_H
