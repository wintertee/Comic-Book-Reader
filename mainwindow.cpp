#include <QMessageBox>
#include <QFileDialog>

#include "mainwindow.h"
#include "qlabel.h"
#include "./ui_mainwindow.h"

#include "bit7z/include/bitextractor.hpp"
#include "bit7z/include/bitexception.hpp"
#include "bit7z/include/bitarchiveinfo.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
      statusbarLabel = new QLabel(this);
      ui->statusbar->addWidget(statusbarLabel);
      statusbarLabel->setText("no reason");

      QObject::connect(ui->actionOpen, &QAction::triggered,this, &MainWindow::import);

}

MainWindow::~MainWindow()
{
    delete statusbarLabel;
    delete ui;
}

bool MainWindow::import(){
    bit7z::BitExtractor * extractor;
    bit7z::BitArchiveInfo * archiveInfo;

    QString fileName = QFileDialog::getOpenFileName(this, "Open File", "","Comic files (*.cbz *.cbr)");
    QFileInfo fileInfo(fileName);
    QString extention = fileInfo.suffix();
    ComicBook newBook;

    try {
        bit7z::Bit7zLibrary lib{ L"7z.dll" };
        if (!QString::compare(extention, "cbz", Qt::CaseInsensitive)){
            extractor = new bit7z::BitExtractor{ lib, bit7z::BitFormat::Zip };
            archiveInfo = new bit7z::BitArchiveInfo{lib, fileName.toStdWString(), bit7z::BitFormat::Zip};
}
        else if (!QString::compare(extention, "cbr", Qt::CaseInsensitive)){
            extractor = new bit7z::BitExtractor{ lib, bit7z::BitFormat::Rar };
             archiveInfo = new bit7z::BitArchiveInfo{lib, fileName.toStdWString(), bit7z::BitFormat::Zip};}
        else throw(bit7z::BitException::runtime_error ("unknown file type " + extention.toStdString()));



        extractor->extract( fileName.toStdWString(), newBook.map );

    } catch ( const bit7z::BitException& ex ) {
          QMessageBox msgBox;
          msgBox.setText("An error occured.");
          msgBox.setInformativeText(ex.what());
          msgBox.setStandardButtons(QMessageBox::Cancel);
          msgBox.exec();
    }
    comicBookList.push_back(newBook);
    delete extractor;
    delete archiveInfo;
    return true;
}

