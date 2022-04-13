#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "emslink.h"
#include "pcslink.h"
#include "mbmslink.h"
#include "meterlink.h"
#include <dllgc.h>
#include <libconfig.h++>

bool remote_flag=false;
bool running_button_flag=false;
bool running_dont_start_flag=true;

std::string ems_redundant;
std::string ems_ip1;
std::string ems_port1;
std::string ems_ip2;
std::string ems_port2;
std::string user;
std::string pwd;

std::string ability_test = "None";

std::string set_source;
std::string set_model;
std::string set_model_index;
std::string set_p;
std::string set_p_index;
std::string set_q;
std::string set_q_index;


std::string pcs_ip;
std::string pcs_port;
std::string pcs_id;

std::string HM_eid;
std::string PCS_eid;
std::string mbms_eid;

std::string HM_ip;
std::string HM_port;
std::string HM_id;

std::string pq_p;
std::string pq_q;

std::string FP_line_p_base;
std::string afc_f1;
std::string afc_f2;
std::string afc_f3;
std::string afc_f4;
std::string afc_f5;
std::string afc_f6;
std::string afc_p1;
std::string afc_p2;
std::string afc_p3;
std::string afc_p4;
std::string afc_p5;
std::string afc_p6;
std::string Vq_q_base;
std::string Vq_v_base;
std::string afc_v1;
std::string afc_v2;
std::string afc_v3;
std::string afc_v4;
std::string afc_v5;
std::string afc_v6;
std::string afc_q1;
std::string afc_q2;
std::string afc_q3;
std::string afc_q4;
std::string afc_q5;
std::string afc_q6;
std::string PQ_q_ref;
std::string PF_pf_ref;
std::string soc_max;
std::string soc_min;
std::string System_p_max;
std::string System_p_min;
std::string System_q_max;
std::string System_q_min;

