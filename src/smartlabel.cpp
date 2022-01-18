#include<QString>

#include "smartlabel.h"
#include "comicbook.h"

SmartLabel::SmartLabel(QWidget *parent){
    imageLabel = new QLabel(this);
    imageLabel->setFrameShape(QFrame::Box);
    imageLabel->setStyleSheet("border-width: 0px;border-style: solid;border-color: blue;");
    imageLabel->setGeometry(0, 0, width(), height()*SMARTLABEL_PROP);
    imageLabel->setAlignment(Qt::AlignHCenter);
    textLabel = new QLabel(this);
    textLabel->setGeometry(0, height()-height()/(1-SMARTLABEL_PROP), width(), height()*(1-SMARTLABEL_PROP));
    textLabel->setAlignment(Qt::AlignHCenter);
    empty = true;
}

SmartLabel::SmartLabel(QWidget *parent, QPixmap image, int n) : QLabel(parent){
    imageLabel = new QLabel(this);
    imageLabel->setPixmap(image);
    imageLabel->setFrameShape(QFrame::Box);
    imageLabel->setStyleSheet("border-width: 0px;border-style: solid;border-color: blue;");
    imageLabel->setGeometry(0, 0, width(), height()*SMARTLABEL_PROP);
    imageLabel->setAlignment(Qt::AlignHCenter);
    textLabel = new QLabel(this);
    textLabel->setText(QString("%1").arg(n+1));
    textLabel->setGeometry(0, height()-height()/(1-SMARTLABEL_PROP), width(), height()*(1-SMARTLABEL_PROP));
    textLabel->setAlignment(Qt::AlignHCenter);
    empty = false;
    num = n;
}

SmartLabel::~SmartLabel(){
    delete imageLabel;
    delete textLabel;
}

bool SmartLabel::hasChosen()
{

   return chosenFlag;

}

void SmartLabel::setChosen(bool flag){
    chosenFlag = flag;
    if (chosenFlag){
        subComicBook[num] = true;
        imageLabel->setStyleSheet("border-width: 1px;border-style: solid;border-color: blue;");
    }
    else{
        subComicBook[num] = false;
        imageLabel->setStyleSheet("border-width: 0px;border-style: solid;border-color: blue;");
    }
}

void SmartLabel::resize(int w, int h){
    imageLabel->setGeometry(0, 0, w, 5*h/6);
    textLabel->setGeometry(0, h-h/6, w, h/6);
    QLabel::resize(w, h);
}

void SmartLabel::mousePressEvent(QMouseEvent *event)
{
    if(!empty && event->button()== Qt::LeftButton)
    {
        chosenFlag = !chosenFlag;
        if (chosenFlag){
            subComicBook[num] = true;
            imageLabel->setStyleSheet("border-width: 1px;border-style: solid;border-color: blue;");
        }
        else{
            subComicBook[num] = false;
            imageLabel->setStyleSheet("border-width: 0px;border-style: solid;border-color: blue;");
        }
    }
}
