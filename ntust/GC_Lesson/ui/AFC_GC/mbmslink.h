#ifndef MBMSLINK_H
#define MBMSLINK_H

#include <QDialog>

namespace Ui {
class MBMSLink;
}

class MBMSLink : public QDialog
{
    Q_OBJECT
public:
    explicit MBMSLink(QWidget *parent = nullptr);
    void setMBMSData(QString);
    ~MBMSLink();
private:
    Ui::MBMSLink *ui;

signals:
    void send_data(QString y);
private slots:
    void on_buttonBox_accepted();
public slots:
    void MBMSdataupdate_slots(double *);
};

#endif // MBMSLINK_H
