#include "mainwindow.h"

#include <QApplication>
#include <QMessageBox>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    try {
        a.exec();
    } catch (const std::exception &e) {
        QMessageBox msgBox;
        msgBox.setText("An error occured.");
        msgBox.setInformativeText(e.what());
        msgBox.setStandardButtons(QMessageBox::Cancel);
        msgBox.exec();
    }
    return 0;
}
