#ifndef SMARTLABEL_H
#define SMARTLABEL_H

#include <QLabel>
#include <QMouseEvent>
#include <QPixmap>
#include <QString>

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

#endif // SMARTLABEL_H
