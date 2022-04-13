#ifndef METERLINK_H
#define METERLINK_H

#include <QDialog>

namespace Ui {
class MeterLink;
}

class MeterLink : public QDialog
{
    Q_OBJECT

public:
    explicit MeterLink(QWidget *parent = nullptr);
    void setMeterData(QString, QString, QString, QString);
    ~MeterLink();

private:
    Ui::MeterLink *ui;
signals:
    void send_data(QString meter_ip, QString meter_port, QString meter_id, QString meter_eid);
private slots:
    void on_buttonBox_accepted();
public slots:
    void Meterdataupdate_slots(double *);
};

#endif // METERLINK_H