double pcs_connectc = 0;
double pcs_resetc = 0;
double pcs_control_time_localc = 0;
double emsdata[48]={0};
double mbmsdata[1]={0};
double HMdata[41]={0};
double pcsdata[44]={0};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //頁面初始化
    ui->comboBox->setEditable(true);
    ui->comboBox->insertItem(0, "STOP mode","STOP mode");
    ui->comboBox->insertItem(1, "PQ model","PQ model");
    ui->comboBox->insertItem(2, "AFC","AFC");
    ui->comboBox->insertItem(3, "Smooth","Smooth");


    ui->comboBox_2->setEditable(true);
    ui->comboBox_2->insertItem(0, "dReg0.5","dReg0.5");
    ui->comboBox_2->insertItem(1, "dReg0.25","dReg0.25");
    ui->comboBox_2->insertItem(2, "sReg","sReg");
    ui->comboBox_2->insertItem(3, "EdReg","EdReg");
    ui->comboBox_2->insertItem(4, "Custom","Custom");
    ui->comboBox_2->insertItem(5, "None","None");

    ui->comboBox_3->setEditable(true);
    ui->comboBox_3->insertItem(0, "None","None");
    ui->comboBox_3->insertItem(1, "Auto","Auto");
    ui->comboBox_3->insertItem(2, "custom","custom");
    ui->comboBox_3->insertItem(3, "constant Q","constant Q");
    ui->comboBox_3->insertItem(4, "constant PF","constant PF");

    ui->comboBox_5->setEditable(true);
    ui->comboBox_5->insertItem(0, "None","None");
    ui->comboBox_5->insertItem(1, "dReg0.25","dReg0.25");
    ui->comboBox_5->insertItem(2, "dReg0.5","dReg0.5");
    ui->comboBox_5->insertItem(3, "sReg","sReg");

    model = new QStandardItemModel(0,3,this);
    model->setHorizontalHeaderItem(0, new QStandardItem(QString("ID")));
    model->setHorizontalHeaderItem(1, new QStandardItem(QString("time")));
    model->setHorizontalHeaderItem(2, new QStandardItem(QString("event")));
    model->setHorizontalHeaderItem(3, new QStandardItem(QString("group")));
    model->setHorizontalHeaderItem(4, new QStandardItem(QString("level")));
    model->setHorizontalHeaderItem(5, new QStandardItem(QString("show")));
    ui->tableView->setModel(model);
    ui->tableView->setColumnWidth(0, 130);
    ui->tableView->setColumnWidth(1, 200);
    ui->tableView->setColumnWidth(2, 200);
    ui->tableView->setColumnWidth(3, 150);
    ui->tableView->setColumnWidth(4, 150);
    ui->tableView->setColumnWidth(5, 150);

    model2 = new QStandardItemModel(0,3,this);
    model2->setHorizontalHeaderItem(0, new QStandardItem(QString("ID")));
    model2->setHorizontalHeaderItem(1, new QStandardItem(QString("time")));
    model2->setHorizontalHeaderItem(2, new QStandardItem(QString("event")));
    model2->setHorizontalHeaderItem(3, new QStandardItem(QString("group")));
    model2->setHorizontalHeaderItem(4, new QStandardItem(QString("level")));
    model2->setHorizontalHeaderItem(5, new QStandardItem(QString("show")));
    ui->tableView_2->setModel(model2);
    ui->tableView_2->setColumnWidth(0, 130);
    ui->tableView_2->setColumnWidth(1, 200);
    ui->tableView_2->setColumnWidth(2, 200);
    ui->tableView_2->setColumnWidth(3, 150);
    ui->tableView_2->setColumnWidth(4, 150);
    ui->tableView_2->setColumnWidth(5, 150);

    thread_mongo_status = new dllSysTest();
    try{
        try{
            //讀取資訊cfg
            cfg.readFile("AFC_init.cfg");
            qDebug() << "AFC_init.cfg runing";
            libconfig::Setting &root = cfg.getRoot();
            libconfig::Setting &init= root["init"][0];
            libconfig::Setting &model_AFC_run= root["model_AFC_run"][0];

            init.lookupValue("ems_redundant",ems_redundant);
            if (ems_redundant == "0"){
                ui->lineEdit_2->setDisabled(true);
            }else if (ems_redundant == "2"){
                ui->lineEdit_2->setDisabled(false);
            }
            init.lookupValue("ems_port1",ems_port1);
            init.lookupValue("ems_port2",ems_port2);
            init.lookupValue("user",user);
            init.lookupValue("pwd",pwd);

            init.lookupValue("pcs_port",pcs_port);
            init.lookupValue("pcs_id",pcs_id);
            init.lookupValue("PCS_eid",PCS_eid);

            init.lookupValue("HM_port",HM_port);
            init.lookupValue("HM_id",HM_id);
            init.lookupValue("HM_eid",HM_eid);

            try{
                init.lookupValue("set_source",set_source);
                ui->lineEdit_7->setText(QString::fromStdString(set_source));
                if (set_source=="Remote"){//Remote
                    ui->checkBox_6->setCheckState(Qt::Checked);
                }
                else{//Local
                    ui->checkBox_6->setCheckState(Qt::Unchecked);
                }

            }
            catch(...){}
            try{
                init.lookupValue("set_model",set_model);
                ui->lineEdit_8->setText(QString::fromStdString(set_model));
                init.lookupValue("set_model_index",set_model_index);
                ui->stackedWidget->setCurrentIndex(std::stoi(set_model_index));
                ui->comboBox->setCurrentIndex(std::stoi(set_model_index));
            }
            catch(...){
                ui->stackedWidget->setCurrentIndex(0);
                ui->comboBox->setCurrentIndex(0);
            }
            try{
                init.lookupValue("set_p",set_p);
                ui->lineEdit_12->setText(QString::fromStdString(set_p));
                init.lookupValue("set_p_index",set_p_index);
                ui->stackedWidget_2->setCurrentIndex(0);//固定
                ui->comboBox_2->setCurrentIndex(std::stoi(set_p_index));
            }
            catch(...){
                ui->stackedWidget_2->setCurrentIndex(0);
                ui->comboBox_2->setCurrentIndex(0);
            }
            try{
                init.lookupValue("set_q",set_q);
                ui->lineEdit_26->setText(QString::fromStdString(set_q));
                init.lookupValue("set_q_index",set_q_index);
                ui->stackedWidget_3->setCurrentIndex(std::stoi(set_q_index));
                ui->comboBox_3->setCurrentIndex(std::stoi(set_q_index));
            }
            catch(...){
                ui->stackedWidget_3->setCurrentIndex(0);
                ui->comboBox_3->setCurrentIndex(0);
            }
            init.lookupValue("ems_ip1",ems_ip1);
            ui->lineEdit->setText(QString::fromStdString(ems_ip1));
            init.lookupValue("ems_ip2",ems_ip2);
            ui->lineEdit_2->setText(QString::fromStdString(ems_ip2));
            init.lookupValue("pcs_ip",pcs_ip);
            ui->lineEdit_3->setText(QString::fromStdString(pcs_ip));
            init.lookupValue("mbms_eid",mbms_eid);
            ui->lineEdit_4->setText(QString::fromStdString(mbms_eid));
            init.lookupValue("HM_ip",HM_ip);
            ui->lineEdit_5->setText(QString::fromStdString(HM_ip));
            init.lookupValue("soc_max",soc_max);
            ui->lineEdit_60->setText(QString::fromStdString(soc_max));
            init.lookupValue("soc_min",soc_min);
            ui->lineEdit_61->setText(QString::fromStdString(soc_min));
            init.lookupValue("System_p_max",System_p_max);
            ui->lineEdit_62->setText(QString::fromStdString(System_p_max));
            init.lookupValue("System_p_min",System_p_min);
            ui->lineEdit_63->setText(QString::fromStdString(System_p_min));
            init.lookupValue("System_q_max",System_q_max);
            ui->lineEdit_64->setText(QString::fromStdString(System_q_max));
            init.lookupValue("System_q_min",System_q_min);
            ui->lineEdit_65->setText(QString::fromStdString(System_q_min));

            model_AFC_run .lookupValue("pq_p",pq_p);
            ui->lineEdit_10->setText(QString::fromStdString(pq_p));
            model_AFC_run .lookupValue("pq_q",pq_q);
            ui->lineEdit_11->setText(QString::fromStdString(pq_q));
            model_AFC_run.lookupValue("FP_line_p_base",FP_line_p_base);
            ui->lineEdit_13->setText(QString::fromStdString(FP_line_p_base));
            model_AFC_run.lookupValue("afc_f1",afc_f1);
            ui->lineEdit_14->setText(QString::fromStdString(afc_f1));
            model_AFC_run.lookupValue("afc_f2",afc_f2);
            ui->lineEdit_15->setText(QString::fromStdString(afc_f2));
            model_AFC_run.lookupValue("afc_f3",afc_f3);
            ui->lineEdit_16->setText(QString::fromStdString(afc_f3));
            model_AFC_run.lookupValue("afc_f4",afc_f4);
            ui->lineEdit_17->setText(QString::fromStdString(afc_f4));
            model_AFC_run.lookupValue("afc_f5",afc_f5);
            ui->lineEdit_18->setText(QString::fromStdString(afc_f5));
            model_AFC_run.lookupValue("afc_f6",afc_f6);
            ui->lineEdit_19->setText(QString::fromStdString(afc_f6));
            model_AFC_run.lookupValue("afc_p1",afc_p1);
            ui->lineEdit_20->setText(QString::fromStdString(afc_p1));
            model_AFC_run.lookupValue("afc_p2",afc_p2);
            ui->lineEdit_21->setText(QString::fromStdString(afc_p2));
            model_AFC_run.lookupValue("afc_p3",afc_p3);
            ui->lineEdit_22->setText(QString::fromStdString(afc_p3));
            model_AFC_run.lookupValue("afc_p4",afc_p4);
            ui->lineEdit_23->setText(QString::fromStdString(afc_p4));
            model_AFC_run.lookupValue("afc_p5",afc_p5);
            ui->lineEdit_24->setText(QString::fromStdString(afc_p5));
            model_AFC_run.lookupValue("afc_p6",afc_p6);
            ui->lineEdit_25->setText(QString::fromStdString(afc_p6));
            model_AFC_run.lookupValue("Vq_q_base",Vq_q_base);
            ui->lineEdit_27->setText(QString::fromStdString(Vq_q_base));
            model_AFC_run.lookupValue("Vq_v_base",Vq_v_base);
            ui->lineEdit_28->setText(QString::fromStdString(Vq_v_base));
            model_AFC_run.lookupValue("afc_v1",afc_v1);
            ui->lineEdit_29->setText(QString::fromStdString(afc_v1));
            model_AFC_run.lookupValue("afc_v2",afc_v2);
            ui->lineEdit_30->setText(QString::fromStdString(afc_v2));
            model_AFC_run.lookupValue("afc_v3",afc_v3);
            ui->lineEdit_31->setText(QString::fromStdString(afc_v3));
            model_AFC_run.lookupValue("afc_v4",afc_v4);
            ui->lineEdit_32->setText(QString::fromStdString(afc_v4));
            model_AFC_run.lookupValue("afc_v5",afc_v5);
            ui->lineEdit_33->setText(QString::fromStdString(afc_v5));
            model_AFC_run.lookupValue("afc_v6",afc_v6);
            ui->lineEdit_34->setText(QString::fromStdString(afc_v6));
            model_AFC_run.lookupValue("afc_q1",afc_q1);
            ui->lineEdit_35->setText(QString::fromStdString(afc_q1));
            model_AFC_run.lookupValue("afc_q2",afc_q2);
            ui->lineEdit_36->setText(QString::fromStdString(afc_q2));
            model_AFC_run.lookupValue("afc_q3",afc_q3);
            ui->lineEdit_37->setText(QString::fromStdString(afc_q3));
            model_AFC_run.lookupValue("afc_q4",afc_q4);
            ui->lineEdit_38->setText(QString::fromStdString(afc_q4));
            model_AFC_run.lookupValue("afc_q5",afc_q5);
            ui->lineEdit_39->setText(QString::fromStdString(afc_q5));
            model_AFC_run.lookupValue("afc_q6",afc_q6);
            ui->lineEdit_40->setText(QString::fromStdString(afc_q6));
            model_AFC_run.lookupValue("PQ_q_ref",PQ_q_ref);
            ui->lineEdit_41->setText(QString::fromStdString(PQ_q_ref));
            model_AFC_run.lookupValue("PF_pf_ref",PF_pf_ref);
            ui->lineEdit_42->setText(QString::fromStdString(PF_pf_ref));

            qDebug() << "AFC_init.cfg read end";
        }
        catch(...){qDebug() << "AFC_init.cfg error";}
        try{
            //AFC_class
            AFC_class = new AFCBackground(this);
            AFC_class -> name = "AFC C++ run";
            connect(AFC_class,SIGNAL(AFCUpdate(double,int)),this,SLOT(ondAFCReturn(double,int)));
            //啟動C++
            if (ui->lineEdit_7->text() == "Remote"){
                qDebug() << "AFC_class up runing";
                AFC_class->Stop=false;
                AFC_class->user = user;
                AFC_class->pwd = pwd;
                AFC_class->IP1 = ems_ip1;
                AFC_class->mongo_port1 = std::stoi(ems_port1);
                AFC_class->IP2 = ems_ip2;
                AFC_class->mongo_port2 = std::stoi(ems_port2);

                AFC_class->set_source = set_source;
                AFC_class->HM_eid = HM_eid;
                AFC_class->PCS_eid = PCS_eid;
                AFC_class->MBMS_eid = mbms_eid;
                AFC_class->pcs_ip = pcs_ip;
                AFC_class->pcs_port = std::stoi(pcs_port);
                AFC_class->pcs_id = std::stoi(pcs_id);
                AFC_class->HM_ip = HM_ip;
                AFC_class->HM_port = std::stoi(HM_port);
                AFC_class->HM_id = std::stoi(HM_id);


                AFC_class->start();
            }
        }
        catch(...){}
    }
    catch(...){
        qDebug() << "unknow error";
    }
    //跳頁面(設定)
    ems_win = new EMSLink();
    pcs_win = new PCSLink();
    mbms_win = new MBMSLink();
    HM_win = new MeterLink();
    //頁面資料傳輸
    connect(ems_win, &EMSLink::send_data, this, &MainWindow::EMSreshow);
    connect(pcs_win, &PCSLink::send_data, this, &MainWindow::PCSreshow);
    connect(pcs_win, &PCSLink::send_control, this, &MainWindow::PCScontrol);
    connect(mbms_win, &MBMSLink::send_data, this, &MainWindow::MBMSreshow);
    connect(HM_win, &MeterLink::send_data, this, &MainWindow::HMreshow);

    timSend = new QTimer(this);
    timSend->setInterval(1000);
    timSend->start();
    connect(timSend, &QTimer::timeout, this, &MainWindow::MBMSdataupdate_loop);
    connect(this, &MainWindow::EMSdataupdate_signals, ems_win, &EMSLink::EMSdataupdate_slots);
    connect(this, &MainWindow::MBMSdataupdate_signals, mbms_win, &MBMSLink::MBMSdataupdate_slots);
    connect(this, &MainWindow::HMdataupdate_signals, HM_win, &MeterLink::Meterdataupdate_slots);
    connect(this, &MainWindow::PCSdataupdate_signals, pcs_win, &PCSLink::PCSdataupdate_slots);

}
MainWindow::~MainWindow()
{
    timSend->stop();
    delete thread_mongo_status;
    delete timSend;
    delete AFC_class;
    delete ems_win;
    delete pcs_win;
    delete mbms_win;
    delete HM_win;
    delete ui;
}
void MainWindow::EMSreshow(QString ems_redundantb, QString ems_ip1b, QString ems_port1b, QString ems_ip2b, QString ems_port2b, QString userb, QString pwdb)
{
    libconfig::Setting &root = cfg.getRoot();
    libconfig::Setting &init =root["init"][0];
    libconfig::Setting &ems_redundanta =init.lookup("ems_redundant");
    ems_redundanta = ems_redundantb.toStdString();
    ems_redundant = ems_redundantb.toStdString();
    if (ems_redundant == "0"){
        ui->lineEdit_2->setDisabled(true);
    }else if (ems_redundant == "2"){
        ui->lineEdit_2->setDisabled(false);
    }
    libconfig::Setting &ems_ip1a =init.lookup("ems_ip1");
    ems_ip1a = ems_ip1b.toStdString();
    ems_ip1 = ems_ip1b.toStdString();
    libconfig::Setting &ems_port1a =init.lookup("ems_port1");
    ems_port1a = ems_port1b.toStdString();
    ems_port1 = ems_port1b.toStdString();
    libconfig::Setting &ems_ip2a =init.lookup("ems_ip2");
    ems_ip2a = ems_ip2b.toStdString();
    ems_ip2 = ems_ip2b.toStdString();
    libconfig::Setting &ems_port2a =init.lookup("ems_port2");
    ems_port2a = ems_port2b.toStdString();
    ems_port2 = ems_port2b.toStdString();
    libconfig::Setting &usera =init.lookup("user");
    usera = userb.toStdString();
    user = userb.toStdString();
    libconfig::Setting &pwda =init.lookup("pwd");
    pwda = pwdb.toStdString();
    pwd = pwdb.toStdString();
    cfg.writeFile ("AFC_init.cfg");

    ui->lineEdit->setText(ems_ip1b);
    ui->lineEdit_2->setText(ems_ip2b);
    //this->close();
}
void MainWindow::PCSreshow(QString pcs_ipb, QString pcs_portb, QString pcs_idb, QString PCS_eidb)
{
    libconfig::Setting &root = cfg.getRoot();
    libconfig::Setting &init =root["init"][0];
    libconfig::Setting &pcs_ipa =init.lookup("pcs_ip");
    pcs_ipa = pcs_ipb.toStdString();
    pcs_ip = pcs_ipb.toStdString();
    libconfig::Setting &pcs_porta =init.lookup("pcs_port");
    pcs_porta = pcs_portb.toStdString();
    pcs_port = pcs_portb.toStdString();
    libconfig::Setting &pcs_ida =init.lookup("pcs_id");
    pcs_ida = pcs_idb.toStdString();
    pcs_id = pcs_idb.toStdString();
    libconfig::Setting &PCS_eida =init.lookup("PCS_eid");
    PCS_eida = PCS_eidb.toStdString();
    PCS_eid = PCS_eidb.toStdString();
    cfg.writeFile ("AFC_init.cfg");
    ui->lineEdit_3->setText(pcs_ipb);
}
void MainWindow::PCScontrol(double pcs_connectb, double pcs_resetb, double pcs_control_time_localb)
{
    pcs_connectc = pcs_connectb;
    pcs_resetc = pcs_resetb;
    pcs_control_time_localc = pcs_control_time_localb;
//    qDebug()<<pcs_connectc;
//    qDebug()<<pcs_resetc;
}
void MainWindow::MBMSreshow(QString mbms_eidb)
{
    libconfig::Setting &root = cfg.getRoot();
    libconfig::Setting &init =root["init"][0];
    libconfig::Setting &mbms_eida =init.lookup("mbms_eid");
    mbms_eida = mbms_eidb.toStdString();
    mbms_eid = mbms_eidb.toStdString();
    cfg.writeFile ("AFC_init.cfg");
    ui->lineEdit_4->setText(mbms_eidb);
}
void MainWindow::MBMSdataupdate_loop()
{
    emit EMSdataupdate_signals(emsdata);
    emit MBMSdataupdate_signals(mbmsdata);
    emit HMdataupdate_signals(HMdata);
    emit PCSdataupdate_signals(pcsdata);

}
void MainWindow::HMreshow(QString HM_ipb, QString HM_portb, QString HM_idb, QString HM_eidb)
{
    libconfig::Setting &root = cfg.getRoot();
    libconfig::Setting &init =root["init"][0];
    libconfig::Setting &HM_ipa =init.lookup("HM_ip");
    HM_ipa = HM_ipb.toStdString();
    HM_ip = HM_ipb.toStdString();
    libconfig::Setting &HM_porta =init.lookup("HM_port");
    HM_porta = HM_portb.toStdString();
    HM_port = HM_portb.toStdString();
    libconfig::Setting &HM_ida =init.lookup("HM_id");
    HM_ida = HM_idb.toStdString();
    HM_id = HM_idb.toStdString();
    libconfig::Setting &HM_eida =init.lookup("HM_eid");
    HM_eida = HM_eidb.toStdString();
    HM_eid = HM_eidb.toStdString();
    cfg.writeFile ("AFC_init.cfg");

    ui->lineEdit_5->setText(HM_ipb);
}
AFCBackground::AFCBackground(QObject *parent):
    QThread(parent){
    thread_AFC = new dllgc();
}
AFCBackground::~AFCBackground(){
    delete thread_AFC;
}
void AFCBackground::run(){
    thread_AFC->ems_redundant = ems_redundant;
    thread_AFC->user = user;
    thread_AFC->pwd = pwd;
    thread_AFC->IP1 = IP1;
    thread_AFC->mongo_port1 = mongo_port1;
    thread_AFC->IP2 = IP2;
    thread_AFC->mongo_port2 = mongo_port2;
    bool stopflag=false;
    double soc_now =0;
    int error_code=0;
    qDebug() << name << "runing";
    pcs_connectc = 0;
    pcs_resetc = 0;
    pcs_control_time_localc = 0;
    std::thread t1(&dllgc::AFC_run,thread_AFC,
                   std::ref(stopflag),std::ref(error_code),std::ref(ability_test),
                   std::ref(pcs_connectc),std::ref(pcs_resetc),std::ref(pcs_control_time_localc),
                   std::ref(emsdata),std::ref(mbmsdata),std::ref(HMdata),std::ref(pcsdata),
                   MBMS_eid,HM_eid,PCS_eid,set_source,
                   mode,scale,soc_max,soc_min,
                   System_p_max,System_p_min,System_q_max,System_q_min,
                   f_arr,p_arr,v_arr,q_arr,PQ_p_ref,PQ_q_ref,
                   FP_type,FP_soc_goal_percent,FP_main_f_set,FP_line_p_base,
                   FP_soc_dead_band,FP_soc_adjust,VC_type,Vq_v_base,Vq_q_base,
                   Vq_droop,Vq_v_set,PF_pf_ref,
                   pcs_ip,pcs_port,pcs_id,
                   HM_ip,HM_port,HM_id);
    this->msleep(1000);
    while (true){
        QMutex mutex;
//        qDebug() << name << "runing";
        mutex.lock();
        if (this->Stop){
            qDebug() << name << "stop";
            stopflag=true;
            t1.join();
            mutex.unlock();
            break;
        }
        mutex.unlock();
        emit AFCUpdate(soc_now,error_code);
        this->msleep(1000);
    }
    running_dont_start_flag=true;
    qDebug() << name << "end";


}
void MainWindow::onAFCReturn(double soc_now, int error_code){
}


