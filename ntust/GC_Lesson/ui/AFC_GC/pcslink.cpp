#include "pcslink.h"
#include "ui_pcslink.h"
#include <QtCore>
double pcs_connect = 0;
double pcs_reset = 0;
double pcs_control_time_local = 0;
PCSLink::PCSLink(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PCSLink)
{
    ui->setupUi(this);

}

PCSLink::~PCSLink()
{
    delete ui;
}
void PCSLink::setPCSData(QString pcs_ip, QString pcs_port, QString pcs_id, QString PCS_eid)
{
    ui->lineEdit->setText(pcs_ip);
    ui->lineEdit_2->setText(pcs_port);
    ui->lineEdit_3->setText(pcs_id);
    ui->lineEdit_4->setText(PCS_eid);
}

void PCSLink::on_buttonBox_accepted()
{
    QString pcs_ip = ui->lineEdit->text();
    QString pcs_port = ui->lineEdit_2->text();
    QString pcs_id = ui->lineEdit_3->text();
    QString PCS_eid = ui->lineEdit_4->text();
    emit send_data(pcs_ip, pcs_port, pcs_id, PCS_eid);    //傳出子窗體訊號
    this->close();
}
void PCSLink::PCSdataupdate_slots(double *pcsdata)
{
    ui->lineEdit_5->setText(QString::number(pcsdata[0],10,2));
    ui->lineEdit_6->setText(QString::number(pcsdata[1],10,2));
    ui->lineEdit_7->setText(QString::number(pcsdata[2],10,2));
    ui->lineEdit_8->setText(QString::number(pcsdata[3],10,2));
    ui->lineEdit_9->setText(QString::number(pcsdata[4],10,2));
    ui->lineEdit_10->setText(QString::number(pcsdata[5],10,2));
    ui->lineEdit_11->setText(QString::number(pcsdata[6],10,2));
    ui->lineEdit_12->setText(QString::number(pcsdata[7],10,2));
    ui->lineEdit_13->setText(QString::number(pcsdata[8],10,2));
    ui->lineEdit_14->setText(QString::number(pcsdata[9],10,2));
    ui->lineEdit_15->setText(QString::number(pcsdata[10],10,2));
    ui->lineEdit_16->setText(QString::number(pcsdata[11],10,2));
    ui->lineEdit_17->setText(QString::number(pcsdata[12],10,2));
    ui->lineEdit_18->setText(QString::number(pcsdata[13],10,2));
    ui->lineEdit_19->setText(QString::number(pcsdata[14],10,2));
    ui->lineEdit_20->setText(QString::number(pcsdata[15],10,2));
    ui->lineEdit_21->setText(QString::number(pcsdata[16],10,2));
    ui->lineEdit_22->setText(QString::number(pcsdata[17],10,2));
    ui->lineEdit_23->setText(QString::number(pcsdata[18],10,2));
    ui->lineEdit_24->setText(QString::number(pcsdata[19],10,2));
    ui->lineEdit_25->setText(QString::number(pcsdata[20],10,2));
    ui->lineEdit_26->setText(QString::number(pcsdata[21],10,0));
    ui->lineEdit_27->setText(QString::number(pcsdata[22],10,0));
    ui->lineEdit_28->setText(QString::number(pcsdata[23],10,0));
    ui->lineEdit_29->setText(QString::number(pcsdata[24],10,0));
    ui->lineEdit_30->setText(QString::number(pcsdata[25],10,0));
    ui->lineEdit_31->setText(QString::number(pcsdata[26],10,0));
    ui->lineEdit_32->setText(QString::number(pcsdata[27],10,0));
    ui->lineEdit_33->setText(QString::number(pcsdata[28],10,0));
    ui->lineEdit_34->setText(QString::number(pcsdata[29],10,0));
    ui->lineEdit_35->setText(QString::number(pcsdata[30],10,0));
    ui->lineEdit_36->setText(QString::number(pcsdata[31],10,0));
    ui->lineEdit_37->setText(QString::number(pcsdata[32],10,0));
    ui->lineEdit_38->setText(QString::number(pcsdata[33],10,0));
    ui->lineEdit_39->setText(QString::number(pcsdata[34],10,0));
    ui->lineEdit_40->setText(QString::number(pcsdata[35],10,0));
    ui->lineEdit_41->setText(QString::number(pcsdata[36],10,0));
    ui->lineEdit_42->setText(QString::number(pcsdata[37],10,0));
    ui->lineEdit_43->setText(QString::number(pcsdata[38],10,0));
    ui->lineEdit_44->setText(QString::number(pcsdata[39],10,0));
    ui->lineEdit_45->setText(QString::number(pcsdata[40],10,0));
    ui->lineEdit_46->setText(QString::number(pcsdata[41],10,0));
    ui->lineEdit_47->setText(QString::number(pcsdata[42],10,0));
    ui->lineEdit_48->setText(QString::number(pcsdata[43],10,0));
    ui->lineEdit_49->setText(QString::number(pcsdata[44],10,2));
    ui->lineEdit_50->setText(QString::number(pcsdata[45],10,2));
}

void PCSLink::on_pushButton_pressed()
{
    pcs_connect = 1;
//    pcs_control_time_local = QDateTime::currentDateTime().toMSecsSinceEpoch();
    pcs_control_time_local = 12233;
    emit send_control(pcs_connect, pcs_reset, pcs_control_time_local);
}

void PCSLink::on_pushButton_2_pressed()
{
    pcs_connect = 2;
     pcs_control_time_local = QDateTime::currentDateTime().toMSecsSinceEpoch();
    emit send_control(pcs_connect, pcs_reset, pcs_control_time_local);
}

void PCSLink::on_pushButton_3_pressed()
{
    pcs_reset = 1;
    pcs_control_time_local = QDateTime::currentDateTime().toMSecsSinceEpoch();;
    emit send_control(pcs_connect, pcs_reset, pcs_control_time_local);
}

