#ifndef EXTRACTDIALOG_H
#define EXTRACTDIALOG_H

#include "comicbook.h"
#include "smartlabel.h"
#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QScrollArea>
#include <QVBoxLayout>

class ExtractDialog : public QDialog {
    Q_OBJECT

  public:
    explicit ExtractDialog(ComicBook *comicBook, QWidget *parent = nullptr);
    ~ExtractDialog();

  signals:
    void extract(std::vector<int> chosenFlag);

  private:
    void YesToAll();
    void NoToAll();
    QVBoxLayout *mainLayout;
    QScrollArea *scrollArea;

    QLabel *extractLabel;
    QDialogButtonBox *buttonBox;
    QVBoxLayout *qvLayout;

    std::vector<QHBoxLayout *> qhLayoutVtr;

    std::vector<SmartLabel *> qLabelVtr;
    ComicBook *comicBook;
    std::vector<int> chosenFlag;
    const int labelWidth = 150;
    const int labelHeight = 200;
    const int spacing = 18;
};

#endif // EXTRACTDIALOG_H