void MainWindow::on_pushButton_8_pressed()
{
    //模式 切換
    if (running_button_flag!=true){
        libconfig::Setting &root = cfg.getRoot();
        libconfig::Setting &init= root["init"][0];
        libconfig::Setting &set_modela =init.lookup("set_model");
        set_modela = ui->comboBox->currentData().toString().toStdString();
        ui->lineEdit_8->setText(ui->comboBox->currentData().toString());

        libconfig::Setting &set_model_indexa =init.lookup("set_model_index");
        set_model_indexa = std::to_string(ui->comboBox->currentIndex());
        ui->stackedWidget->setCurrentIndex(ui->comboBox->currentIndex());
        cfg.writeFile ("AFC_init.cfg");
    }
    else{
        //執行終止允許切換為STOP模式
        if (ui->comboBox->currentData().toString()=="停機(STOP)"){
            ui->lineEdit_8->setText(ui->comboBox->currentData().toString());
            ui->stackedWidget->setCurrentIndex(ui->comboBox->currentIndex());
        }
        else{
            ui->textEdit->insertPlainText("執行中請勿操作設定\n");
        }
    }
}

void MainWindow::on_pushButton_46_pressed()
{
    //AFC頻率實功 設定
    if (running_button_flag!=true){
        libconfig::Setting &root = cfg.getRoot();
        libconfig::Setting &init= root["init"][0];
        libconfig::Setting &set_pa =init.lookup("set_p");
        set_pa = ui->comboBox_2->currentData().toString().toStdString();
        ui->lineEdit_12->setText(ui->comboBox_2->currentData().toString());

        libconfig::Setting &set_p_indexa =init.lookup("set_p_index");
        set_p_indexa = std::to_string(ui->comboBox_2->currentIndex());
        ui->stackedWidget_2->setCurrentIndex(0);//固定
        if (ui->comboBox_2->currentData()=="dReg0.5"){
            ui->lineEdit_14->setText("59.50");
            ui->lineEdit_14->setDisabled(true);
            ui->lineEdit_15->setText("59.75");
            ui->lineEdit_15->setDisabled(true);
            ui->lineEdit_16->setText("59.98");
            ui->lineEdit_16->setDisabled(true);
            ui->lineEdit_17->setText("60.02");
            ui->lineEdit_17->setDisabled(true);
            ui->lineEdit_18->setText("60.25");
            ui->lineEdit_18->setDisabled(true);
            ui->lineEdit_19->setText("60.50");
            ui->lineEdit_19->setDisabled(true);

            ui->lineEdit_20->setText("100");
            ui->lineEdit_20->setDisabled(true);
            ui->lineEdit_21->setText("48");
            ui->lineEdit_21->setDisabled(true);
            ui->lineEdit_22->setText("0");
            ui->lineEdit_22->setDisabled(true);
            ui->lineEdit_23->setText("0");
            ui->lineEdit_23->setDisabled(true);
            ui->lineEdit_24->setText("-48");
            ui->lineEdit_24->setDisabled(true);
            ui->lineEdit_25->setText("-100");
            ui->lineEdit_25->setDisabled(true);
        }
        else if (ui->comboBox_2->currentData()=="dReg0.25"){
            ui->lineEdit_14->setText("59.75");
            ui->lineEdit_14->setDisabled(true);
            ui->lineEdit_15->setText("59.86");
            ui->lineEdit_15->setDisabled(true);
            ui->lineEdit_16->setText("59.98");
            ui->lineEdit_16->setDisabled(true);
            ui->lineEdit_17->setText("60.02");
            ui->lineEdit_17->setDisabled(true);
            ui->lineEdit_18->setText("60.14");
            ui->lineEdit_18->setDisabled(true);
            ui->lineEdit_19->setText("60.25");
            ui->lineEdit_19->setDisabled(true);

            ui->lineEdit_20->setText("100");
            ui->lineEdit_20->setDisabled(true);
            ui->lineEdit_21->setText("52");
            ui->lineEdit_21->setDisabled(true);
            ui->lineEdit_22->setText("0");
            ui->lineEdit_22->setDisabled(true);
            ui->lineEdit_23->setText("0");
            ui->lineEdit_23->setDisabled(true);
            ui->lineEdit_24->setText("-52");
            ui->lineEdit_24->setDisabled(true);
            ui->lineEdit_25->setText("-100");
            ui->lineEdit_25->setDisabled(true);
        }
        else if (ui->comboBox_2->currentData()=="sReg"){
            ui->lineEdit_14->setText("59.88");
            ui->lineEdit_14->setDisabled(true);
            ui->lineEdit_15->setText("59.98");
            ui->lineEdit_15->setDisabled(true);
            ui->lineEdit_16->setText("60.00");
            ui->lineEdit_16->setDisabled(true);
            ui->lineEdit_17->setText("60.25");
            ui->lineEdit_17->setDisabled(true);
            ui->lineEdit_18->setText(" ");
            ui->lineEdit_18->setDisabled(true);
            ui->lineEdit_19->setText(" ");
            ui->lineEdit_19->setDisabled(true);

            ui->lineEdit_20->setText("100");
            ui->lineEdit_20->setDisabled(true);
            ui->lineEdit_21->setText("0");
            ui->lineEdit_21->setDisabled(true);
            ui->lineEdit_22->setText("-9");
            ui->lineEdit_22->setDisabled(true);
            ui->lineEdit_23->setText("-100");
            ui->lineEdit_23->setDisabled(true);
            ui->lineEdit_24->setText(" ");
            ui->lineEdit_24->setDisabled(true);
            ui->lineEdit_25->setText(" ");
            ui->lineEdit_25->setDisabled(true);
        }
        else if (ui->comboBox_2->currentData()=="自訂"){
            ui->lineEdit_14->setDisabled(false);
            ui->lineEdit_15->setDisabled(false);
            ui->lineEdit_16->setDisabled(false);
            ui->lineEdit_17->setDisabled(false);
            ui->lineEdit_18->setDisabled(false);
            ui->lineEdit_19->setDisabled(false);

            ui->lineEdit_20->setDisabled(false);
            ui->lineEdit_21->setDisabled(false);
            ui->lineEdit_22->setDisabled(false);
            ui->lineEdit_23->setDisabled(false);
            ui->lineEdit_24->setDisabled(false);
            ui->lineEdit_25->setDisabled(false);
        }
        cfg.writeFile ("AFC_init.cfg");

    }
    else{
        ui->textEdit->insertPlainText("執行中請勿操作設定\n");
    }
}

