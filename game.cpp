#include "game.h"
#include "ui_game.h"
#include "initialparam.h"


Game::Game(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Game)
{
    ui->setupUi(this);
    setMouseTracking(true);
    // pole - игровое поле, 0 - свободно, 1 - плюсик, 2 - кружочег
    // порядок клеток - сверхуслева->вправовниз


    pole.resize(9);
    pole.fill(0,9);
    win_prob.resize(9);
    win_prob.fill(-1000,9);

    win = 0;        // Сброс флага выигрыша
    comp_flag = 1;       // Компутер играет плюсиками по умолчанию
    human_flag = 2;
    win_row = 0;        // Обнулить
    human_first = 0;

    // Окно с параметрами игры, по нажатию на кнопку оно закрывается
    // и через слот передаёт параметры
    InitialParam *iniparam = new InitialParam();
    iniparam->show();
    iniparam->setWindowState(Qt::WindowActive);
    connect(iniparam,SIGNAL(setInitParams(int,bool,bool)),this,SLOT(getInitParam(int,bool,bool)));

    // DRAW POLE
    //debuginfo_enable = 1;
    this->update();

}

Game::~Game()
{
    delete ui;
}

// Шаг компьютера
void Game::makeStep(void)
{
    QVector<int> pole_temp;
    int level = 1000;    // просто большое число, урвоень вложенности
                        // и заодно приоритет, чем больше вложенность - тем меньше приоритет
    avail_ = pole.count(0); // Число свободных клеток
    // Делать шаг, если ещё нет выигрыша или пока есть свободные клетки
    if((win==0) & (avail_!=0))
    {
        int avail = 0;        
        win_prob.fill(-1000,9);
        for(int i=8;i>=0;i--)
        {   // Расчёт вероятности выигрыша перебором
            if(pole.at(i) == 0)
            {
                avail++;
                pole_temp = pole;
                // Поставить метку в первую свободную ячейку
                pole_temp.replace(i,comp_flag);
                // Проверить, а вдруг выигрыш
                if(checkPotentialWin(pole_temp)==comp_flag)
                {   // Нифига! Обыграли глупого юзера
                    win_prob.replace(i,level);   // Наивысший приоритет хода
                }
                else
                {   // Проверим, что будет, если чоловик поставит что-нибудь
                    win_prob.replace(i,checkHumanStep(pole_temp,level/5));
                }
            }
        } // for pole[i]

        // Сортировка результатов
        QVector<QVector2D> sort_index;
        for(int i=0;i<9;i++)
        {
            if( win_prob.at(i)!=-1000)
            {
                sort_index.append(QVector2D(i,win_prob.at(i)));
            }
        }
        QVector2D temp1;
        for(int i=1;i<sort_index.size();i++)
        {
            for(int j=1;j<sort_index.size();j++)
            {
                 if(sort_index.at(j).y() > sort_index.at(j-1).y())
                 {
                     temp1 = sort_index.at(j);
                     sort_index.replace(j,sort_index.at(j-1));
                     sort_index.replace(j-1,temp1);
                 }
            }
        }
        // Здесь тупенький комп начинает тупить
        int max_ind=0;
                // коэффициент, вроде робит 4=длина ползунка сложности
        int rand_noise = (qrand()%sort_index.size())*(4-hardness)*0.2;
        max_ind += rand_noise;
        if(max_ind > (sort_index.size()-1))
            max_ind = sort_index.size()-1;
        // Поставить флаг в клетку с максимальной вероятностью
        if(pole.at(sort_index.at(max_ind).x())==0)
            pole.replace(sort_index.at(max_ind).x(),comp_flag);
        if(checkWin(pole)==comp_flag)
            win = comp_flag;
        avail_ = pole.count(0);
    }
    // Ничья
    if((avail_==0) & (win==0))
    {
        win = 3;
    }

    this->update();
}

