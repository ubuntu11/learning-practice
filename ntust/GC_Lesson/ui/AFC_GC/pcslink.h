#ifndef PCSLINK_H
#define PCSLINK_H

#include <QDialog>

namespace Ui {
class PCSLink;
}

class PCSLink : public QDialog
{
    Q_OBJECT

public:
    explicit PCSLink(QWidget *parent = nullptr);
    void setPCSData(QString, QString, QString, QString);
    ~PCSLink();

private:
    Ui::PCSLink *ui;
signals:
    void send_data(QString pcs_ip, QString pcs_port, QString pcs_id, QString PCS_eid);
    void send_control(double pcs_connect, double pcs_reset, double pcs_control_time_local);
private slots:
    void on_buttonBox_accepted();
    void on_pushButton_pressed();

    void on_pushButton_2_pressed();

    void on_pushButton_3_pressed();

public slots:
    void PCSdataupdate_slots(double *);
};

#endif // PCSLINK_H