void MainWindow::on_pushButton_47_pressed()
{
    //AFC電壓虛功 設定
    if (running_button_flag!=true){
        libconfig::Setting &root = cfg.getRoot();
        libconfig::Setting &init= root["init"][0];
        libconfig::Setting &set_qa =init.lookup("set_q");
        set_qa = ui->comboBox_3->currentData().toString().toStdString();
        ui->lineEdit_26->setText(ui->comboBox_3->currentData().toString());
        libconfig::Setting &set_q_indexa =init.lookup("set_q_index");
        set_q_indexa = std::to_string(ui->comboBox_3->currentIndex());
        ui->stackedWidget_3->setCurrentIndex(ui->comboBox_3->currentIndex());
        cfg.writeFile ("AFC_init.cfg");
    }
    else{
        ui->textEdit->insertPlainText("執行中請勿操作設定\n");
    }
}
void MainWindow::on_pushButton_48_pressed()
{
    //能力測試 設定
    ui->lineEdit_6->setText(ui->comboBox_5->currentData().toString());
    ui->stackedWidget_5->setCurrentIndex(ui->comboBox_5->currentIndex());
    if (ui->comboBox_5->currentData().toString() == "None"){
        ui->lineEdit_50->setText("None");
    }
    else{
        ui->lineEdit_50->setText("please press the button");
    }
}
void MainWindow::on_pushButton_7_pressed()
{
    //Remote Local 切換
    if (running_button_flag!=true){
        libconfig::Setting &root = cfg.getRoot();
        libconfig::Setting &init= root["init"][0];
        libconfig::Setting &set_sourcea =init.lookup("set_source");
        if (ui->checkBox_6->isChecked()){
            ui->lineEdit_7->setText("Remote");
            set_sourcea = "Remote";
            set_source = "Remote";
        }
        else{
            ui->lineEdit_7->setText("Local");
            set_sourcea = "Local";
            set_source = "Local";
        }
        cfg.writeFile ("AFC_init.cfg");
    }
    else{
        ui->textEdit->insertPlainText("執行中請勿操作設定\n");
    }
}
void MainWindow::on_pushButton_5_pressed()
{
    //執行按鈕Run
    if (running_button_flag==false and running_dont_start_flag==true){
        ui->textEdit->insertPlainText("系統啟動\n");
        running_button_flag=true;
        running_dont_start_flag=false;

        qDebug() << "AFC_class up runing";
        AFC_class->Stop=false;
        AFC_class->ems_redundant = ems_redundant;
        AFC_class->user = user;
        AFC_class->pwd = pwd;
        AFC_class->IP1 = ems_ip1;
        AFC_class->mongo_port1 = std::stoi(ems_port1);
        AFC_class->IP2 = ems_ip2;
        AFC_class->mongo_port2 = std::stoi(ems_port2);

        AFC_class->set_source = set_source;
        AFC_class->HM_eid = HM_eid;
        AFC_class->PCS_eid = PCS_eid;
        AFC_class->MBMS_eid = mbms_eid;
        AFC_class->pcs_ip = pcs_ip;
        AFC_class->pcs_port = std::stoi(pcs_port);
        AFC_class->pcs_id = std::stoi(pcs_id);
        AFC_class->HM_ip = HM_ip;
        AFC_class->HM_port = std::stoi(HM_port);
        AFC_class->HM_id = std::stoi(HM_id);
        if(ui->comboBox->currentData()=="STOP mode"){ //STOP
            AFC_class->mode = 0;
        }
        else if(ui->comboBox->currentData()=="PQ model"){ //PQ
            AFC_class->mode = 6;
        }else if(ui->comboBox->currentData()=="Smooth"){ //PQ
            AFC_class->mode = 3;
        }
        else if(ui->comboBox->currentData()=="AFC"){
            if (ui->comboBox_2->currentData()=="dReg0.5" or ui->comboBox_2->currentData()=="dReg0.25" or ui->comboBox_2->currentData()=="Custom" or ui->comboBox_2->currentData()=="EdReg"){
                AFC_class->mode = 5;
                AFC_class->FP_type = 2;
            }
            else if(ui->comboBox_2->currentData()=="sReg"){
                AFC_class->mode = 5;
                AFC_class->FP_type = 3;
            }
            else if(ui->comboBox_2->currentData()=="None"){ // VQ
                AFC_class->mode = 2;
            }
            if (ui->comboBox_3->currentData()=="None"){
                AFC_class->VC_type = 0;
            }
            else if (ui->comboBox_3->currentData()=="Auto"){
                AFC_class->VC_type = 1;
            }
            else if (ui->comboBox_3->currentData()=="constant Q"){
                AFC_class->VC_type = 3;
            }
            else if (ui->comboBox_3->currentData()=="constant PF"){
                AFC_class->VC_type = 4;
            }
            else if (ui->comboBox_3->currentData()=="custom"){
                AFC_class->VC_type = 2;
            }
        }
        AFC_class->FP_soc_goal_percent = ui->lineEdit_43->text().toDouble();
        AFC_class->FP_soc_dead_band = ui->lineEdit_44->text().toDouble();
        AFC_class->FP_soc_adjust = ui->lineEdit_45->text().toDouble();
        AFC_class->FP_line_p_base = ui->lineEdit_13->text().toDouble();
        AFC_class->Vq_q_base = ui->lineEdit_27->text().toDouble();
        AFC_class->Vq_v_base = ui->lineEdit_28->text().toDouble();
        AFC_class->PQ_q_ref = ui->lineEdit_41->text().toDouble();
        AFC_class->PF_pf_ref = ui->lineEdit_42->text().toDouble();
        AFC_class->f_arr[0] = ui->lineEdit_14->text().toDouble();
        AFC_class->f_arr[1] = ui->lineEdit_15->text().toDouble();
        AFC_class->f_arr[2] = ui->lineEdit_16->text().toDouble();
        AFC_class->f_arr[3] = ui->lineEdit_17->text().toDouble();
        AFC_class->f_arr[4] = ui->lineEdit_18->text().toDouble();
        AFC_class->f_arr[5] = ui->lineEdit_19->text().toDouble();

        AFC_class->p_arr[0] = ui->lineEdit_20->text().toDouble();
        AFC_class->p_arr[1] = ui->lineEdit_21->text().toDouble();
        AFC_class->p_arr[2] = ui->lineEdit_22->text().toDouble();
        AFC_class->p_arr[3] = ui->lineEdit_23->text().toDouble();
        AFC_class->p_arr[4] = ui->lineEdit_24->text().toDouble();
        AFC_class->p_arr[5] = ui->lineEdit_25->text().toDouble();

        AFC_class->v_arr[0] = ui->lineEdit_29->text().toDouble();
        AFC_class->v_arr[1] = ui->lineEdit_30->text().toDouble();
        AFC_class->v_arr[2] = ui->lineEdit_31->text().toDouble();
        AFC_class->v_arr[3] = ui->lineEdit_32->text().toDouble();
        AFC_class->v_arr[4] = ui->lineEdit_33->text().toDouble();
        AFC_class->v_arr[5] = ui->lineEdit_34->text().toDouble();

        AFC_class->q_arr[0] = ui->lineEdit_35->text().toDouble();
        AFC_class->q_arr[1] = ui->lineEdit_36->text().toDouble();
        AFC_class->q_arr[2] = ui->lineEdit_37->text().toDouble();
        AFC_class->q_arr[3] = ui->lineEdit_38->text().toDouble();
        AFC_class->q_arr[4] = ui->lineEdit_39->text().toDouble();
        AFC_class->q_arr[5] = ui->lineEdit_40->text().toDouble();
        AFC_class-> soc_max = ui->lineEdit_60->text().toDouble();
        AFC_class-> soc_min = ui->lineEdit_61->text().toDouble();
        AFC_class-> System_p_max = ui->lineEdit_62->text().toDouble();
        AFC_class-> System_p_min = ui->lineEdit_63->text().toDouble();
        AFC_class-> System_q_max = ui->lineEdit_64->text().toDouble();
        AFC_class-> System_q_min = ui->lineEdit_65->text().toDouble();
        AFC_class-> PQ_p_ref = ui->lineEdit_10->text().toDouble();
        AFC_class-> PQ_q_ref = ui->lineEdit_11->text().toDouble();


//        qDebug()  << "runing";
        AFC_class->start();
    }
    else if (running_button_flag == true){
        ui->textEdit->insertPlainText("程式執行中，請按停止\n");
    }
    else if (running_dont_start_flag == false){
        ui->textEdit->insertPlainText("程式執行中，等待停止後再試一次\n");
    }
    ui->lineEdit_9->setText(ui->lineEdit_8->text());
}
void MainWindow::on_pushButton_4_pressed()
{
    AFC_class->Stop =true;
    running_button_flag=false;
    ui->lineEdit_9->setText("停機(STOP)");
    ui->textEdit->insertPlainText("系統停機(停機中)\n");
}

