#ifndef GAME_H
#define GAME_H

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QVector>


namespace Ui {
class Game;
}

class Game : public QWidget
{
    Q_OBJECT

public:
    explicit Game(QWidget *parent = 0);
    ~Game();
    int checkWin(QVector<int> pole);
    int checkPotentialWin(QVector<int> pole);

    void makeStep(void);
    int checkHumanStep(QVector<int> pole_inc, int level);
    int checkCompStep(QVector<int> pole_inc, int level);

public slots:
    void getInitParam(int h, bool fi, bool fl);

private:
    Ui::Game *ui;    

    QVector<int> pole;      // Игровое поле
    QVector<int> win_prob;  // Расчёт вероятности выигрыша для каждой клетки
    int win;                // Флаг выигрыша/ничьей, окончания игры
    int avail_;             // Число свободных клеток
    int comp_flag;          // Метка, которой ходит комп (крестик/нолик)
    int human_flag;         // Метка, которой ходит человек (крестик/нолик)
    int win_row;            // Полоска выигрыша
    int debuginfo_enable;
    int human_first;        // Первый ход делает челвоек
    int hardness;           // Сложность - 4-максимум, 1-тупит иногда

protected:
    void paintEvent( QPaintEvent *);
    virtual void mousePressEvent( QMouseEvent * event) override;
};

#endif // GAME_H