// Проверить, выиграл ли кто-нибудь в этом ходу
int Game::checkWin(QVector<int> pole)
{
    int win_player = 0;         // Ничья
    for(int player=1;player<=2;player++)
    {
        // 3 горизонтальные
        if((pole.at(0)==player) & (pole.at(1)==player) & (pole.at(2)==player)) {
            win_player = player; win_row = 1; }
        else if ((pole.at(3)==player) & (pole.at(4)==player) & (pole.at(5)==player)) {
            win_player = player; win_row = 2; }
        else if ((pole.at(6)==player) & (pole.at(7)==player) & (pole.at(8)==player)) {
            win_player = player; win_row = 3; }
        // 3 вертикальные
        else if ((pole.at(0)==player) & (pole.at(3)==player) & (pole.at(6)==player)) {
            win_player = player; win_row = 4; }
        else if ((pole.at(1)==player) & (pole.at(4)==player) & (pole.at(7)==player)) {
            win_player = player; win_row = 5; }
        else if ((pole.at(2)==player) & (pole.at(5)==player) & (pole.at(8)==player)) {
            win_player = player; win_row = 6; }
        // 2 диаголнальные
        else if ((pole.at(0)==player) & (pole.at(4)==player) & (pole.at(8)==player)) {
            win_player = player; win_row = 7; }
        else if ((pole.at(2)==player) & (pole.at(4)==player) & (pole.at(6)==player)) {
            win_player = player; win_row = 8; }
    }
    return win_player;
}

// Проверить, выиграл ли кто-нибудь в этом (предполагаемом) ходу
int Game::checkPotentialWin(QVector<int> pole)
{
    int win_player = 0;         // Ничья
    for(int player=1;player<=2;player++)
    {
        // 3 горизонтальные
        if((pole.at(0)==player) & (pole.at(1)==player) & (pole.at(2)==player)) {
            win_player = player; }
        else if ((pole.at(3)==player) & (pole.at(4)==player) & (pole.at(5)==player)) {
            win_player = player; }
        else if ((pole.at(6)==player) & (pole.at(7)==player) & (pole.at(8)==player)) {
            win_player = player; }
        // 3 вертикальные
        else if ((pole.at(0)==player) & (pole.at(3)==player) & (pole.at(6)==player)) {
            win_player = player; }
        else if ((pole.at(1)==player) & (pole.at(4)==player) & (pole.at(7)==player)) {
            win_player = player; }
        else if ((pole.at(2)==player) & (pole.at(5)==player) & (pole.at(8)==player)) {
            win_player = player; }
        // 2 диаголнальные
        else if ((pole.at(0)==player) & (pole.at(4)==player) & (pole.at(8)==player)) {
            win_player = player; }
        else if ((pole.at(2)==player) & (pole.at(4)==player) & (pole.at(6)==player)) {
            win_player = player; }
    }
    return win_player;
}

// В рекурсии, расчёт вероятности выигрыша после ходов челвоека
int Game::checkHumanStep(QVector<int> pole_inc, int level)
{
    int prob = 0;
    if(level!=0)
    {
        QVector<int> pole_temp;
        for(int i=8;i>=0;i--)
        {
            if(pole_inc.at(i)==0)
            {
                pole_temp = pole_inc;
                pole_temp.replace(i,human_flag);
                if(checkPotentialWin(pole_temp)==human_flag)
                    prob-=level;
                else
                {
                    prob+=checkCompStep(pole_temp,level/3);
                }
            }
        }
    }
    return prob;
}
// В рекурсии, вычисление вероятности выигрыша при шаге компутером
int Game::checkCompStep(QVector<int> pole_inc, int level)
{
    int prob = 0;
    if(level!=0)
    {
        QVector<int> pole_temp;
        for(int i=8;i>=0;i--)
        {
            if(pole_inc.at(i)==0)
            {
                pole_temp = pole_inc;
                pole_temp.replace(i,comp_flag);
                if(checkPotentialWin(pole_temp)==comp_flag)
                    prob+=level;
                else
                {
                    prob+=checkHumanStep(pole_temp,level/3);
                }
            }
        }
    }
    return prob;
}

