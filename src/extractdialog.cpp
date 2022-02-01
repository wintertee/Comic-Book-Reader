#include "extractdialog.h"
#include <QGuiApplication>
#include <QMessageBox>
#include <QPushButton>
#include <QScreen>

ExtractDialog::ExtractDialog(ComicBook *comicBook, QWidget *parent) : QDialog(parent), comicBook(comicBook) {
    mainLayout = new QVBoxLayout;
    scrollArea = new QScrollArea(this);
    extractLabel = new QLabel(this);
    buttonBox = new QDialogButtonBox(QDialogButtonBox::YesToAll | QDialogButtonBox::NoToAll | QDialogButtonBox::Save | QDialogButtonBox::Cancel);
    qvLayout = new QVBoxLayout();
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    scrollArea->setWidget(extractLabel);
    scrollArea->setVisible(true);
    mainLayout->addWidget(scrollArea);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);

    connect(buttonBox->button(QDialogButtonBox::YesToAll), &QPushButton::clicked, this, &ExtractDialog::YesToAll);
    connect(buttonBox->button(QDialogButtonBox::NoToAll), &QPushButton::clicked, this, &ExtractDialog::NoToAll);
    connect(buttonBox->button(QDialogButtonBox::Save), &QPushButton::clicked, this, [&]() {
        bool chosenSomePages = false;
        for (auto &i : chosenFlag) {
            if (i == true) {
                chosenSomePages = true;
                break;
            }
        }
        if (chosenSomePages) {
            emit extract(chosenFlag);
            done(0);
        } else {
            QMessageBox::warning(this, "Warning", "At least one page should be selected.");
        }
    });
    connect(buttonBox->button(QDialogButtonBox::Cancel), &QPushButton::clicked, this, &ExtractDialog::done);

    resize(QGuiApplication::primaryScreen()->availableSize() * 3 / 5);

    chosenFlag.assign(comicBook->getSize(), false);

    int colNum = (width()) / (labelWidth + spacing);
    colNum = colNum > 0 ? colNum : 1;
    int lineNum = (int)comicBook->getSize() / colNum;
    comicBook->scalePageAround(0, labelWidth, labelHeight * SMARTLABEL_PROP, -1);
    unsigned int page = 0;
    for (int i = 0; i < lineNum; ++i) {
        qhLayoutVtr.push_back(new QHBoxLayout);
        for (int j = 0; j < colNum; ++j, ++page) {
            qLabelVtr.push_back(new SmartLabel(extractLabel, comicBook->getPage(page)->getPixmap(), page, &chosenFlag[page]));
            qLabelVtr[page]->resize(labelWidth, labelHeight);
            qhLayoutVtr[i]->addWidget(qLabelVtr[page]);
        }
        qhLayoutVtr[i]->setSpacing(spacing);
        qvLayout->addLayout(qhLayoutVtr[i]);
    }
    if (comicBook->getSize() % colNum != 0) {
        qhLayoutVtr.push_back(new QHBoxLayout);
        for (int j = 0; j < colNum; ++j, ++page) {
            if (page < comicBook->getSize()) {
                qLabelVtr.push_back(new SmartLabel(extractLabel, comicBook->getPage(page)->getPixmap(), page, &chosenFlag[page]));
                qLabelVtr[page]->resize(labelWidth, labelHeight);
                qhLayoutVtr[qhLayoutVtr.size() - 1]->addWidget(qLabelVtr[page]);
            } else {
                qLabelVtr.push_back(new SmartLabel(extractLabel));
                qLabelVtr[page]->resize(labelWidth, labelHeight);
                qhLayoutVtr[qhLayoutVtr.size() - 1]->addWidget(qLabelVtr[page]);
            }
        }
        qhLayoutVtr[qhLayoutVtr.size() - 1]->setSpacing(spacing);
        qvLayout->addLayout(qhLayoutVtr[qhLayoutVtr.size() - 1]);
    }
    extractLabel->resize((labelWidth + spacing) * colNum, (lineNum + (comicBook->getSize() % colNum != 0)) * (labelHeight + 12));
    extractLabel->setLayout(qvLayout);
}

ExtractDialog::~ExtractDialog() {
    for (auto &it : qhLayoutVtr) {
        delete it;
    };

    for (auto &it : qLabelVtr) {
        delete it;
    }
    delete qvLayout;
    delete extractLabel;
    delete scrollArea;
    delete buttonBox;
    delete mainLayout;
}

void ExtractDialog::YesToAll() {
    for (unsigned int i = 0; i < comicBook->getSize(); ++i) {
        qLabelVtr[i]->setChosen(true);
    }
}

void ExtractDialog::NoToAll() {
    for (unsigned int i = 0; i < comicBook->getSize(); ++i) {
        qLabelVtr[i]->setChosen(false);
    }
}
