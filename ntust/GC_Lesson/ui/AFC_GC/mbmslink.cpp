#include "mbmslink.h"
#include "ui_mbmslink.h"
#include <QtCore>
MBMSLink::MBMSLink(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MBMSLink)
{
    ui->setupUi(this);
}

MBMSLink::~MBMSLink()
{
    delete ui;
}
void MBMSLink::setMBMSData(QString mbms_eid)
{
    ui->lineEdit->setText(mbms_eid);
}

void MBMSLink::on_buttonBox_accepted()
{
    QString mbms_id = ui->lineEdit->text();
    emit send_data(mbms_id);    //傳出子窗體訊號
    this->close();
}

void MBMSLink::MBMSdataupdate_slots(double *mbmsdata)
{
    ui->lineEdit_2->setText(QString::number(mbmsdata[0],10,2));
}