void MainWindow::on_pushButton_2_pressed()
{
    ui->textEdit->clear();
}

void MainWindow::on_toolButton_pressed()
{
    ems_win->setModal(true);
    ems_win->setWindowTitle("EMS set");
    ems_win->setEMSData( QString::fromStdString(ems_redundant),QString::fromStdString(ems_ip1),QString::fromStdString(ems_port1),QString::fromStdString(ems_ip2),QString::fromStdString(ems_port2),QString::fromStdString(user),QString::fromStdString(pwd));
    ems_win->exec();
}
void MainWindow::on_toolButton_2_pressed()
{
    pcs_win->setModal(true);
    pcs_win->setWindowTitle("PCS set");
    pcs_win->setPCSData( QString::fromStdString(pcs_ip), QString::fromStdString(pcs_port), QString::fromStdString(pcs_id), QString::fromStdString(PCS_eid));
    pcs_win->exec();
}
void MainWindow::on_toolButton_3_pressed()
{
    mbms_win->setModal(true);
    mbms_win->setWindowTitle("MBMS set");
    mbms_win->setMBMSData( QString::fromStdString(mbms_eid) );
    mbms_win->exec();
}
void MainWindow::on_toolButton_4_pressed()
{
    HM_win->setModal(true);
    HM_win->setWindowTitle("High Meter set");
    HM_win->setMeterData( QString::fromStdString(HM_ip), QString::fromStdString(HM_port), QString::fromStdString(HM_id), QString::fromStdString(HM_eid));
    HM_win->exec();
}
void MainWindow::on_lineEdit_editingFinished()
{
    libconfig::Setting &root = cfg.getRoot();
    libconfig::Setting &init= root["init"][0];
    libconfig::Setting &ems_ip1a =init.lookup("ems_ip1");
    ems_ip1a = ui->lineEdit->text().toStdString();
    ems_ip1  = ui->lineEdit->text().toStdString();
    cfg.writeFile ("AFC_init.cfg");
}
void MainWindow::on_lineEdit_2_editingFinished()
{
    libconfig::Setting &root = cfg.getRoot();
    libconfig::Setting &init= root["init"][0];
    libconfig::Setting &ems_ip2a =init.lookup("ems_ip2");
    ems_ip2a = ui->lineEdit_2->text().toStdString();
    ems_ip2  = ui->lineEdit_2->text().toStdString();
    cfg.writeFile ("AFC_init.cfg");
}
void MainWindow::on_lineEdit_3_editingFinished()
{
    libconfig::Setting &root = cfg.getRoot();
    libconfig::Setting &init= root["init"][0];
    libconfig::Setting &pcs_ipa =init.lookup("pcs_ip");
    pcs_ipa = ui->lineEdit_3->text().toStdString();
    pcs_ip  = ui->lineEdit_3->text().toStdString();
    cfg.writeFile ("AFC_init.cfg");
}
void MainWindow::on_lineEdit_4_editingFinished()
{
    libconfig::Setting &root = cfg.getRoot();
    libconfig::Setting &init= root["init"][0];
    libconfig::Setting &mbms_eida =init.lookup("mbms_eid");
    mbms_eida = ui->lineEdit_4->text().toStdString();
    mbms_eid  = ui->lineEdit_4->text().toStdString();
    cfg.writeFile ("AFC_init.cfg");
}
void MainWindow::on_lineEdit_5_editingFinished()
{
    libconfig::Setting &root = cfg.getRoot();
    libconfig::Setting &init= root["init"][0];
    libconfig::Setting &HM_ipa =init.lookup("HM_ip");
    HM_ipa = ui->lineEdit_5->text().toStdString();
    HM_ip  = ui->lineEdit_5->text().toStdString();
    cfg.writeFile ("AFC_init.cfg");
}
void MainWindow::on_lineEdit_10_editingFinished()
{
    libconfig::Setting &root = cfg.getRoot();
    libconfig::Setting &model_AFC_run = root["model_AFC_run"][0];
    libconfig::Setting &pq_pa =model_AFC_run.lookup("pq_p");
    pq_pa = ui->lineEdit_10->text().toStdString();
    pq_p   = ui->lineEdit_10->text().toStdString();
    cfg.writeFile ("AFC_init.cfg");
}
void MainWindow::on_lineEdit_11_editingFinished()
{
    libconfig::Setting &root = cfg.getRoot();
    libconfig::Setting &model_AFC_run =root["model_AFC_run"][0];
    libconfig::Setting &pq_qa =model_AFC_run.lookup("pq_q");
    pq_qa = ui->lineEdit_11->text().toStdString();
    pq_q  = ui->lineEdit_11->text().toStdString();
    cfg.writeFile ("AFC_init.cfg");
}
void MainWindow::on_lineEdit_13_editingFinished()
{
    libconfig::Setting &root = cfg.getRoot();
    libconfig::Setting &model_AFC_run =root["model_AFC_run"][0];
    libconfig::Setting &FP_line_p_basea =model_AFC_run.lookup("FP_line_p_base");
    FP_line_p_basea = ui->lineEdit_13->text().toStdString();
    FP_line_p_base  = ui->lineEdit_13->text().toStdString();
    cfg.writeFile ("AFC_init.cfg");
}
void MainWindow::on_lineEdit_14_editingFinished(){
    libconfig::Setting &root = cfg.getRoot();
    libconfig::Setting &model_AFC_run =root["model_AFC_run"][0];
    libconfig::Setting &afc_f1a =model_AFC_run.lookup("afc_f1");
    afc_f1a = ui->lineEdit_14->text().toStdString();
    afc_f1  = ui->lineEdit_14->text().toStdString();
    cfg.writeFile ("AFC_init.cfg");
}
void MainWindow::on_lineEdit_15_editingFinished(){
    libconfig::Setting &root = cfg.getRoot();
    libconfig::Setting &model_AFC_run =root["model_AFC_run"][0];
    libconfig::Setting &afc_f2a =model_AFC_run.lookup("afc_f2");
    afc_f2a = ui->lineEdit_15->text().toStdString();
    afc_f2  = ui->lineEdit_15->text().toStdString();
    cfg.writeFile ("AFC_init.cfg");
}
void MainWindow::on_lineEdit_16_editingFinished(){
    libconfig::Setting &root = cfg.getRoot();
    libconfig::Setting &model_AFC_run =root["model_AFC_run"][0];
    libconfig::Setting &afc_f3a =model_AFC_run.lookup("afc_f3");
    afc_f3a = ui->lineEdit_16->text().toStdString();
    afc_f3  = ui->lineEdit_16->text().toStdString();
    cfg.writeFile ("AFC_init.cfg");
}
void MainWindow::on_lineEdit_17_editingFinished(){
    libconfig::Setting &root = cfg.getRoot();
    libconfig::Setting &model_AFC_run =root["model_AFC_run"][0];
    libconfig::Setting &afc_f4a =model_AFC_run.lookup("afc_f4");
    afc_f4a = ui->lineEdit_17->text().toStdString();
    afc_f4  = ui->lineEdit_17->text().toStdString();
    cfg.writeFile ("AFC_init.cfg");
}
void MainWindow::on_lineEdit_18_editingFinished(){
    libconfig::Setting &root = cfg.getRoot();
    libconfig::Setting &model_AFC_run =root["model_AFC_run"][0];
    libconfig::Setting &afc_f5a =model_AFC_run.lookup("afc_f5");
    afc_f5a = ui->lineEdit_18->text().toStdString();
    afc_f5  = ui->lineEdit_18->text().toStdString();
    cfg.writeFile ("AFC_init.cfg");
}
void MainWindow::on_lineEdit_19_editingFinished(){
    libconfig::Setting &root = cfg.getRoot();
    libconfig::Setting &model_AFC_run =root["model_AFC_run"][0];
    libconfig::Setting &afc_f6a =model_AFC_run.lookup("afc_f6");
    afc_f6a = ui->lineEdit_19->text().toStdString();
    afc_f6  = ui->lineEdit_19->text().toStdString();
    cfg.writeFile ("AFC_init.cfg");
}
void MainWindow::on_lineEdit_20_editingFinished(){
    libconfig::Setting &root = cfg.getRoot();
    libconfig::Setting &model_AFC_run =root["model_AFC_run"][0];
    libconfig::Setting &afc_p1a =model_AFC_run.lookup("afc_p1");
    afc_p1a = ui->lineEdit_20->text().toStdString();
    afc_p1  = ui->lineEdit_20->text().toStdString();
    cfg.writeFile ("AFC_init.cfg");
}
void MainWindow::on_lineEdit_21_editingFinished(){
    libconfig::Setting &root = cfg.getRoot();
    libconfig::Setting &model_AFC_run =root["model_AFC_run"][0];
    libconfig::Setting &afc_p2a =model_AFC_run.lookup("afc_p2");
    afc_p2a = ui->lineEdit_21->text().toStdString();
    afc_p2  = ui->lineEdit_21->text().toStdString();
    cfg.writeFile ("AFC_init.cfg");
}
void MainWindow::on_lineEdit_22_editingFinished(){
    libconfig::Setting &root = cfg.getRoot();
    libconfig::Setting &model_AFC_run =root["model_AFC_run"][0];
    libconfig::Setting &afc_p3a =model_AFC_run.lookup("afc_p3");
    afc_p3a = ui->lineEdit_22->text().toStdString();
    afc_p3  = ui->lineEdit_22->text().toStdString();
    cfg.writeFile ("AFC_init.cfg");
}
void MainWindow::on_lineEdit_23_editingFinished(){
    libconfig::Setting &root = cfg.getRoot();
    libconfig::Setting &model_AFC_run =root["model_AFC_run"][0];
    libconfig::Setting &afc_p4a =model_AFC_run.lookup("afc_p4");
    afc_p4a = ui->lineEdit_23->text().toStdString();
    afc_p4  = ui->lineEdit_23->text().toStdString();
    cfg.writeFile ("AFC_init.cfg");
}
void MainWindow::on_lineEdit_24_editingFinished(){
    libconfig::Setting &root = cfg.getRoot();
    libconfig::Setting &model_AFC_run =root["model_AFC_run"][0];
    libconfig::Setting &afc_p5a =model_AFC_run.lookup("afc_p5");
    afc_p5a = ui->lineEdit_24->text().toStdString();
    afc_p5  = ui->lineEdit_24->text().toStdString();
    cfg.writeFile ("AFC_init.cfg");
}
void MainWindow::on_lineEdit_25_editingFinished(){
    libconfig::Setting &root = cfg.getRoot();
    libconfig::Setting &model_AFC_run =root["model_AFC_run"][0];
    libconfig::Setting &afc_p6a =model_AFC_run.lookup("afc_p6");
    afc_p6a = ui->lineEdit_25->text().toStdString();
    afc_p6  = ui->lineEdit_25->text().toStdString();
    cfg.writeFile ("AFC_init.cfg");
}
void MainWindow::on_lineEdit_27_editingFinished(){
    libconfig::Setting &root = cfg.getRoot();
    libconfig::Setting &model_AFC_run =root["model_AFC_run"][0];
    libconfig::Setting &Vq_q_basea =model_AFC_run.lookup("Vq_q_base");
    Vq_q_basea = ui->lineEdit_27->text().toStdString();
    Vq_q_base  = ui->lineEdit_27->text().toStdString();
    cfg.writeFile ("AFC_init.cfg");
}
void MainWindow::on_lineEdit_28_editingFinished(){
    libconfig::Setting &root = cfg.getRoot();
    libconfig::Setting &model_AFC_run =root["model_AFC_run"][0];
    libconfig::Setting &Vq_v_basea =model_AFC_run.lookup("Vq_v_base");
    Vq_v_basea = ui->lineEdit_28->text().toStdString();
    Vq_v_base  = ui->lineEdit_28->text().toStdString();
    cfg.writeFile ("AFC_init.cfg");
}
void MainWindow::on_lineEdit_29_editingFinished(){
    libconfig::Setting &root = cfg.getRoot();
    libconfig::Setting &model_AFC_run =root["model_AFC_run"][0];
    libconfig::Setting &afc_v1a =model_AFC_run.lookup("afc_v1");
    afc_v1a = ui->lineEdit_29->text().toStdString();
    afc_v1  = ui->lineEdit_29->text().toStdString();
    cfg.writeFile ("AFC_init.cfg");
}
void MainWindow::on_lineEdit_30_editingFinished(){
    libconfig::Setting &root = cfg.getRoot();
    libconfig::Setting &model_AFC_run =root["model_AFC_run"][0];
    libconfig::Setting &afc_v2a =model_AFC_run.lookup("afc_v2");
    afc_v2a = ui->lineEdit_30->text().toStdString();
    afc_v2  = ui->lineEdit_30->text().toStdString();
    cfg.writeFile ("AFC_init.cfg");
}
void MainWindow::on_lineEdit_31_editingFinished(){
    libconfig::Setting &root = cfg.getRoot();
    libconfig::Setting &model_AFC_run =root["model_AFC_run"][0];
    libconfig::Setting &afc_v3a =model_AFC_run.lookup("afc_v3");
    afc_v3a = ui->lineEdit_31->text().toStdString();
    afc_v3  = ui->lineEdit_31->text().toStdString();
    cfg.writeFile ("AFC_init.cfg");
}
void MainWindow::on_lineEdit_32_editingFinished(){
    libconfig::Setting &root = cfg.getRoot();
    libconfig::Setting &model_AFC_run =root["model_AFC_run"][0];
    libconfig::Setting &afc_v4a =model_AFC_run.lookup("afc_v4");
    afc_v4a = ui->lineEdit_32->text().toStdString();
    afc_v4  = ui->lineEdit_32->text().toStdString();
    cfg.writeFile ("AFC_init.cfg");
}
void MainWindow::on_lineEdit_33_editingFinished(){
    libconfig::Setting &root = cfg.getRoot();
    libconfig::Setting &model_AFC_run =root["model_AFC_run"][0];
    libconfig::Setting &afc_v5a =model_AFC_run.lookup("afc_v5");
    afc_v5a = ui->lineEdit_33->text().toStdString();
    afc_v5  = ui->lineEdit_33->text().toStdString();
    cfg.writeFile ("AFC_init.cfg");
}
void MainWindow::on_lineEdit_34_editingFinished(){
    libconfig::Setting &root = cfg.getRoot();
    libconfig::Setting &model_AFC_run =root["model_AFC_run"][0];
    libconfig::Setting &afc_v6a =model_AFC_run.lookup("afc_v6");
    afc_v6a = ui->lineEdit_34->text().toStdString();
    afc_v6  = ui->lineEdit_34->text().toStdString();
    cfg.writeFile ("AFC_init.cfg");
}
void MainWindow::on_lineEdit_35_editingFinished(){
    libconfig::Setting &root = cfg.getRoot();
    libconfig::Setting &model_AFC_run =root["model_AFC_run"][0];
    libconfig::Setting &afc_q1a =model_AFC_run.lookup("afc_q1");
    afc_q1a = ui->lineEdit_35->text().toStdString();
    afc_q1  = ui->lineEdit_35->text().toStdString();
    cfg.writeFile ("AFC_init.cfg");
}
void MainWindow::on_lineEdit_36_editingFinished(){
    libconfig::Setting &root = cfg.getRoot();
    libconfig::Setting &model_AFC_run =root["model_AFC_run"][0];
    libconfig::Setting &afc_q2a =model_AFC_run.lookup("afc_q2");
    afc_q2a = ui->lineEdit_36->text().toStdString();
    afc_q2  = ui->lineEdit_36->text().toStdString();
    cfg.writeFile ("AFC_init.cfg");
}
void MainWindow::on_lineEdit_37_editingFinished(){
    libconfig::Setting &root = cfg.getRoot();
    libconfig::Setting &model_AFC_run =root["model_AFC_run"][0];
    libconfig::Setting &afc_q3a =model_AFC_run.lookup("afc_q3");
    afc_q3a = ui->lineEdit_37->text().toStdString();
    afc_q3  = ui->lineEdit_37->text().toStdString();
    cfg.writeFile ("AFC_init.cfg");
}
void MainWindow::on_lineEdit_38_editingFinished(){
    libconfig::Setting &root = cfg.getRoot();
    libconfig::Setting &model_AFC_run =root["model_AFC_run"][0];
    libconfig::Setting &afc_q4a =model_AFC_run.lookup("afc_q4");
    afc_q4a = ui->lineEdit_38->text().toStdString();
    afc_q4  = ui->lineEdit_38->text().toStdString();
    cfg.writeFile ("AFC_init.cfg");
}
void MainWindow::on_lineEdit_39_editingFinished(){
    libconfig::Setting &root = cfg.getRoot();
    libconfig::Setting &model_AFC_run =root["model_AFC_run"][0];
    libconfig::Setting &afc_q5a =model_AFC_run.lookup("afc_q5");
    afc_q5a = ui->lineEdit_39->text().toStdString();
    afc_q5  = ui->lineEdit_39->text().toStdString();
    cfg.writeFile ("AFC_init.cfg");
}
void MainWindow::on_lineEdit_40_editingFinished(){
    libconfig::Setting &root = cfg.getRoot();
    libconfig::Setting &model_AFC_run =root["model_AFC_run"][0];
    libconfig::Setting &afc_q6a =model_AFC_run.lookup("afc_q6");
    afc_q6a = ui->lineEdit_40->text().toStdString();
    afc_q6  = ui->lineEdit_40->text().toStdString();
    cfg.writeFile ("AFC_init.cfg");
}

