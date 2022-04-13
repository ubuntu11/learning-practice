#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "dllSysTest.h"
#include <QThread>
#include <dllgc.h>
#include "emslink.h"
#include "pcslink.h"
#include "mbmslink.h"
#include "meterlink.h"
#include <QtCore>
#include <libconfig.h++>
#include <stdlib.h>
#include <QStandardItemModel>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class AFCBackground:public QThread{
    Q_OBJECT
    public:
        explicit AFCBackground(QObject *parent = 0);
        virtual ~AFCBackground();
        void run();
        bool Stop;
        QString name;
        std::string ems_redundant;
        std::string user;
        std::string pwd;
        std::string IP1;
        int mongo_port1;
        std::string IP2;
        int mongo_port2;

        std::string MBMS_eid; std::string HM_eid; std::string PCS_eid;
        std::string set_source;
        int mode; double scale; double soc_max; double soc_min;
        double System_p_max; double System_p_min; double System_q_max; double System_q_min;
        double f_arr[6], p_arr[6], v_arr[6], q_arr[6]; double PQ_p_ref; double PQ_q_ref;
        int FP_type; double FP_soc_goal_percent; double FP_main_f_set; double FP_line_p_base;
        double FP_soc_dead_band; double FP_soc_adjust; int VC_type; double Vq_v_base; double Vq_q_base;
        double Vq_droop; double Vq_v_set; double PF_pf_ref;
        std::string pcs_ip; int pcs_port; int pcs_id;
        std::string HM_ip; int HM_port; int HM_id;
    signals:
        void AFCUpdate(double, int);
    private:
      dllgc *thread_AFC;
};
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    AFCBackground *AFC_class;
    libconfig::Config cfg;
private slots:

    void on_pushButton_8_pressed();

    void on_pushButton_7_pressed();

    void on_pushButton_5_pressed();

    void on_pushButton_2_pressed();

    void on_toolButton_pressed();

    void on_pushButton_4_pressed();

    void on_toolButton_2_pressed();

    void on_toolButton_3_pressed();

    void on_toolButton_4_pressed();

    void on_lineEdit_editingFinished();

    void on_lineEdit_3_editingFinished();

    void on_lineEdit_2_editingFinished();

    void on_lineEdit_4_editingFinished();

    void on_lineEdit_5_editingFinished();

    void on_lineEdit_10_editingFinished();

    void on_lineEdit_11_editingFinished();

    void on_pushButton_46_pressed();

    void on_pushButton_47_pressed();

    void on_lineEdit_13_editingFinished();

    void on_lineEdit_14_editingFinished();

    void on_lineEdit_15_editingFinished();

    void on_lineEdit_16_editingFinished();

    void on_lineEdit_17_editingFinished();

    void on_lineEdit_18_editingFinished();

    void on_lineEdit_19_editingFinished();

    void on_lineEdit_20_editingFinished();

    void on_lineEdit_21_editingFinished();

    void on_lineEdit_22_editingFinished();

    void on_lineEdit_23_editingFinished();

    void on_lineEdit_24_editingFinished();

    void on_lineEdit_25_editingFinished();

    void on_lineEdit_27_editingFinished();

    void on_lineEdit_28_editingFinished();

    void on_lineEdit_29_editingFinished();

    void on_lineEdit_30_editingFinished();

    void on_lineEdit_31_editingFinished();

    void on_lineEdit_32_editingFinished();

    void on_lineEdit_33_editingFinished();

    void on_lineEdit_34_editingFinished();

    void on_lineEdit_35_editingFinished();

    void on_lineEdit_36_editingFinished();

    void on_lineEdit_37_editingFinished();

    void on_lineEdit_38_editingFinished();

    void on_lineEdit_39_editingFinished();

    void on_lineEdit_40_editingFinished();

    void on_lineEdit_41_editingFinished();

    void on_lineEdit_42_editingFinished();

    void on_lineEdit_60_editingFinished();

    void on_lineEdit_61_editingFinished();

    void on_lineEdit_62_editingFinished();

    void on_lineEdit_63_editingFinished();

    void on_lineEdit_64_editingFinished();

    void on_lineEdit_65_editingFinished();

    void on_pushButton_48_pressed();

    void on_pushButton_51_pressed();

    void on_pushButton_52_pressed();

    void on_pushButton_53_pressed();

    void on_pushButton_54_pressed();

    void on_pushButton_55_pressed();

    void on_pushButton_56_pressed();

    void on_pushButton_57_pressed();

    void on_pushButton_58_pressed();

    void on_pushButton_59_pressed();

    void on_pushButton_60_pressed();

    void on_pushButton_61_pressed();

    void on_pushButton_62_pressed();

    void on_pushButton_63_pressed();

    void on_pushButton_11_pressed();

    void on_pushButton_66_pressed();

    void on_pushButton_67_pressed();

    void on_pushButton_68_pressed();

    void on_pushButton_69_pressed();

private:
    Ui::MainWindow *ui;
    EMSLink *ems_win;
    PCSLink *pcs_win;
    MBMSLink *mbms_win;
    MeterLink *HM_win;
    QTimer *timSend;
    QStandardItemModel *model;
    QStandardItemModel *model2;
    dllSysTest *thread_mongo_status;
public slots:
    void EMSreshow(QString,QString,QString,QString,QString,QString,QString);
    void PCSreshow(QString,QString,QString,QString);
    void PCScontrol(double,double,double);
    void MBMSreshow(QString);
    void HMreshow(QString,QString,QString,QString);
    void onAFCReturn(double, int);
    void MBMSdataupdate_loop();
signals:
    void EMSdataupdate_signals(double *);
    void MBMSdataupdate_signals(double *);
    void HMdataupdate_signals(double *);
    void PCSdataupdate_signals(double *);

};
#endif // MAINWINDOW_H
