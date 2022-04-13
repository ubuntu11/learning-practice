#include "parameter_set.h"
#include "ui_parameter_set.h"

parameter_set::parameter_set(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::parameter_set)
{
    ui->setupUi(this);
}

parameter_set::~parameter_set()
{
    delete ui;
}
