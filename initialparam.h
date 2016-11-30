#ifndef INITIALPARAM_H
#define INITIALPARAM_H

#include <QWidget>

namespace Ui {
class InitialParam;
}

class InitialParam : public QWidget
{
    Q_OBJECT

public:
    explicit InitialParam(QWidget *parent = 0);
    ~InitialParam();

signals:
    void setInitParams(int,bool,bool);

private slots:
    void getInitParams(void);

private:
    Ui::InitialParam *ui;
};

#endif // INITIALPARAM_H
