#include "emslink.h"
#include "ui_emslink.h"
#include "dllSysTest.h"
#include <iostream>
#include <mainwindow.h>
#include <QtCore>
EMSLink::EMSLink(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EMSLink)
{
    ui->setupUi(this);
    thread_mongo = new dllSysTest();
}

EMSLink::~EMSLink()
{
    delete thread_mongo;
    delete ui;
}

void EMSLink::setEMSData(QString ems_redundant, QString ems_ip1, QString ems_port1, QString ems_ip2, QString ems_port2, QString user, QString pwd)
{
    ui->checkBox->setCheckState((Qt::CheckState)ems_redundant.toInt());
    ui->lineEdit->setText(ems_ip1);
    ui->lineEdit_3->setText(ems_port1);
    ui->lineEdit_2->setText(ems_ip2);
    ui->lineEdit_4->setText(ems_port2);
    ui->lineEdit_5->setText(user);
    ui->lineEdit_6->setText(pwd);

}
void EMSLink::on_checkBox_stateChanged(int arg1)
{
    if (arg1==0){
        ui->label_3->setDisabled(true);
        ui->lineEdit_2->setDisabled(true);
        ui->pushButton_2->setDisabled(true);
        ui->label_5->setDisabled(true);
        ui->lineEdit_4->setDisabled(true);
        ui->pushButton_6->setDisabled(true);
    }
    else{
        ui->label_3->setDisabled(false) ;
        ui->lineEdit_2->setDisabled(false) ;
        ui->pushButton_2->setDisabled(false) ;
        ui->label_5->setDisabled(false);
        ui->lineEdit_4->setDisabled(false);
        ui->pushButton_6->setDisabled(false);
    }
}


void EMSLink::on_pushButton_5_released()
{
    ui->textEdit->clear();
}


void EMSLink::on_checkBox_clicked(bool checked)
{
    //on off切換
    if (checked==true){
        ui->textEdit->insertPlainText("啟用備援\n");
    }
    else{
        ui->textEdit->insertPlainText("停用備援\n");
    }

}


void EMSLink::on_pushButton_released()
{
    dllSysTest sys_test;
    sys_test.IP=ui->lineEdit->text().toStdString();
    sys_test.ping();
}


void EMSLink::on_pushButton_2_released()
{
    dllSysTest sys_test;
    sys_test.IP=ui->lineEdit_2->text().toStdString();
    sys_test.ping();}


void EMSLink::on_pushButton_3_released()
{
    thread_mongo->IP = ui->lineEdit->text().toStdString();
    thread_mongo->user = ui->lineEdit_5->text().toStdString();
    thread_mongo->pwd = ui->lineEdit_6->text().toStdString();
    thread_mongo->mongo_port = ui->lineEdit_3->text().toInt();
    bool ans = thread_mongo->mongo();
    if (ans == true){
        ui->textEdit->insertPlainText("mongo "+ui->lineEdit->text()+" 連線成功\n");
    }
    else{
        ui->textEdit->insertPlainText("mongo "+ui->lineEdit->text()+" 連線失敗\n");
    }
}


void EMSLink::on_buttonBox_accepted()
{
    QString ems_redundant = QString::number(ui->checkBox->checkState());
    QString ems_ip1 = ui->lineEdit->text();
    QString ems_port1 = ui->lineEdit_3->text();
    QString ems_ip2 = ui->lineEdit_2->text();
    QString ems_port2 = ui->lineEdit_4->text();
    QString user = ui->lineEdit_5->text();
    QString pwd = ui->lineEdit_6->text();
    emit send_data(ems_redundant, ems_ip1, ems_port1, ems_ip2, ems_port2, user, pwd);    //傳出子窗體訊號
    this->close();
}


void EMSLink::EMSdataupdate_slots(double *emsdata)
{
    ui->lineEdit_7->setText(QString::number(emsdata[0],10,2));
    ui->lineEdit_8->setText(QString::number(emsdata[1],10,2));
    ui->lineEdit_9->setText(QString::number(emsdata[2],10,2));
    ui->lineEdit_10->setText(QString::number(emsdata[3],10,2));
    ui->lineEdit_11->setText(QString::number(emsdata[4],10,2));
    ui->lineEdit_12->setText(QString::number(emsdata[5],10,2));
    ui->lineEdit_13->setText(QString::number(emsdata[6],10,2));
    ui->lineEdit_14->setText(QString::number(emsdata[7],10,2));
    ui->lineEdit_15->setText(QString::number(emsdata[8],10,2));
    ui->lineEdit_16->setText(QString::number(emsdata[9],10,2));
    ui->lineEdit_17->setText(QString::number(emsdata[10],10,2));
    ui->lineEdit_18->setText(QString::number(emsdata[11],10,2));
    ui->lineEdit_19->setText(QString::number(emsdata[12],10,2));
    ui->lineEdit_20->setText(QString::number(emsdata[13],10,2));
    ui->lineEdit_21->setText(QString::number(emsdata[14],10,2));
    ui->lineEdit_22->setText(QString::number(emsdata[15],10,2));
    ui->lineEdit_23->setText(QString::number(emsdata[16],10,2));
    ui->lineEdit_24->setText(QString::number(emsdata[17],10,2));
    ui->lineEdit_25->setText(QString::number(emsdata[18],10,2));
    ui->lineEdit_26->setText(QString::number(emsdata[19],10,2));
    ui->lineEdit_27->setText(QString::number(emsdata[20],10,2));
    ui->lineEdit_28->setText(QString::number(emsdata[21],10,2));
    ui->lineEdit_29->setText(QString::number(emsdata[22],10,2));
    ui->lineEdit_30->setText(QString::number(emsdata[23],10,2));
    ui->lineEdit_31->setText(QString::number(emsdata[24],10,2));
    ui->lineEdit_32->setText(QString::number(emsdata[25],10,2));
    ui->lineEdit_33->setText(QString::number(emsdata[26],10,2));
    ui->lineEdit_34->setText(QString::number(emsdata[27],10,2));
    ui->lineEdit_35->setText(QString::number(emsdata[28],10,2));
    ui->lineEdit_36->setText(QString::number(emsdata[29],10,2));
    ui->lineEdit_37->setText(QString::number(emsdata[30],10,2));
    ui->lineEdit_38->setText(QString::number(emsdata[31],10,2));
    ui->lineEdit_39->setText(QString::number(emsdata[32],10,2));
    ui->lineEdit_40->setText(QString::number(emsdata[33],10,2));
    ui->lineEdit_41->setText(QString::number(emsdata[34],10,2));
    ui->lineEdit_42->setText(QString::number(emsdata[35],10,2));
    ui->lineEdit_43->setText(QString::number(emsdata[36],10,2));
    ui->lineEdit_44->setText(QString::number(emsdata[37],10,2));
    ui->lineEdit_45->setText(QString::number(emsdata[38],10,2));
    ui->lineEdit_46->setText(QString::number(emsdata[39],10,2));
    ui->lineEdit_47->setText(QString::number(emsdata[40],10,2));
    ui->lineEdit_48->setText(QString::number(emsdata[41],10,2));
    ui->lineEdit_49->setText(QString::number(emsdata[42],10,2));
    ui->lineEdit_50->setText(QString::number(emsdata[43],10,2));
    ui->lineEdit_51->setText(QString::number(emsdata[44],10,2));
    ui->lineEdit_52->setText(QString::number(emsdata[45],10,2));
    ui->lineEdit_53->setText(QString::number(emsdata[46],10,2));
    ui->lineEdit_54->setText(QString::number(emsdata[47],10,2));
}




