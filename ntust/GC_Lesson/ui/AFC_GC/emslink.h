#ifndef EMSLINK_H
#define EMSLINK_H

#include <QDialog>
#include "dllSysTest.h"

namespace Ui {
class EMSLink;
}

class EMSLink : public QDialog
{
    Q_OBJECT

public:
    explicit EMSLink(QWidget *parent = nullptr);

    void setEMSData(QString, QString, QString, QString, QString, QString, QString);
    std::string ems2on_offb;
    std::string ems_ip1b;
    std::string ems_port1b;
    std::string ems_ip2b;
    std::string ems_port2b;
    std::string userb;
    std::string pwdb;
    ~EMSLink();


private slots:

    void on_checkBox_stateChanged(int arg1);

    void on_pushButton_5_released();

    void on_checkBox_clicked(bool checked);

    void on_pushButton_released();

    void on_pushButton_2_released();

    void on_pushButton_3_released();

    void on_buttonBox_accepted();

private:
    Ui::EMSLink *ui;
    dllSysTest *thread_mongo;
signals:
    void send_data(QString ems2on_off, QString ems_ip1, QString ems_ip2, QString ems_port1, QString ems_port2, QString user, QString pwd);
public slots:
    void EMSdataupdate_slots(double *);
};

#endif // EMSLINK_H
