#include "initialparam.h"
#include "ui_initialparam.h"

InitialParam::InitialParam(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InitialParam)
{
    ui->setupUi(this);
    ui->first_hum->setChecked(true);
    ui->flag_crest->setChecked(true);
    connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(getInitParams()));
}

InitialParam::~InitialParam()
{
    delete ui;
}

void InitialParam::getInitParams(void)
{
    int h;
    h = ui->hadr_slider->value();
    bool fs;
    if(ui->first_comp->isChecked())
        fs = false;
    else
        fs = true;
    bool fl;
    if(ui->flag_crest->isChecked())
        fl = true;
    else
        fl = false;
    emit setInitParams(h,fs,fl);
    delete this;
}