// Отрисовка поля, клеток и всего
void Game::paintEvent( QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    int wid = this->rect().width();
    int hei = this->rect().height();
    QPen pen;
    pen.setBrush(Qt::SolidPattern);
    pen.setColor(QColor(0x0f,0x0f,0x0f,0x0f));
    // Рисуем поле
    pen.setWidth(wid*0.01);
    painter.setPen(pen);
    painter.drawLine(0.33*wid,0,0.33*wid,hei);
    painter.drawLine(0.66*wid,0,0.66*wid,hei);
    painter.drawLine(0,0.33*hei,wid,0.33*hei);
    painter.drawLine(0,0.66*hei,wid,0.66*hei);
    // Рисуем закрашенные ячейки
    pen.setColor(QColor(0x00,0x00,0x00,0xff));
    pen.setWidth(wid*0.05);
    painter.setPen(pen);
    // 0
    if(pole.at(0)==1) {
        painter.drawLine(0.1*wid,0.1*hei,0.22*wid,0.22*hei);
        painter.drawLine(0.22*wid,0.1*hei,0.1*wid,0.22*hei);
    }
    else if(pole.at(0)==2){
        painter.drawEllipse(QPoint(0.166*wid,0.166*hei),0.07*wid,0.07*hei);
    }
    // 1
    if(pole.at(1)==1) {
        painter.drawLine(0.44*wid,0.1*hei,0.56*wid,0.22*hei);
        painter.drawLine(0.56*wid,0.1*hei,0.44*wid,0.22*hei);
    }
    else if(pole.at(1)==2){
        painter.drawEllipse(QPoint(0.5*wid,0.166*hei),0.07*wid,0.07*hei);
    }
    // 2
    if(pole.at(2)==1) {
        painter.drawLine(0.78*wid,0.1*hei,0.9*wid,0.22*hei);
        painter.drawLine(0.9*wid,0.1*hei,0.78*wid,0.22*hei);
    }
    else if(pole.at(2)==2){
        painter.drawEllipse(QPoint(0.833*wid,0.166*hei),0.07*wid,0.07*hei);
    }
    // 3
    if(pole.at(3)==1) {
        painter.drawLine(0.1*wid,0.44*hei,0.22*wid,0.56*hei);
        painter.drawLine(0.22*wid,0.44*hei,0.1*wid,0.56*hei);
    }
    else if(pole.at(3)==2){
        painter.drawEllipse(QPoint(0.166*wid,0.5*hei),0.07*wid,0.07*hei);
    }
    // 4
    if(pole.at(4)==1) {
        painter.drawLine(0.44*wid,0.44*hei,0.56*wid,0.56*hei);
        painter.drawLine(0.56*wid,0.44*hei,0.44*wid,0.56*hei);
    }
    else if(pole.at(4)==2){
        painter.drawEllipse(QPoint(0.5*wid,0.5*hei),0.07*wid,0.07*hei);
    }
    // 5
    if(pole.at(5)==1) {
        painter.drawLine(0.78*wid,0.44*hei,0.9*wid,0.56*hei);
        painter.drawLine(0.9*wid,0.44*hei,0.78*wid,0.56*hei);
    }
    else if(pole.at(5)==2){
        painter.drawEllipse(QPoint(0.833*wid,0.5*hei),0.07*wid,0.07*hei);
    }
    // 6
    if(pole.at(6)==1) {
        painter.drawLine(0.1*wid,0.78*hei,0.22*wid,0.9*hei);
        painter.drawLine(0.22*wid,0.78*hei,0.1*wid,0.9*hei);
    }
    else if(pole.at(6)==2){
        painter.drawEllipse(QPoint(0.166*wid,0.833*hei),0.07*wid,0.07*hei);
    }
    // 7
    if(pole.at(7)==1) {
        painter.drawLine(0.44*wid,0.78*hei,0.56*wid,0.9*hei);
        painter.drawLine(0.56*wid,0.78*hei,0.44*wid,0.9*hei);
    }
    else if(pole.at(7)==2){
        painter.drawEllipse(QPoint(0.5*wid,0.82*hei),0.07*wid,0.07*hei);
    }
    // 8
    if(pole.at(8)==1) {
        painter.drawLine(0.78*wid,0.78*hei,0.9*wid,0.9*hei);
        painter.drawLine(0.9*wid,0.78*hei,0.78*wid,0.9*hei);
    }
    else if(pole.at(8)==2){
        painter.drawEllipse(QPoint(0.833*wid,0.833*hei),0.07*wid,0.07*hei);
    }

    // Полоса выигрыша
    pen.setColor(QColor(0xff,0x00,0x00,0xff));
    pen.setWidth(wid*0.1);
    painter.setPen(pen);
    switch(win_row){
    case 0: break;
    case 1: painter.drawLine(0.1*wid,0.166*hei,0.9*wid,0.166*hei); break;
    case 2: painter.drawLine(0.1*wid,0.5*hei,0.9*wid,0.5*hei); break;
    case 3: painter.drawLine(0.1*wid,0.82*hei,0.9*wid,0.82*hei); break;
    case 4: painter.drawLine(0.166*wid,0.1*hei,0.166*wid,0.9*hei); break;
    case 5: painter.drawLine(0.5*wid,0.1*hei,0.5*wid,0.9*hei); break;
    case 6: painter.drawLine(0.82*wid,0.1*hei,0.82*wid,0.9*hei); break;
    case 7: painter.drawLine(0.1*wid,0.1*hei,0.9*wid,0.9*hei); break;
    case 8: painter.drawLine(0.9*wid,0.1*hei,0.1*wid,0.9*hei); break;
    default: break;
    }

    // Надпись окончания игры
    if(win!=0)
    {
        QBrush brush;
        brush.setStyle(Qt::SolidPattern);
        QFont font;
        font.setPointSize(0.028*wid);
        QPen pen;
        pen.setColor(QColor(Qt::black));
        painter.setFont(font);
        painter.setPen(pen);
        QRect table = QRect(QPoint(0,0.3*hei),QSize(wid,0.4*hei));
        if(win==comp_flag)
        {
            brush.setColor(QColor(0xff,0x00,0x00,0xf0));
            painter.setBrush(brush);
            painter.fillRect(table,brush);
            painter.drawText(table,
                             Qt::AlignCenter,
                             QString("Ой-вей, комплюктер победил"));

        }
        else if(win==human_flag)
        {
            brush.setColor(QColor(0x00,0xff,0x00,0xf0));
            painter.setBrush(brush);
            painter.drawRect(table);
            painter.drawText(table,
                             Qt::AlignCenter,
                             QString("Бездумная машина проиграла"));
        }
        else
        {
            brush.setColor(QColor(0xff,0xc0,0x00,0xf0));
            painter.setBrush(brush);
            painter.drawRect(table);
            painter.drawText(table,
                             Qt::AlignCenter,
                             QString("Ну фиг знает.."));

        }

    }
    if(debuginfo_enable==1)
    {
        QPen pen1;
        pen1.setColor(QColor(Qt::black));
        painter.setPen(pen1);
        QFont font1;
        font1.setPointSize(0.03*wid);
        painter.setFont(font1);
        painter.drawText(QRect(0,0.2*hei,0.33*wid,0.1*hei),
                         Qt::AlignRight,
                         QString::number(win_prob.at(0)));
        painter.drawText(QRect(0.33*wid,0.2*hei,0.33*wid,0.1*hei),
                         Qt::AlignRight,
                         QString::number(win_prob.at(1)));
        painter.drawText(QRect(0.66*wid,0.2*hei,0.33*wid,0.1*hei),
                         Qt::AlignRight,
                         QString::number(win_prob.at(2)));
        painter.drawText(QRect(0*wid,0.5*hei,0.33*wid,0.1*hei),
                         Qt::AlignRight,
                         QString::number(win_prob.at(3)));
        painter.drawText(QRect(0.33*wid,0.5*hei,0.33*wid,0.1*hei),
                         Qt::AlignRight,
                         QString::number(win_prob.at(4)));
        painter.drawText(QRect(0.66*wid,0.5*hei,0.33*wid,0.1*hei),
                         Qt::AlignRight,
                         QString::number(win_prob.at(5)));
        painter.drawText(QRect(0*wid,0.9*hei,0.33*wid,0.1*hei),
                         Qt::AlignRight,
                         QString::number(win_prob.at(6)));
        painter.drawText(QRect(0.33*wid,0.9*hei,0.33*wid,0.1*hei),
                         Qt::AlignRight,
                         QString::number(win_prob.at(7)));
        painter.drawText(QRect(0.66*wid,0.9*hei,0.33*wid,0.1*hei),
                         Qt::AlignRight,
                         QString::number(win_prob.at(8)));
    }

}

