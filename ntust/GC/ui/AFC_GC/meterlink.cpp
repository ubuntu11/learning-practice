#include "meterlink.h"
#include "ui_meterlink.h"

MeterLink::MeterLink(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MeterLink)
{
    ui->setupUi(this);
}

MeterLink::~MeterLink()
{
    delete ui;
}
void MeterLink::setMeterData(QString meter_ip, QString meter_port, QString meter_id, QString meter_eid)
{
    ui->lineEdit->setText(meter_ip);
    ui->lineEdit_2->setText(meter_port);
    ui->lineEdit_3->setText(meter_id);
    ui->lineEdit_4->setText(meter_eid);
}

void MeterLink::on_buttonBox_accepted()
{
    QString meter_ip = ui->lineEdit->text();
    QString meter_port = ui->lineEdit_2->text();
    QString meter_id = ui->lineEdit_3->text();
    QString meter_eid = ui->lineEdit_4->text();
    emit send_data(meter_ip, meter_port, meter_id, meter_eid);    //傳出子窗體訊號
    this->close();
}
void MeterLink::Meterdataupdate_slots(double *meterdata)
{
    ui->lineEdit_5->setText(QString::number(meterdata[0],10,2));
    ui->lineEdit_6->setText(QString::number(meterdata[1],10,2));
    ui->lineEdit_7->setText(QString::number(meterdata[2],10,2));
    ui->lineEdit_8->setText(QString::number(meterdata[3],10,2));
    ui->lineEdit_45->setText(QString::number(meterdata[4],10,2));
    ui->lineEdit_9->setText(QString::number(meterdata[5],10,2));
    ui->lineEdit_10->setText(QString::number(meterdata[6],10,2));
    ui->lineEdit_11->setText(QString::number(meterdata[7],10,2));
    ui->lineEdit_12->setText(QString::number(meterdata[8],10,2));
    ui->lineEdit_13->setText(QString::number(meterdata[9],10,2));
    ui->lineEdit_14->setText(QString::number(meterdata[10],10,2));
    ui->lineEdit_15->setText(QString::number(meterdata[11],10,2));
    ui->lineEdit_16->setText(QString::number(meterdata[12],10,2));
    ui->lineEdit_17->setText(QString::number(meterdata[13],10,2));
    ui->lineEdit_18->setText(QString::number(meterdata[14],10,2));
    ui->lineEdit_19->setText(QString::number(meterdata[15],10,2));
    ui->lineEdit_20->setText(QString::number(meterdata[16],10,2));/*p*/
    ui->lineEdit_21->setText(QString::number(meterdata[17],10,2));
    ui->lineEdit_22->setText(QString::number(meterdata[18],10,2));
    ui->lineEdit_23->setText(QString::number(meterdata[19],10,2));
    ui->lineEdit_24->setText(QString::number(meterdata[20],10,2));/*q*/
    ui->lineEdit_25->setText(QString::number(meterdata[21],10,2));
    ui->lineEdit_26->setText(QString::number(meterdata[22],10,2));
    ui->lineEdit_27->setText(QString::number(meterdata[23],10,2));
    ui->lineEdit_28->setText(QString::number(meterdata[24],10,2));
    ui->lineEdit_29->setText(QString::number(meterdata[25],10,2));
    ui->lineEdit_30->setText(QString::number(meterdata[26],10,2));
    ui->lineEdit_31->setText(QString::number(meterdata[27],10,2));
    ui->lineEdit_32->setText(QString::number(meterdata[28],10,2));
    ui->lineEdit_33->setText(QString::number(meterdata[29],10,2));
    ui->lineEdit_34->setText(QString::number(meterdata[30],10,2));
    ui->lineEdit_35->setText(QString::number(meterdata[31],10,2));
    ui->lineEdit_36->setText(QString::number(meterdata[32],10,2));
    ui->lineEdit_37->setText(QString::number(meterdata[33],10,2));
    ui->lineEdit_38->setText(QString::number(meterdata[34],10,2));
    ui->lineEdit_39->setText(QString::number(meterdata[35],10,2));
    ui->lineEdit_40->setText(QString::number(meterdata[36],10,2));
    ui->lineEdit_41->setText(QString::number(meterdata[37],10,2));
    ui->lineEdit_42->setText(QString::number(meterdata[38],10,2));
    ui->lineEdit_43->setText(QString::number(meterdata[39],10,2));
    ui->lineEdit_44->setText(QString::number(meterdata[40],10,2));
}
