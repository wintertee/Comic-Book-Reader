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
        QMessageBox::critical(nullptr, "Error", e.what());
    }
    return 0;
}