void MainWindow::on_lineEdit_41_editingFinished()
{
    libconfig::Setting &root = cfg.getRoot();
    libconfig::Setting &model_AFC_run =root["model_AFC_run"][0];
    libconfig::Setting &PQ_q_refa =model_AFC_run.lookup("PQ_q_ref");
    PQ_q_refa = ui->lineEdit_41->text().toStdString();
    PQ_q_ref  = ui->lineEdit_41->text().toStdString();
    cfg.writeFile ("AFC_init.cfg");
}


void MainWindow::on_lineEdit_42_editingFinished()
{
    libconfig::Setting &root = cfg.getRoot();
    libconfig::Setting &model_AFC_run =root["model_AFC_run"][0];
    libconfig::Setting &PF_pf_refa =model_AFC_run.lookup("PF_pf_ref");
    PF_pf_refa = ui->lineEdit_42->text().toStdString();
    PF_pf_ref  = ui->lineEdit_42->text().toStdString();
    cfg.writeFile ("AFC_init.cfg");
}


void MainWindow::on_lineEdit_60_editingFinished()
{
    libconfig::Setting &root = cfg.getRoot();
    libconfig::Setting &init= root["init"][0];
    libconfig::Setting &soc_maxa =init.lookup("soc_max");
    soc_maxa = ui->lineEdit_60->text().toStdString();
    soc_max  = ui->lineEdit_60->text().toStdString();
    cfg.writeFile ("AFC_init.cfg");
}


