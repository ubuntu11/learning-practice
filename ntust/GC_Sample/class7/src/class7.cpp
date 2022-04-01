#include <iostream>
#include <windows.h>
#include <modbus/modbus.h>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/pool.hpp>
#include <mongocxx/cursor.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types/value.hpp>
#include <thread>
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;
using bsoncxx::builder::basic::sub_array;
SYSTEMTIME currentTime_start_new;
void mongo_time(auto doc, std::string point_name, double &return_data){
	try{
		bsoncxx::document::element bson_data = doc[point_name];
		if(bson_data.type() == bsoncxx::type::k_date){
			std::cout << bsoncxx::to_json(doc) << std::endl;
			return_data = bson_data.get_date();
//			std::cout<<"return_data :"<<point_name<<" = "<< std::to_string(return_data) <<std::endl;
		}
		else{
			// Error
		}
	}
	catch(...){}
}
void mongo_data(auto doc, std::string point_name ,double &return_data){
	try{
		bsoncxx::document::element bson_data = doc[point_name];
		if(bson_data.type() == bsoncxx::type::k_double) {
			return_data = bson_data.get_double().value;
		}
		else if(bson_data.type() == bsoncxx::type::k_int32){
			return_data = bson_data.get_int32().value;
		}
		else{
			// Error
			return_data=-1;
		}
	}
	catch(...){
		return_data=-1;
	}
}
void mongo_dic(auto doc, std::string point_name,
		std::string *name, double *data, int scale_len){
	try{
		bsoncxx::document::element bson_data = doc[point_name];
		if(bson_data.type() == bsoncxx::type::k_document){
			bsoncxx::document::view subdoc = bson_data.get_document().value;
			for (int i = 0;i< scale_len;i++){
				bsoncxx::document::element data_buffer = subdoc[name[i]];
				data[i] = data_buffer.get_double().value;
			}
		}
		else{
			for (int i = 0;i< scale_len;i++){
				data[i]=-1;
			}
		}
	}
	catch(...){
		for (int i = 0;i< scale_len;i++){
			data[i]=-1;
		}
	}
}
void EMS_ReadData_thread(bool &thread_end, int &thread_count, double *emsdata,
		double *mbmsdata, bool &remote_first_run, std::string set_source,
		std::string *scale_name, double *scale_data, int &scale_len, int &mode,
		double &soc_max, double &soc_min, double &System_p_max,
		double &System_p_min, double &System_q_max, double &System_q_min,
		double *f_arr, double *p_arr, double *v_arr, double *q_arr,
		double &PQ_p_ref, double &PQ_q_ref, int &FP_type,
		double &FP_soc_goal_percent, double &FP_main_f_set,
		double &FP_line_p_base, double &FP_soc_dead_band, double &FP_soc_adjust,
		int &VC_type, double &Vq_v_base, double &Vq_q_base, double &Vq_droop,
		double &Vq_v_set, double &PF_pf_ref, double a1, double b1, double a21,
		double b21, double a22, double b22, double a23, double b23, double b3,
		double a41, double b41, double a42, double b42, double a43, double b43,
		double a5, double b5, double a_sReg, double b_sReg, double a_qAuto,
		double b_qAuto, double &soc, int &ems_count, double &mbms_time,
		int &mbms_count, int &pcs_write_error_count, double &pcs_reset,
		double &pcs_control, double &pcs_control_time_remote,modbus_t *pcs_read_ctx, int pcs_read_ret,
		std::string MBMS_id, std::string PCS_id, mongocxx::database db,
		mongocxx::database db_local, bool &error_flag) {
	int lock_buffer_1s = int(currentTime_start_new.wSecond);
	int lock_buffer_100ms = int(currentTime_start_new.wMilliseconds / 100);
	std::string fc_name = "EMS_ReadData_thread";
	while (true) {
		while (lock_buffer_100ms
				== int(currentTime_start_new.wMilliseconds / 100)) {
			if (thread_end == true)
				return;
			GetSystemTime(&currentTime_start_new);
			Sleep(100);
		}
		lock_buffer_100ms = int(currentTime_start_new.wMilliseconds / 100);
		if (thread_end == true)
			return;
		if (lock_buffer_1s != int(currentTime_start_new.wSecond)) {
			lock_buffer_1s = int(currentTime_start_new.wSecond);
			thread_count++;
			try {
				mongocxx::options::find opts; /*限制搜尋數量*/
				auto order = bsoncxx::builder::stream::document { } << "time"
						<< -1 << bsoncxx::builder::stream::finalize;
				opts.sort(order.view());
				opts.limit(1);
				//			std::cout<<"set_source :"<<set_source<<std::endl;
				if (set_source == "Remote") {
					mongocxx::collection coll_site = db["site_control"];
					try{
						mongocxx::cursor cursor_site = coll_site.find(
								make_document(kvp("ID", "site_control")), opts);
						int count = 0;
						for (bsoncxx::v_noabi::document::view doc : cursor_site) {
							try {
								double modeb;
								double FP_typeb;
								double VC_typeb;
	//							std::cout<< bsoncxx::to_json(doc)<<std::endl;
								mongo_dic(doc, "scale", scale_name, scale_data,
										scale_len);
	//							std::cout <<"scale_name[0] :"<< scale_name[0] <<" scale_data[0] :"<<scale_data[0]<<" scale_len :"<<scale_len<< std::endl;
								mongo_data(doc, "mode", modeb);
								mode = (int) modeb;
	//							std::cout <<"mode :"<< mode << std::endl;
								mongo_data(doc, "soc_max", soc_max);
								mongo_data(doc, "soc_min", soc_min);
								mongo_data(doc, "System_p_max", System_p_max);
								mongo_data(doc, "System_p_min", System_p_min);
								mongo_data(doc, "System_q_max", System_q_max);
								mongo_data(doc, "System_q_min", System_q_min);
								mongo_data(doc, "PQ_p_ref", PQ_p_ref);
								mongo_data(doc, "PQ_q_ref", PQ_q_ref);
								mongo_data(doc, "FP_type", FP_typeb);
								FP_type = (int) FP_typeb;
								mongo_data(doc, "FP_soc_goal_percent",
										FP_soc_goal_percent);
								mongo_data(doc, "FP_main_f_set", FP_main_f_set);
								mongo_data(doc, "f1_line_set", f_arr[0]);
								mongo_data(doc, "f2_line_set", f_arr[1]);
								mongo_data(doc, "f3_line_set", f_arr[2]);
								mongo_data(doc, "f4_line_set", f_arr[3]);
								mongo_data(doc, "f5_line_set", f_arr[4]);
								mongo_data(doc, "f6_line_set", f_arr[5]);
								mongo_data(doc, "p1_line_set", p_arr[0]);
								mongo_data(doc, "p2_line_set", p_arr[1]);
								mongo_data(doc, "p3_line_set", p_arr[2]);
								mongo_data(doc, "p4_line_set", p_arr[3]);
								mongo_data(doc, "p5_line_set", p_arr[4]);
								mongo_data(doc, "p6_line_set", p_arr[5]);
								mongo_data(doc, "FP_line_p_base", FP_line_p_base);
								mongo_data(doc, "FP_soc_dead_band",
										FP_soc_dead_band);
								mongo_data(doc, "FP_soc_adjust", FP_soc_adjust);
								mongo_data(doc, "VC_type", VC_typeb);
								VC_type = (int) VC_typeb;
								mongo_data(doc, "Vq_v_base", Vq_v_base);
								mongo_data(doc, "Vq_q_base", Vq_q_base);
								mongo_data(doc, "Vq_droop", Vq_droop);
								mongo_data(doc, "Vq_v_set", Vq_v_set);
								mongo_data(doc, "Vq_v1_set", v_arr[0]);
								mongo_data(doc, "Vq_v2_set", v_arr[1]);
								mongo_data(doc, "Vq_v3_set", v_arr[2]);
								mongo_data(doc, "Vq_v4_set", v_arr[3]);
								mongo_data(doc, "Vq_v5_set", v_arr[4]);
								mongo_data(doc, "Vq_v6_set", v_arr[5]);
								mongo_data(doc, "Vq_v7_set", v_arr[6]);
								mongo_data(doc, "Vq_q1_set", q_arr[0]);
								mongo_data(doc, "Vq_q2_set", q_arr[1]);
								mongo_data(doc, "Vq_q3_set", q_arr[2]);
								mongo_data(doc, "Vq_q4_set", q_arr[3]);
								mongo_data(doc, "Vq_q5_set", q_arr[4]);
								mongo_data(doc, "Vq_q6_set", q_arr[5]);
								mongo_data(doc, "Vq_q7_set", q_arr[6]);
								mongo_data(doc, "PF_pf_ref", PF_pf_ref);
								//						mongo_data(doc,"PF_p_ref",PF_p_ref);
								//						mongo_data(doc,"scheduleFlag",scheduleFlag);
								count++;
								/*更新控制規則*/
								/*dReg 計算*/
								FP_soc_adjust = FP_soc_adjust / 100.0;
								p_arr[0] = p_arr[0] * FP_line_p_base / 100;
								p_arr[1] = p_arr[1] * FP_line_p_base / 100;
								p_arr[2] = p_arr[2] * FP_line_p_base / 100;
								p_arr[3] = p_arr[3] * FP_line_p_base / 100;
								p_arr[4] = p_arr[4] * FP_line_p_base / 100;
								p_arr[5] = p_arr[5] * FP_line_p_base / 100;

								a1 = (p_arr[1] - p_arr[0]) / (f_arr[1] - f_arr[0])
										* 1.000;
								a21 = ((p_arr[2] + (FP_soc_adjust * FP_line_p_base))
										- p_arr[1]) / (f_arr[2] - f_arr[1]) * 1.000;
								a22 = (p_arr[2] - p_arr[1]) / (f_arr[2] - f_arr[1])
										* 1.000;
								a23 = ((p_arr[2] - (FP_soc_adjust * FP_line_p_base))
										- p_arr[1]) / (f_arr[2] - f_arr[1]) * 1.000;
								//						a3 = 0; /*中間值自由調整為0無須斜率*/
								a41 = (p_arr[4]
										- (p_arr[3]
												+ (FP_soc_adjust * FP_line_p_base)))
										/ (f_arr[4] - f_arr[3]) * 1.000;
								a42 = (p_arr[4] - p_arr[3]) / (f_arr[4] - f_arr[3])
										* 1.000;
								a43 = (p_arr[4]
										- (p_arr[3]
												- (FP_soc_adjust * FP_line_p_base)))
										/ (f_arr[4] - f_arr[3]) * 1.000;
								a5 = (p_arr[5] - p_arr[4]) / (f_arr[5] - f_arr[4])
										* 1.000;

								b1 = p_arr[0] - a1 * f_arr[0] * 1.000;
								b21 = p_arr[1] - a21 * f_arr[1] * 1.000;
								b22 = p_arr[1] - a22 * f_arr[1] * 1.000;
								b23 = p_arr[1] - a23 * f_arr[1] * 1.000;
								b3 = FP_soc_adjust * FP_line_p_base;
								b41 = p_arr[4] - a41 * f_arr[4] * 1.000;
								b42 = p_arr[4] - a42 * f_arr[4] * 1.000;
								b43 = p_arr[4] - a43 * f_arr[4] * 1.000;
								b5 = p_arr[5] - a5 * f_arr[5] * 1.000;
								/*sReg計算*/
								a_sReg = (p_arr[3] - p_arr[2])
										/ (f_arr[3] - f_arr[2]) * 1.000;
								;
								b_sReg = p_arr[3] - a_sReg * f_arr[3] * 1.000;
								/*VQ計算*/
								a_qAuto = Vq_droop;
								b_qAuto = Vq_q_base - (Vq_droop * Vq_v_base);
							} catch (...) {
								std::cout << "error" << std::endl;
							}
							if (count == 0) {
								/*沒讀到*/
								ems_count++;
								/*回傳ui*/
								for (int x = 0; x < 48; x++) {
									emsdata[x] = -1;
								}
								//
							} else {
								/*有讀到資料*/
								ems_count = 0;
								remote_first_run = false;
								/*回傳ui*/
								emsdata[0] = mode;
								//					emsdata[1] = mode;
								emsdata[2] = soc_max;
								emsdata[3] = soc_min;
								emsdata[4] = System_p_max;
								emsdata[5] = System_p_min;
								emsdata[6] = System_q_max;
								emsdata[7] = System_q_min;
								emsdata[8] = PQ_p_ref;
								emsdata[9] = PQ_q_ref;
								emsdata[10] = FP_type;
								emsdata[11] = FP_soc_goal_percent;
								emsdata[12] = FP_main_f_set;
								emsdata[13] = f_arr[0];
								emsdata[14] = f_arr[1];
								emsdata[15] = f_arr[2];
								emsdata[16] = f_arr[3];
								emsdata[17] = f_arr[4];
								emsdata[18] = f_arr[5];
								emsdata[19] = p_arr[0];
								emsdata[20] = p_arr[1];
								emsdata[21] = p_arr[2];
								emsdata[22] = p_arr[3];
								emsdata[23] = p_arr[4];
								emsdata[24] = p_arr[5];
								emsdata[25] = FP_line_p_base;
								emsdata[26] = FP_soc_dead_band;
								emsdata[27] = FP_soc_adjust;
								emsdata[28] = VC_type;
								emsdata[29] = Vq_v_base;
								emsdata[30] = Vq_q_base;
								emsdata[31] = Vq_droop;
								emsdata[32] = Vq_v_set;
								emsdata[33] = v_arr[0];
								emsdata[34] = v_arr[1];
								emsdata[35] = v_arr[2];
								emsdata[36] = v_arr[3];
								emsdata[37] = v_arr[4];
								emsdata[38] = v_arr[5];
								emsdata[39] = v_arr[6];
								emsdata[40] = q_arr[0];
								emsdata[41] = q_arr[1];
								emsdata[42] = q_arr[2];
								emsdata[43] = q_arr[3];
								emsdata[44] = q_arr[4];
								emsdata[45] = q_arr[5];
								emsdata[46] = q_arr[6];
								emsdata[47] = PF_pf_ref;

							}
						}
					}catch(...){}


					mongocxx::collection coll_pcs = db["pcs_control"];
					try{
						mongocxx::cursor cursor_pcs = coll_pcs.find(
								make_document(kvp("ID", PCS_id)), opts);
	//					std::cout<< "mongo data find"<<std::endl;
						for (bsoncxx::v_noabi::document::view doc : cursor_pcs) {
							try {
	//							std::cout<< bsoncxx::to_json(doc)<<std::endl;
								mongo_data(doc, "time", pcs_control_time_remote);
								mongo_data(doc, "control", pcs_control);
								mongo_data(doc, "reset", pcs_reset);
							} catch (...) {
							}
						}
					}catch(...){}


				} else if (set_source == "Local") {

				}
				mongocxx::collection coll_mbms = db["mbms"];
				try{
					mongocxx::cursor cursor_mbms = coll_mbms.find(
							make_document(kvp("ID", MBMS_id)), opts);
					for (bsoncxx::v_noabi::document::view doc : cursor_mbms) {
						try {
							//					std::cout<< bsoncxx::to_json(doc)<<std::endl;

							double mbms_time_next;
							mongo_data(doc, "soc_sys", soc);
							mongo_time(doc, "time", mbms_time_next);
							//					std::cout<< "mbms_time_next :"<<std::to_string(mbms_time_next)<<std::endl;
							if (mbms_time != mbms_time_next) {
								mbms_time = mbms_time_next;
								mbms_count = 0;
								//						std::cout <<"mbms_time !! :"<<std::to_string(mbms_time)<< std::endl;
							} else {
								mbms_count++;
								//						std::cout <<"mbms_time == :"<<std::to_string(mbms_time)<< std::endl;
							}
						} catch (...) {
						}
					}
				}catch(...){}

				mbmsdata[0] = soc;
				error_flag = false;
			} catch (...) {
				if (error_flag == false) {
					std::vector<bsoncxx::document::value> error_docs;
					time_t now = time(0);
					now += 28800;
					error_docs.push_back(
							make_document(kvp("ID", "GC"),
									kvp("time",
											bsoncxx::types::b_date {
													std::chrono::system_clock::from_time_t(
															now) }),
									kvp("event", "unknow error " + fc_name),
									kvp("group", fc_name), kvp("level", 1),
										kvp("show", 1)));
					try{
						db_local["alarm"].insert_many(error_docs);
					}catch(...){}
					error_flag = true;
				}
			}
			thread_count--;
		}
	}
}
int main() {
	std::thread *t_EMS_ReadData_thread = nullptr;

	mongocxx::instance::current();

	mongocxx::database db4;
	std::string url_data="mongodb://root:pc152@127.0.0.1:27017/admin?authSource=admin";
	std::cout<<url_data<<std::endl;
	mongocxx::uri uri(url_data);
	mongocxx::client client(uri);
	db4 = client["AFC"];

	mongocxx::database db4_local;
	std::string url_data_local="mongodb://root:pc152@127.0.0.1:27017/admin?authSource=admin";
	std::cout<<url_data_local<<std::endl;
	mongocxx::uri uri_local(url_data_local);
	mongocxx::client client_local(uri_local);
	db4_local = client_local["AFC_local"];

	std::string ip="127.0.0.1";
	int port =502;
	int id =2;
	modbus_t *pcs_read_ctx = modbus_new_tcp(ip.c_str(), port);
	if (pcs_read_ctx == NULL) {
		printf("Unable to create the libmodbus  read context\n");
		fflush(stdout);
	}
	modbus_set_slave(pcs_read_ctx, id);
	modbus_set_response_timeout(pcs_read_ctx, 0, 300000);
	int pcs_read_ret = modbus_connect(pcs_read_ctx);
	/*----------------------------------*/
	/*測試參數*/
	int pcs_read_ret_ = pcs_read_ret;
	bool thread_end = false;
	int thread_count = 0;

	bool error_flag4;
	double emsdata[48];
	double mbmsdata[1];
	bool remote_first_run = true;
	std::string set_source = "Remote";
	int scale_len = 1;
	std::string scale_name[scale_len] = { "60f8e79f54841d42146b31df" };
	double scale_data[scale_len];
	int modea;
	double scalea;
	double soc_maxa;
	double soc_mina;
	double System_p_maxa;
	double System_p_mina;
	double System_q_maxa;
	double System_q_mina;
	double f_arra[6];
	double p_arra[6];
	double v_arra[6];
	double q_arra[6];

	double PQ_p_refa;
	double PQ_q_refa;
	int FP_typea;
	double FP_soc_goal_percenta;
	double FP_main_f_seta;
	double FP_line_p_basea;
	double FP_soc_dead_banda;
	double FP_soc_adjusta;
	int VC_typea;
	double Vq_v_basea;
	double Vq_q_basea;
	double Vq_droopa;
	double Vq_v_seta;
	double PF_pf_refa;
	double a1,b1,a21,b21,a22,b22,a23,b23,b3,a41,b41,a42,b42,a43,b43,
	a5,b5,a_sReg,b_sReg,a_qAuto,b_qAuto;
	double soc;
	int ems_count;
	double mbms_time;
	int mbms_count;
	int pcs_write_error_count;

	double pcs_reset;
	double pcs_control;
	double pcs_control_time;
	double pcs_control_time_remote;

	std::string MBMS_eid = "12345abc";
	std::string PCS_eid = "";
	/*----------------------------------*/
	t_EMS_ReadData_thread = new std::thread(
		[&]() {
			EMS_ReadData_thread(std::ref(thread_end),
					std::ref(thread_count), std::ref(emsdata),
					std::ref(mbmsdata), std::ref(remote_first_run),
					set_source, scale_name, scale_data, scale_len,
					std::ref(modea), std::ref(soc_maxa),
					std::ref(soc_mina), std::ref(System_p_maxa),
					std::ref(System_p_mina), std::ref(System_q_maxa),
					std::ref(System_q_mina), std::ref(f_arra),
					std::ref(p_arra), std::ref(v_arra),
					std::ref(q_arra), std::ref(PQ_p_refa),
					std::ref(PQ_q_refa), std::ref(FP_typea),
					std::ref(FP_soc_goal_percenta),
					std::ref(FP_main_f_seta), std::ref(FP_line_p_basea),
					std::ref(FP_soc_dead_banda),
					std::ref(FP_soc_adjusta), std::ref(VC_typea),
					std::ref(Vq_v_basea), std::ref(Vq_q_basea),
					std::ref(Vq_droopa), std::ref(Vq_v_seta),
					std::ref(PF_pf_refa), std::ref(a1), std::ref(b1),
					std::ref(a21), std::ref(b21), std::ref(a22),
					std::ref(b22), std::ref(a23), std::ref(b23),
					std::ref(b3), std::ref(a41), std::ref(b41),
					std::ref(a42), std::ref(b42), std::ref(a43),
					std::ref(b43), std::ref(a5), std::ref(b5),
					std::ref(a_sReg), std::ref(b_sReg), a_qAuto,
					b_qAuto, std::ref(soc), std::ref(ems_count),
					std::ref(mbms_time), std::ref(mbms_count),
					std::ref(pcs_write_error_count),
					std::ref(pcs_reset), std::ref(pcs_control),std::ref(pcs_control_time_remote),
					pcs_read_ctx, std::ref(pcs_read_ret_), MBMS_eid,
					PCS_eid, db4, db4_local, std::ref(error_flag4));
		});
	while (true){
		Sleep(1000);
	}
	t_EMS_ReadData_thread->join();
	delete t_EMS_ReadData_thread;
	return 0;
}