// Отслеживание нажатий на клетки
void Game::mousePressEvent(QMouseEvent *event)
{
    if(win==0)
    {
        if(event->button() == Qt::LeftButton)
        {
            int correct_step = 0;
            int wid = this->rect().width();
            int hei = this->rect().height();
            //      QTransform t;
            //      t.scale(1,1);
            QPoint pos = event->pos();
            // Вычисленеи координат
            if(pos.y()<0.33*hei)
            {   // Первая строка
                if(pos.x()<0.33*wid) {
                    if(pole.at(0)==0)
                    {
                        pole.replace(0,human_flag);
                        correct_step = 1;
                    }
                }
                else if(pos.x()<0.66*wid) {
                    if(pole.at(1)==0)
                    {
                        pole.replace(1,human_flag);
                        correct_step = 1;
                    }
                }
                else {
                    if(pole.at(2)==0) {
                        pole.replace(2,human_flag);
                        correct_step = 1;
                    }
                }
            }
            else if(pos.y()<0.66*hei)
            {   // Вторая строка
                if(pos.x()<0.33*wid) {
                    if(pole.at(3)==0) {
                        pole.replace(3,human_flag);
                        correct_step = 1;
                    }
                }
                else if(pos.x()<0.66*wid) {
                    if(pole.at(4)==0) {
                        pole.replace(4,human_flag);
                        correct_step = 1;
                    }
                }
                else {
                    if(pole.at(5)==0) {
                        pole.replace(5,human_flag);
                        correct_step = 1;
                    }
                }
            }
            else
            {   // Третья строка
                if(pos.x()<0.33*wid) {
                    if(pole.at(6)==0) {
                        pole.replace(6,human_flag);
                        correct_step = 1;
                    }
                }
                else if(pos.x()<0.66*wid) {
                    if(pole.at(7)==0) {
                        pole.replace(7,human_flag);
                        correct_step = 1;
                    }
                }
                else {
                    if(pole.at(8)==0) {
                        pole.replace(8,human_flag);
                        correct_step = 1;
                    }
                }
            }
            // Проверка, что нажали в свободную клетку
            if(correct_step)
            {
                if(checkWin(pole)==human_flag)
                    win = human_flag;
                else
                    makeStep();
                this->update();
            }
        }
    }
    else
    {
        if(event->button() == Qt::LeftButton)
        {
            win = 0;
            win_row = 0;
            pole.fill(0,9);
        }
        if(human_first)
        {
            // do nothing, wait click
        }
        else
        {   // step by computer first
            makeStep();
        }
        this->update();
    }
}

// Слот принятия параметров из окна параметров, запускает игру
void Game::getInitParam(int h,bool fi,bool fl)
{
    hardness = h;
    if(fi)
        human_first = 1;
    else
        human_first = 0;
    if(fl)
    {
        comp_flag = 1;
        human_flag = 2;
    }
    else
    {
        comp_flag = 2;
        human_flag = 1;
    }

    if(human_first)
    {
        //  do nothing, wait signal to makeStep from mouseEvent;
    }
    else
    {       // first step by computer
          makeStep();
    }
    this->update();
}
