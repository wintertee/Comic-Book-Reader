---
marp: true
---

# Projet IN204: Comic Book Reader


###### Yuang ZHAO, Tiancheng YANG

###### 14-02-2022

---
<!-- paginate: true -->

# Introduction

Notre programme met en œuvre les fonctions suivantes:

- Lecture de fichiers `cbz` et `cbr`
- Décompression asynchrone et redimensionnement asynchrone des images
- Extraction et l'exportation de fichiers

---

# Architecture du programme

## Organization du code

Les dossiers dans le projet:

- `3rdparty`: Bibliothèques de tiers.
  - `7zip`
  - `bit7z`
  - `ImageMagick`
- `docs`: Documentation.
- `includes`: Headers.
- `resources`: Fichiers de ressources.
- `src`: Codes
  
---

# Architecture du programme

## Design du programme

Il y a 6 classes dans le programme:

- `MainWindow`: Interface principale du programme
- `ExtractDialog`: Interface pour extraire certaines pages et les exporter
- `CBFile`: Manipulation (décompression et compression) de fichiers `cbz`/`cbr`
- `ComicBook`: Contenu de la bande dessinée ( titres et toutes les images)
- `SmartImage`: Stockage d'une image, offrant la possibilité de zoomer avec différents filtres
- `SmartLable`: Aide pour afficher chaque image dans `ExtractDialog`

---

# Encapsulation des images

```c++
class SmartImage{
    Magick::Blob blob, scaled_blob;
    Magick::Image image;
    QPixmap pixmap;
    QString name;
    Magick::FilterType filter;

    /// load page to blob
    void loadpage(const std::vector<unsigned char> *page, QString name);
    /// set scaleFactor and generate pixmap
    void setScaleFactor(double scaleFactor);
    /// calculate scaleFactor to fit to window size
    void fitToWindow(int win_w, int win_h);
    void setFilter(Magick::FilterType filter);
    const QPixmap &getPixmap() const;
}
```

---

# Décompression asynchrone

Grâce à `QtConcurrent` de QT, une API multithread avancée, nous pouvons facilement implémenter la décompression asynchrone.

```c++
// MainWindow::open()
QtConcurrent::run([&] { cbfile.extract(fileName, extention); });
```

---

# Décompression asynchrone

Grâce à signal-slot de QT:

```c++
// CBFile::extract()
for (auto &item : archiveInfo->items()) {
      auto *out_buffer = new std::vector<unsigned char>;
      extractor->extract(wFileName, *out_buffer, item.index());
      emit extractPage(out_buffer, pageIdx, item.name()));
}
```

```c++
// MainWindow::MainWindow()
connect(&cbfile, &CBFile::extractPage, &comicBook, &ComicBook::setPage, Qt::QueuedConnection);
```

---

# Mise à l'échelle asynchrone de l'image

Chaque mise à l'échelle d'image est un `QtConcurrent`, Le fait qu'une tâche soit terminée ou non est enregistré dans un `QFuture`.

```c++
class ComicBook{
    /// pointers to each page
    std::vector<SmartImage *> pages;

    /// status of page-loading/scaling for each page
    std::vector<QFuture<void>> qFutures;
}
```

---

# Mise à l'échelle asynchrone de l'image

Chaque mise à l'échelle d'image est un `QtConcurrent`, Le fait qu'une tâche soit terminée ou non est enregistré dans un `QFuture`.

```c++
void ComicBook::setPage(std::vector<unsigned char> *page, int pageIdx, QString pageName) {
    qFutures[pageIdx] = QtConcurrent::run([this, page, pageIdx, pageName] {
        pages[pageIdx] = new SmartImage();
        pages[pageIdx]->loadpage(page, pageName);
        delete page;
    });
}

void ComicBook::waitUntilPageAvailable(unsigned int pageIdx) const {
    while (pageIdx >= size || qFutures[pageIdx].isCanceled() || !qFutures[pageIdx].isFinished()) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
}

void ComicBook::scalePageRange(unsigned int start, unsigned int end, double scaleFactor) {
    for (unsigned int i = start; i < end; ++i) {
        waitUntilPageAvailable(i);
        qFutures[pageIdx].then([&] { scalePage(i, scaleFactor); });
    }
}
```

---

# Encapsulation du Label

```C++
#define SMARTLABEL_PROP 5.0 / 6.0 // the proportion of image part
class SmartLabel : public QLabel {
    Q_OBJECT
  public:
    SmartLabel(QWidget *parent);
    SmartLabel(QWidget *parent, QPixmap image, int num, int *chosenFlag);
    ~SmartLabel();
    void setChosen(bool flag);
    void resize(int w, int h);
  protected:
    void mousePressEvent(QMouseEvent *event);
  private:
    QLabel *imageLabel;
    QLabel *textLabel;
    int *chosenFlag;
    int num = -1;
    bool empty = true;
};
```

---

# Double Page

```C++
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
```

---

# Double Page

``` C++
void MainWindow::showImage() {
    imageLabel1->setPixmap(comicBook.getPage(currentPage)->getPixmap());
    imageLabel1->adjustSize();
    if (doublePageFlag && (currentPage < (unsigned int)comicBook.getSize() - 1)) { // normal case for double page
        imageLabel2->setPixmap(comicBook.getPage(currentPage + 1)->getPixmap());
        imageLabel2->adjustSize();
        imageLabel->resize(imageLabel1->width() + imageLabel2->width() + 6, imageLabel1->height());
        imageLabel1->setGeometry(0, 0, imageLabel1->width(), imageLabel1->height());
        imageLabel2->setGeometry(imageLabel1->width() + 6, 0, imageLabel2->width(), imageLabel2->height());
    } else if (doublePageFlag) {
        imageLabel->resize(imageLabel1->width() + imageLabel1->width() + 6, imageLabel1->height());
        imageLabel1->setGeometry(0, 0, imageLabel1->width(), imageLabel1->height());
        imageLabel2->setGeometry(imageLabel1->width() + 6, 0, 0, 0);
    } else {
        imageLabel->resize(imageLabel1->width(), imageLabel1->height());
    }
    updateActions();
    updateTitle();
}
```

---

# Filtre

- GaussianFilter : Zoom in for image
- BoxFilter : Zoom in for text
- SincFilter : Zoom out for text
- LanczosFilter : Zoom out for image

#### référence : https://legacy.imagemagick.org/Usage/filter/

---

# Merci