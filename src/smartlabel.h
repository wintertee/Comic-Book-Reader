#ifndef SMARTLABEL_H
#define SMARTLABEL_H

#include <QLabel>
#include <QMouseEvent>
#include <QString>
#include <QPixmap>

#define SMARTLABEL_PROP 5.0/6.0 // the proportion of image part

class SmartLabel : public QLabel{
    Q_OBJECT
public:

    SmartLabel(QWidget *parent);
    SmartLabel(QWidget *parent, QPixmap image, int num);
    ~SmartLabel();

    bool hasChosen();
    void setChosen(bool flag);
    void resize(int w, int h);

protected:

    void mousePressEvent(QMouseEvent *event);

private:

    QLabel *imageLabel;
    QLabel *textLabel;

    bool chosenFlag = false;
    int num = -1;
    bool empty = true;
};

#endif // SMARTLABEL_H