void MainWindow::on_lineEdit_61_editingFinished()
{
    libconfig::Setting &root = cfg.getRoot();
    libconfig::Setting &init= root["init"][0];
    libconfig::Setting &soc_mina =init.lookup("soc_min");
    soc_mina = ui->lineEdit_61->text().toStdString();
    soc_min  = ui->lineEdit_61->text().toStdString();
    cfg.writeFile ("AFC_init.cfg");
}


void MainWindow::on_lineEdit_62_editingFinished()
{
    libconfig::Setting &root = cfg.getRoot();
    libconfig::Setting &init= root["init"][0];
    libconfig::Setting &System_p_maxa =init.lookup("System_p_max");
    System_p_maxa = ui->lineEdit_62->text().toStdString();
    System_p_max  = ui->lineEdit_62->text().toStdString();
    cfg.writeFile ("AFC_init.cfg");
}


void MainWindow::on_lineEdit_63_editingFinished()
{
    libconfig::Setting &root = cfg.getRoot();
    libconfig::Setting &init= root["init"][0];
    libconfig::Setting &System_p_mina =init.lookup("System_p_min");
    System_p_mina = ui->lineEdit_63->text().toStdString();
    System_p_min  = ui->lineEdit_63->text().toStdString();
    cfg.writeFile ("AFC_init.cfg");
}


void MainWindow::on_lineEdit_64_editingFinished()
{
    libconfig::Setting &root = cfg.getRoot();
    libconfig::Setting &init= root["init"][0];
    libconfig::Setting &System_q_maxa =init.lookup("System_q_max");
    System_q_maxa = ui->lineEdit_64->text().toStdString();
    System_q_max  = ui->lineEdit_64->text().toStdString();
    cfg.writeFile ("AFC_init.cfg");
}


void MainWindow::on_lineEdit_65_editingFinished()
{
    libconfig::Setting &root = cfg.getRoot();
    libconfig::Setting &init= root["init"][0];
    libconfig::Setting &System_q_mina =init.lookup("System_q_min");
    System_q_mina = ui->lineEdit_65->text().toStdString();
    System_q_min  = ui->lineEdit_65->text().toStdString();
    cfg.writeFile ("AFC_init.cfg");
}


void MainWindow::on_pushButton_51_pressed()
{
    ui->lineEdit_50->setText("(1)dReg0.25");
}

void MainWindow::on_pushButton_52_pressed()
{
    ui->lineEdit_50->setText("(2)dReg0.25");
}

void MainWindow::on_pushButton_53_pressed()
{
    ui->lineEdit_50->setText("(3)dReg0.25");
}

void MainWindow::on_pushButton_54_pressed()
{
    ui->lineEdit_50->setText("(4)dReg0.25");
}

void MainWindow::on_pushButton_55_pressed()
{
    ui->lineEdit_50->setText("(5)dReg0.25");
}

void MainWindow::on_pushButton_56_pressed()
{
    ui->lineEdit_50->setText("(6)dReg0.5");
}

void MainWindow::on_pushButton_57_pressed()
{
    ui->lineEdit_50->setText("(7)dReg0.5");
}

void MainWindow::on_pushButton_58_pressed()
{
    ui->lineEdit_50->setText("(8)dReg0.5");
}

void MainWindow::on_pushButton_59_pressed()
{
    ui->lineEdit_50->setText("(9)dReg0.5");
}

void MainWindow::on_pushButton_60_pressed()
{
    ui->lineEdit_50->setText("(10)dReg0.5");
}

void MainWindow::on_pushButton_61_pressed()
{
    ui->lineEdit_50->setText("(11)sReg");
}

void MainWindow::on_pushButton_62_pressed()
{
    ui->lineEdit_50->setText("(12)sReg");
}

void MainWindow::on_pushButton_63_pressed()
{
    ui->lineEdit_50->setText("(13)sReg");
}


void MainWindow::on_pushButton_11_pressed()
{
    if (ui->lineEdit_50->text() == "None"){
        ability_test = "None";
    }
    else if (ui->lineEdit_50->text() == "(1)dReg0.25"){
        ability_test = "1";
    }
    else if (ui->lineEdit_50->text() == "(2)dReg0.25"){
        ability_test = "2";
    }
    else if (ui->lineEdit_50->text() == "(3)dReg0.25"){
        ability_test = "3";
    }
    else if (ui->lineEdit_50->text() == "(4)dReg0.25"){
        ability_test = "4";
    }
    else if (ui->lineEdit_50->text() == "(5)dReg0.25"){
        ability_test = "5";
    }
    else if (ui->lineEdit_50->text() == "(6)dReg0.5"){
        ability_test = "6";
    }
    else if (ui->lineEdit_50->text() == "(7)dReg0.5"){
        ability_test = "7";
    }
    else if (ui->lineEdit_50->text() == "(8)dReg0.5"){
        ability_test = "8";
    }
    else if (ui->lineEdit_50->text() == "(9)dReg0.5"){
        ability_test = "9";
    }
    else if (ui->lineEdit_50->text() == "(10)dReg0.5"){
        ability_test = "10";
    }
    else if (ui->lineEdit_50->text() == "(11)sReg"){
        ability_test = "11";
    }
    else if (ui->lineEdit_50->text() == "(12)sReg"){
        ability_test = "12";
    }
    else if (ui->lineEdit_50->text() == "(13)sReg"){
        ability_test = "13";
    }
}
void MainWindow::on_pushButton_66_pressed()
{
    double mongo_return[60] = {-1};
    std::string mongo_return_str[60] = {""};
    thread_mongo_status -> mongo_find_status(mongo_return, mongo_return_str);
    int count = 0;
    //model->clear();
    for(int i=model->rowCount()-1;i>=0;i--) {
        model->removeRow(i);
    }
    while (true){
        int rowCnt;
        model = (QStandardItemModel *)ui->tableView->model();
        rowCnt = model->rowCount();
        QStandardItem *i0 = new QStandardItem("");/*ID*/
        QStandardItem *i1 = new QStandardItem("");/*time*/
        QStandardItem *i2 = new QStandardItem("");/*event*/
        QStandardItem *i3 = new QStandardItem("");/*group*/
        QStandardItem *i4 = new QStandardItem("");/*level*/
        QStandardItem *i5 = new QStandardItem("");/*show*/

        model->setItem(rowCnt, 0, i0);
        model->setItem(rowCnt, 1, i1);
        model->setItem(rowCnt, 2, i2);
        model->setItem(rowCnt, 3, i3);
        model->setItem(rowCnt, 4, i4);
        model->setItem(rowCnt, 5, i5);

        model->item(rowCnt, 1)->setText( QDateTime::fromTime_t(mongo_return[count]/1000-28800).toString("yyyy-MM-dd hh:mm:ss") );
        model->item(rowCnt, 4)->setText(QString::number(mongo_return[count + 1],10,2));
        model->item(rowCnt, 5)->setText(QString::number(mongo_return[count + 2],10,2));
        model->item(rowCnt, 0)->setText(QString::fromStdString(mongo_return_str[count]));
        model->item(rowCnt, 2)->setText(QString::fromStdString(mongo_return_str[count + 1]));
        model->item(rowCnt, 3)->setText(QString::fromStdString(mongo_return_str[count + 2]));
        count = count + 3;
        if (mongo_return[count] == -1 or count >=60){
            break;
        }

    }
}


void MainWindow::on_pushButton_67_pressed()
{
    model = (QStandardItemModel *)ui->tableView->model();

    //model->clear();
    for(int i=model->rowCount()-1;i>=0;i--) {
        model->removeRow(i);
    }
}


void MainWindow::on_pushButton_68_pressed()
{
    double mongo_return[60] = {-1};
    std::string mongo_return_str[60] = {""};
    thread_mongo_status -> mongo_find_alarm(mongo_return, mongo_return_str);
    int count = 0;
    //model2->clear();
    for(int i=model2->rowCount()-1;i>=0;i--) {
        model2->removeRow(i);
    }
    model2 = (QStandardItemModel *)ui->tableView_2->model();
    while (true){
        int rowCnt;
        rowCnt = model2->rowCount();
        QStandardItem *i0 = new QStandardItem("");/*ID*/
        QStandardItem *i1 = new QStandardItem("");/*time*/
        QStandardItem *i2 = new QStandardItem("");/*event*/
        QStandardItem *i3 = new QStandardItem("");/*group*/
        QStandardItem *i4 = new QStandardItem("");/*level*/
        QStandardItem *i5 = new QStandardItem("");/*show*/

        model2->setItem(rowCnt, 0, i0);
        model2->setItem(rowCnt, 1, i1);
        model2->setItem(rowCnt, 2, i2);
        model2->setItem(rowCnt, 3, i3);
        model2->setItem(rowCnt, 4, i4);
        model2->setItem(rowCnt, 5, i5);

        model2->item(rowCnt, 1)->setText( QDateTime::fromTime_t(mongo_return[count]/1000-28800).toString("yyyy-MM-dd hh:mm:ss") );
        model2->item(rowCnt, 4)->setText(QString::number(mongo_return[count + 1],10,2));
        model2->item(rowCnt, 5)->setText(QString::number(mongo_return[count + 2],10,2));
        model2->item(rowCnt, 0)->setText(QString::fromStdString(mongo_return_str[count]));
        model2->item(rowCnt, 2)->setText(QString::fromStdString(mongo_return_str[count + 1]));
        model2->item(rowCnt, 3)->setText(QString::fromStdString(mongo_return_str[count + 2]));
        count = count + 3;
        if (mongo_return[count] == -1 or count >=60){
            break;
        }

    }
}


void MainWindow::on_pushButton_69_pressed()
{
    model2 = (QStandardItemModel *)ui->tableView_2->model();

    //model->clear();
    for(int i=model2->rowCount()-1;i>=0;i--) {
        model2->removeRow(i);
    }
}

