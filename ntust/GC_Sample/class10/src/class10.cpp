#include "dllgc.h"
#include <stdio.h>
#include <modbus/modbus.h>
#include <iostream>
#include <string>
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

#include <cmath>

#include <iomanip>
#include <vector>
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;
using bsoncxx::builder::basic::sub_array;
void modbus_find(modbus_t *ctx, int *fc, int &ret, bool &read_ok, int **arr,
		int *read_start, int *read_length, int order) {
	for (int y = 0; y < order; y++) {
		if (fc[y] == 3) {
			uint16_t table_16[read_length[y]];

			ret = modbus_read_registers(ctx, read_start[y], read_length[y],
					table_16);
			if (ret > 0) {
				for (int x = 0; x < read_length[y]; x++) {
					arr[y][x] = table_16[x];
//					printf("reg[%d]=%d (0x%X)\n", x, table_16[x], table_16[x]);
//					fflush(stdout);
				}
				read_ok = true;
			} else {
				std::cout << "read_start[y] :" << read_start[y]
						<< " read_length[y] :" << read_length[y] << std::endl;
				std::cout << "modbus dont find data" << std::endl;

				read_ok = false;
			}
		}
		if (fc[y] == 4) {
			uint16_t table_16[read_length[y]];

			ret = modbus_read_input_registers(ctx, read_start[y],
					read_length[y], table_16);
			if (ret > 0) {
				for (int x = 0; x < ret; x++) {
					arr[y][x] = table_16[x];
//					printf("reg[%d]=%d (0x%X)\n", x, table_16[x], table_16[x]);
//					fflush(stdout);
				}
				read_ok = true;
			} else {
				std::cout << "read_start[y] :" << read_start[y]
						<< " read_length[y] :" << read_length[y] << std::endl;
				std::cout << "modbus dont find data" << std::endl;
				read_ok = false;
			}
		}
	}
}
void Smooth_mode(double pv_p_now, double &total_p_per, double Smooth_p_one, double &pcs_p_now){
	try{
//		std::cout<<"Smooth_p_one :"<<Smooth_p_one<<std::endl;
		if (pv_p_now > total_p_per + Smooth_p_one){
			pcs_p_now = total_p_per + Smooth_p_one - pv_p_now;
		}
		else if (pv_p_now < total_p_per + Smooth_p_one){
			pcs_p_now = total_p_per - Smooth_p_one - pv_p_now;
		}
		else {
			pcs_p_now = 0;
		}
		total_p_per = pv_p_now + pcs_p_now;
	}
	catch(...){}
}

double Read_upload(auto point, int **modbus_data) {
	double return_data = 0;
	try {
//		std::cout<<modbus_data[point.order][point.length]<<std::endl;
//		std::cout<<return_data<<std::endl;
		if (point.type == "int") {
			for (int x = 0; x < point.length; x++) {
//				std::cout<<modbus_data[point.order][point.Address+x]<<std::endl;
				return_data = ((uint32_t) return_data << 16)
						+ modbus_data[point.order][point.Address + x];
			}
			if (point.length == 1) {
				uint16_t xorbit = 65535; /*256**2 -1*/
				uint16_t xorbit_test = 32768;/*(256**2)>>1*/
				if ((uint16_t) return_data
						> ((uint16_t) return_data ^ xorbit_test)) {
					return_data = -(((uint16_t) return_data ^ xorbit) + 1);
				}

			} else if (point.length == 2) {
				uint32_t xorbit = 4294967295; /*256**^4 -1*/
				uint32_t xorbit_test = 2147483648;/*(256^4)>>1*/
				if ((uint32_t) return_data
						> ((uint32_t) return_data ^ xorbit_test)) {
					return_data = -(((uint32_t) return_data ^ xorbit) + 1);
				}
			}
		} else if (point.type == "uint") {
			for (int x = 0; x < point.length; x++) {
//				std::cout<<modbus_data[point.order][point.Address+x]<<std::endl;
				return_data = ((uint32_t) return_data << 16)
						+ modbus_data[point.order][point.Address + x];
			}
		} else if (point.type == "bit") {/*顯示用*/
			for (int x = 0; x < point.length; x++) {
//				std::cout<<modbus_data[point.order][point.Address+x]<<std::endl;
				return_data = ((uint32_t) return_data << 16)
						+ modbus_data[point.order][point.Address + x];
			}
		} else if (point.type == "float") {
			float temp;
			char *Modbus_HoldReg[4];
			Modbus_HoldReg[0] = ((char*) (&temp)) + 3;
			Modbus_HoldReg[1] = ((char*) (&temp)) + 2;
			Modbus_HoldReg[2] = ((char*) (&temp)) + 1;
			Modbus_HoldReg[3] = ((char*) (&temp)) + 0;
			*Modbus_HoldReg[0] = (modbus_data[point.order][point.Address] >> 8)
					& 0xff;
			*Modbus_HoldReg[1] = modbus_data[point.order][point.Address] & 0xff;
			*Modbus_HoldReg[2] = (modbus_data[point.order][point.Address + 1]
					>> 8) & 0xff;
			*Modbus_HoldReg[3] = modbus_data[point.order][point.Address + 1]
					& 0xff;
			return_data = (double) (temp * 10000) / 10000;
//			std::cout<<"return_data :"<<return_data<<std::endl;

		} else {
		}

		if (return_data != 0 and point.scale != 0) {
			return_data = return_data / point.scale;
		}
	} catch (...) {
	}
	return return_data;
}
void Read_upload_bit(auto point, int **modbus_data, int *bin16) {
	try {
		if (point.type == "bit") {
			int return_data = modbus_data[point.order][point.Address];
			for (int x = 0; x < 16; x++) {
				if (return_data != 1) {
					bin16[x] = return_data % 2;
					return_data = return_data / 2;
				} else {
					bin16[x] = 0;
				}
			}
		} else {
		}
	} catch (...) {
	}
}
void control_thread(bool &function_run, int &thread_count,
		bool remote_first_run, std::string set_source, double pcs_connect_local,
		double pcs_reset_local,  double pcs_control_time_local, double *HMdata, bool &next_sec,
		std::string &ability_test, int &ability_test_count,
		int &lock_buffer_100ms, int &HM_error_count, bool &first_run,
		char &path_dReg, char &path_sReg, bool &sReg_flag,
		double &pcs_out_pre_p, double &pcs_out_pre_q, double *HM_time,
		double **HM_data, int ems_count, int mbms_count, double soc,
		modbus_t *HM_ctx, int &HM_ret, bool &modbus_link_HM_flag, double HM_f,
		double HM_p, double HM_v, double HM_q, double &pcs_out_p,
		double PQ_p_ref, double &pcs_out_q, double PQ_q_ref, int mode,
		double soc_max, double soc_min, double System_p_max,
		double System_p_min, double System_q_max, double System_q_min,
		double *f_arr, double *p_arr, double *v_arr, double *q_arr, int FP_type,
		int VC_type, double FP_soc_goal_percent, double FP_soc_dead_band,
		double PF_pf_ref, double Vq_v_base, double Vq_q_base, double Vq_droop,
		double Vq_v_set, double a1, double b1, double a21, double b21,
		double a22, double b22, double a23, double b23, double b3, double a41,
		double b41, double a42, double b42, double a43, double b43, double a5,
		double b5, double a_sReg, double b_sReg, double a_qAuto, double b_qAuto,
		bool &VQ_Hys_line, double &HM_v_last, std::string &control_point,
		std::string HM_eid, mongocxx::database db, mongocxx::database db_local,
		bool &error_flag, double *pcsdata,
		double &pcs_reset, double &pcs_control, double &control_flag, double &pcs_control_time_remote, double &pcs_control_time,
		int &pcs_read_error_count, int **pcs_read_arr, std::string pcs_ip,
		int pcs_port, int pcs_id, modbus_t *pcs_read_ctx, int &pcs_read_ret,
		bool &modbus_link_pcs_read_flag, std::string PCS_eid) {
	thread_count++;
	std::string fc_name = "control_thread";
	try {
		if (function_run == false) { /*是否已有線呈進入*/
			function_run = true;
//			std::cout <<"control_thread runing"<< std::endl;
			int pcs_write_buffer;
			/*1*/
			int xorbit = 65535; /*256的2次方 - 1*/
			/*2*//*
			 int xorbit=4294967295; *//*256的4次方 - 1*/

			time_t now = time(0);
			now += 28800;
			ModbusData::Meter_ION9000 Meter_ION9000;
			ModbusData::Meter_ION9000::i_a i_a;
			ModbusData::Meter_ION9000::i_b i_b;
			ModbusData::Meter_ION9000::i_c i_c;
			ModbusData::Meter_ION9000::i i;
			ModbusData::Meter_ION9000::f f;
			ModbusData::Meter_ION9000::vl_ab vl_ab;
			ModbusData::Meter_ION9000::vl_bc vl_bc;
			ModbusData::Meter_ION9000::vl_ca vl_ca;
			ModbusData::Meter_ION9000::vl vl;
			ModbusData::Meter_ION9000::v_a v_a;
			ModbusData::Meter_ION9000::v_b v_b;
			ModbusData::Meter_ION9000::v_c v_c;
			ModbusData::Meter_ION9000::v v;
			ModbusData::Meter_ION9000::p_a p_a;
			ModbusData::Meter_ION9000::p_b p_b;
			ModbusData::Meter_ION9000::p_c p_c;
			ModbusData::Meter_ION9000::p p;
			ModbusData::Meter_ION9000::q_a q_a;
			ModbusData::Meter_ION9000::q_b q_b;
			ModbusData::Meter_ION9000::q_c q_c;
			ModbusData::Meter_ION9000::q q;
			ModbusData::Meter_ION9000::s_a s_a;
			ModbusData::Meter_ION9000::s_b s_b;
			ModbusData::Meter_ION9000::s_c s_c;
			ModbusData::Meter_ION9000::s s;
			ModbusData::Meter_ION9000::pf_a pf_a;
			ModbusData::Meter_ION9000::pf_b pf_b;
			ModbusData::Meter_ION9000::pf_c pf_c;
			ModbusData::Meter_ION9000::pf pf;
			ModbusData::Meter_ION9000::imp_kwh imp_kwh;
			ModbusData::Meter_ION9000::exp_kwh exp_kwh;
			ModbusData::Meter_ION9000::tot_kwh tot_kwh;
			ModbusData::Meter_ION9000::net_kwh net_kwh;
			ModbusData::Meter_ION9000::imp_kvarh imp_kvarh;
			ModbusData::Meter_ION9000::exp_kvarh exp_kvarh;
			ModbusData::Meter_ION9000::tot_kvarh tot_kvarh;
			ModbusData::Meter_ION9000::net_kvarh net_kvarh;
			ModbusData::Meter_ION9000::imp_kvah imp_kvah;
			ModbusData::Meter_ION9000::exp_kvah exp_kvah;
			ModbusData::Meter_ION9000::tot_kvah tot_kvah;
			ModbusData::Meter_ION9000::net_kvah net_kvah;

//			long usage = memChecker::get_maxMem_kb() ;
//			std::cout<<usage<<std::endl ;
			/*modbus 讀取高壓表*/
//			std::cout << "modbus read HM start" << std::endl;
			try {
				if (HM_ret == -1) {
//					std::cout << "HM_ret change" << std::endl;
					modbus_link_HM_flag = false;
				}
//				std::cout<<"HM_ret :"<<HM_ret<<std::endl;
				bool HM_read_ok = false;
				int order_len = Meter_ION9000.order;
				int HM_arr_1[order_len][80];
				int *HM_arr[order_len];
				for (int x = 0; x < order_len; x++) {
					HM_arr[x] = &HM_arr_1[x][0];
				}

				HM_time[lock_buffer_100ms] = now;
				if (modbus_link_HM_flag == true) {
					modbus_find(HM_ctx, Meter_ION9000.fc, HM_ret, HM_read_ok,
							HM_arr, Meter_ION9000.Address, Meter_ION9000.Length,
							Meter_ION9000.order);
//					std::cout << HM_time[lock_buffer_100ms] << std::endl;
					if (HM_read_ok == true) {/*高壓錶讀取成功*/
						HM_error_count = 0;
//						std::cout <<"HM_arr:" << HM_arr[0][0] << std::endl;
					} else {/*高壓錶讀失敗*/
//						std::cout << "modbus HM dont find HM_ret : " << HM_ret
//								<< " HM_error_count : " << HM_error_count
//								<< std::endl;
						HM_error_count++;
					}
					HM_data[0][lock_buffer_100ms] = Read_upload(i_a, HM_arr);
					HM_data[1][lock_buffer_100ms] = Read_upload(i_b, HM_arr);
					HM_data[2][lock_buffer_100ms] = Read_upload(i_c, HM_arr);
					HM_data[3][lock_buffer_100ms] = Read_upload(i, HM_arr);
					HM_data[4][lock_buffer_100ms] = Read_upload(f, HM_arr);
					HM_data[5][lock_buffer_100ms] = Read_upload(vl_ab, HM_arr);
					HM_data[6][lock_buffer_100ms] = Read_upload(vl_bc, HM_arr);
					HM_data[7][lock_buffer_100ms] = Read_upload(vl_ca, HM_arr);
					HM_data[8][lock_buffer_100ms] = Read_upload(vl, HM_arr);
					HM_data[9][lock_buffer_100ms] = Read_upload(v_a, HM_arr);
					HM_data[10][lock_buffer_100ms] = Read_upload(v_b, HM_arr);
					HM_data[11][lock_buffer_100ms] = Read_upload(v_c, HM_arr);
					HM_data[12][lock_buffer_100ms] = Read_upload(v, HM_arr);
					HM_data[13][lock_buffer_100ms] = Read_upload(p_a, HM_arr);
					HM_data[14][lock_buffer_100ms] = Read_upload(p_b, HM_arr);
					HM_data[15][lock_buffer_100ms] = Read_upload(p_c, HM_arr);
					HM_data[16][lock_buffer_100ms] = Read_upload(p, HM_arr);
					HM_data[17][lock_buffer_100ms] = Read_upload(q_a, HM_arr);
					HM_data[18][lock_buffer_100ms] = Read_upload(q_b, HM_arr);
					HM_data[19][lock_buffer_100ms] = Read_upload(q_c, HM_arr);
					HM_data[20][lock_buffer_100ms] = Read_upload(q, HM_arr);
					HM_data[21][lock_buffer_100ms] = Read_upload(s_a, HM_arr);
					HM_data[22][lock_buffer_100ms] = Read_upload(s_b, HM_arr);
					HM_data[23][lock_buffer_100ms] = Read_upload(s_c, HM_arr);
					HM_data[24][lock_buffer_100ms] = Read_upload(s, HM_arr);
					HM_data[25][lock_buffer_100ms] = Read_upload(pf_a, HM_arr);
					HM_data[26][lock_buffer_100ms] = Read_upload(pf_b, HM_arr);
					HM_data[27][lock_buffer_100ms] = Read_upload(pf_c, HM_arr);
					HM_data[28][lock_buffer_100ms] = Read_upload(pf, HM_arr);
					HM_data[29][lock_buffer_100ms] = Read_upload(imp_kwh,
							HM_arr);
					HM_data[30][lock_buffer_100ms] = Read_upload(exp_kwh,
							HM_arr);
					HM_data[31][lock_buffer_100ms] = Read_upload(tot_kwh,
							HM_arr);
					HM_data[32][lock_buffer_100ms] = Read_upload(net_kwh,
							HM_arr);
					HM_data[33][lock_buffer_100ms] = Read_upload(imp_kvarh,
							HM_arr);
					HM_data[34][lock_buffer_100ms] = Read_upload(exp_kvarh,
							HM_arr);
					HM_data[35][lock_buffer_100ms] = Read_upload(tot_kvarh,
							HM_arr);
					HM_data[36][lock_buffer_100ms] = Read_upload(net_kvarh,
							HM_arr);
					HM_data[37][lock_buffer_100ms] = Read_upload(imp_kvah,
							HM_arr);
					HM_data[38][lock_buffer_100ms] = Read_upload(exp_kvah,
							HM_arr);
					HM_data[39][lock_buffer_100ms] = Read_upload(tot_kvah,
							HM_arr);
					HM_data[40][lock_buffer_100ms] = Read_upload(net_kvah,
							HM_arr);

				} else if (modbus_link_HM_flag == false) {/*無法連線等待另一個 線呈連線成功*/
//					std::cout << "HM dont link wait" << std::endl;
					if (HM_ret == 0) {/*連線成功*/
						modbus_link_HM_flag = true;
					} else {
						HM_error_count++;
					}
				}
				if (HM_read_ok == true) {/*高壓錶讀取成功*/
					HM_f = Read_upload(f, HM_arr);
					HM_p = Read_upload(p, HM_arr);
					HM_v = Read_upload(v, HM_arr);
					HM_q = Read_upload(q, HM_arr);
//					std::cout <<"11111111111111111"<< std::endl;
//					std::cout <<"data HM_read_ok end"<< std::endl;
					/*回傳ui*/
					for (int x = 0; x < 41; x++) {
						HMdata[x] = HM_data[x][lock_buffer_100ms];
					}
				} else if (HM_read_ok == false and HM_error_count < 30) {/*高壓錶讀失敗 <30*/
					int pro_order;
					if (lock_buffer_100ms == 0) {
						pro_order = 9;
					} else {
						pro_order = lock_buffer_100ms - 1;
					}
					for (int x = 0; x < 41; x++) {
						HM_data[x][lock_buffer_100ms] = HM_data[x][pro_order];
					}
					HM_f = HM_data[4][pro_order];
					HM_p = HM_data[16][pro_order];
					HM_v = HM_data[12][pro_order];
					HM_q = HM_data[20][pro_order];
//					std::cout <<"22222222222222222222222"<< std::endl;
					/*回傳ui*/
					for (int x = 0; x < 41; x++) {
						HMdata[x] = HM_data[x][lock_buffer_100ms];
					}
				} else {
					for (int x = 0; x < 41; x++) {
						HM_data[x][lock_buffer_100ms] = -1;
					}
//					std::cout <<"333333333333333"<< std::endl;
					/*回傳ui*/
					for (int x = 0; x < 41; x++) {
						HMdata[x] = -1;
					}
					HM_f = -1;
					HM_p = -1;
					HM_v = -1;
					HM_q = -1;
				}
//				for (int x = 0; x < Meter_ION9000.order; x++)
//					delete[] HM_arr[x];
//				delete[] HM_arr;
//				std::cout << "main run  " << "HM_ret :" << HM_ret
//						<< " modbus_link_HM_flag :" << modbus_link_HM_flag
//						<< std::endl;
			} catch (...) {
			}

//			std::cout << "read HM　end"<< std::endl;
			double pcs_out_p_now;/*此次P 0.1s*/
			double pcs_out_q_now;/*此次Q 0.1s*/
			if (next_sec == true) { /*換秒*/

				/*HM有讀到值*/
				/*沒讀到值30次 (又不是第一次)*/
//				std::cout << "next_sec == true"<< std::endl;
//				std::cout << "first_run :" << first_run << std::endl;
//				std::cout << "ems_count :" << ems_count <<" mbms_count :" << mbms_count << std::endl;
				if ((HM_ret > 0
						or (HM_ret < 0 and HM_error_count < 30
								and first_run == false))
						and (ems_count < 60 and mbms_count < 60)) { /*3秒內讀不到還是會進*/
//					std::cout << "read High meter OK"<< std::endl;
//					std::cout << "HM_f :" << HM_f << " HM_p :" << HM_p
//							<< " HM_v :" << HM_v << " HM_q :" << HM_q
//							<< std::endl;
					/*模式控制*/
//					std::cout << "mode :" << mode << "  FP_type :" << FP_type
//							<< "  VC_type :" << VC_type << std::endl;
					if (mode == 0) {
						pcs_out_p = 0;
						pcs_out_q = 0;
					}  else {
						std::cout << "mode dont find" << std::endl;
					}
//					std::cout << "pcs_out_p :"<<pcs_out_p<< "  pcs_out_q :"<<pcs_out_q<<"  soc :"<<soc<< std::endl;
					/*極限值計算*/
					if (pcs_out_p > System_p_max) {
						pcs_out_p = System_p_max;
					} else if (pcs_out_p < System_p_min) {
						pcs_out_p = System_p_min;
					}
					if (pcs_out_q > System_q_max) {
						pcs_out_q = System_q_max;
					} else if (pcs_out_q < System_q_min) {
						pcs_out_q = System_q_min;
					}
					if ((soc > soc_max and pcs_out_p < 0)
							or (soc < soc_min and pcs_out_p > 0)) {
						pcs_out_p = 0;
					}
//					std::cout << "pcs_out_p :"<<pcs_out_p<< "  pcs_out_q :"<<pcs_out_q<< std::endl;

					first_run = false;
				}
				/*沒讀到值(EMS發告警)(應對PCS下指令停止)*/
				else {
//					std::cout << "dont read High meter" << std::endl;
					pcs_out_p = 0;
					HM_f = 0;
					if (first_run == true) {
						std::cout << "first_run dont read High meter"
								<< std::endl;
					}
				}
				pcs_out_p_now = pcs_out_p;
				pcs_out_q_now = pcs_out_q;
				next_sec = false;
			} else { /*沒換秒 修正讀取值和前一次控制值差值(損失補償)*/
//				std::cout << "pcs_out_pre_p :" << std::to_string(pcs_out_pre_p)<< std::endl;
//				std::cout << "pcs_out_p :" << std::to_string(pcs_out_p)<< std::endl;
//				std::cout << "HM_p :" << std::to_string(HM_p)<< std::endl;

				if (mode == 5 or mode == 11) {
					pcs_out_p_now = pcs_out_pre_p + (pcs_out_p - HM_p);
					pcs_out_q_now = pcs_out_pre_q + (pcs_out_q - HM_q);
				} else {
					pcs_out_p_now = pcs_out_pre_p;
					pcs_out_q_now = pcs_out_pre_q;
				}
				/*先做只有一台的*/
			}
			/*控制PCS輸出*/
			if (set_source == "Local"
					or (set_source == "Remote" and remote_first_run == false)) {/*要讀到才能控制*/
				/*vvvvvvvvvvvvvvvvv暫時寫死資料vvvvvvvvvvvvvvvvvvv*/
				int pcs_start_p = 10; /*P寫入*/
				int pcs_len_p = 1;
				int pcs_start_q = 11; /*Q寫入*/
				int pcs_len_q = 1;

				double pcs_scale_p = 10.0;
				double pcs_scale_q = 10.0;
				/*^^^^^^^^^^^^^^^^^^^暫時寫死資料^^^^^^^^^^^^^^^^^^^*/
				uint16_t *pcs_write_arr_p = new uint16_t[pcs_len_p];
				uint16_t *pcs_write_arr_q = new uint16_t[pcs_len_q];

				/*modbus PCS 控制    P(pcs_out_p_now)*/
				pcs_write_buffer = int(pcs_out_p_now * pcs_scale_p);
				if (pcs_out_p_now >= 0) {
				} else {
					pcs_write_buffer = (abs(pcs_write_buffer) ^ xorbit) + 1;
				}
				/*1*/
				pcs_write_arr_p[0] = pcs_write_buffer;
				/*2*//*
				 pcs_write_arr_p[0] = pcs_write_buffer>>16;
				 pcs_write_arr_p[1] = pcs_write_buffer - ((pcs_write_buffer>>16)<<16);*/

				/*modbus PCS 控制    Q(pcs_out_q_now)*/
				pcs_write_buffer = int(pcs_out_q_now * pcs_scale_q);
				if (pcs_out_q_now >= 0) {
				} else {
					pcs_write_buffer = (abs(pcs_write_buffer) ^ xorbit) + 1;
				}
				/*1*/
				pcs_write_arr_q[0] = pcs_write_buffer;
				/*2*//*
				 pcs_write_arr_q[0] = pcs_write_buffer>>16;
				 pcs_write_arr_q[1] = pcs_write_buffer - ((pcs_write_buffer>>16)<<16);*/

//				std::cout << "pcs_write_arr_p[0] :"<<pcs_write_arr_p[0]<< "  pcs_write_arr_q[0] :"<<pcs_write_arr_q[0]<< std::endl;
//				std::cout << "pcs_start_p :"<<pcs_start_p<< "  pcs_len_p :"<<pcs_len_p<< std::endl;
//				if (modbus_write_register(pcs_write_ctx, pcs_start_p,
//						pcs_len_p, pcs_write_arr_p) != -1) {
				if (modbus_write_register(pcs_read_ctx, pcs_start_p,
						pcs_write_arr_p[0]) == 1) {
//				if (modbus_write_register(pcs_write_ctx, 10, 100) == 1) {
					/*p寫入成功*/
					pcs_out_pre_p = pcs_out_p_now;/*有寫入成功才能代表有輸出*/

				} else {
					/*p寫入失敗*/
					pcs_read_ret = -1;
				}
//				if (modbus_write_register(pcs_write_ctx, pcs_start_q,
//						pcs_len_q, pcs_write_arr_q) != -1) {
				if (modbus_write_register(pcs_read_ctx, pcs_start_q,
						pcs_write_arr_q[0]) == 1) {
					/*q寫入成功*/
					pcs_out_pre_q = pcs_out_q_now;/*有寫入成功才能代表有輸出*/
				} else {/*q寫入失敗*/
					pcs_read_ret = -1;
				}
				delete pcs_write_arr_p;
				delete pcs_write_arr_q;
//				std::cout <<pcs_write_buffer<<"[0] :"<< std::to_string(pcs_write_arr_p[0])<<" [1] :"<<std::to_string(pcs_write_arr_p[1])<< std::endl;
//
//				std::cout<< "pcs_out_p :" << std::to_string(pcs_out_p)
//						<< " pcs_out_p_now :" << std::to_string(pcs_out_p_now)
//						<< " HM_p :" << std::to_string(HM_p)
//						<< " pcs_out_q_now :" << std::to_string(pcs_out_q_now)
//						<< "  f :" << std::to_string(HM_f)
//						<< "  control_point :" << control_point << std::endl;
//				oFile << std::to_string(pcs_out_p) << "," << std::to_string(HM_f)<< "," <<control_point << std::endl;
			}

			if (lock_buffer_100ms == 9) {
//				std::cout << "insert HM data to mongo" << std::endl;
				std::vector<bsoncxx::document::value> HM_docs;
				std::vector<bsoncxx::document::value> HM_docs_one;
				double HM_time_array[10];
				double HM_v_a_array[10];
				double HM_v_b_array[10];
				double HM_v_c_array[10];
				double HM_i_a_array[10];
				double HM_i_b_array[10];
				double HM_i_c_array[10];
				double HM_f_array[10];
				double HM_p_array[10];
				double HM_q_array[10];
				double HM_pf_array[10];
				for (int y = 0; y < 10; y++) {
					HM_time_array[y] = HM_time[y];
					HM_v_a_array[y] = HM_data[9][y];
					HM_v_b_array[y] = HM_data[10][y];
					HM_v_c_array[y] = HM_data[11][y];
					HM_i_a_array[y] = HM_data[0][y];
					HM_i_b_array[y] = HM_data[1][y];
					HM_i_c_array[y] = HM_data[2][y];
					if (ability_test != "None") {
						HM_data[4][y] = HM_data[4][0];
					}
					HM_f_array[y] = HM_data[4][y];
					HM_p_array[y] = HM_data[16][y];
					HM_q_array[y] = HM_data[20][y];
					HM_pf_array[y] = HM_data[28][y];
				}

				for (int y = 0; y < 10; y++) {
					if ((HM_data[0][y] != -1 or HM_data[3][y] != -1
							or HM_data[4][y] != -1) and (first_run == false)) {
//						std::cout << std::to_string(HM_time[y]) << std::endl;
//						std::cout << HM_data[0][y] << std::endl;
//						std::cout << HM_data[3][y] << std::endl;
//						std::cout << HM_data[4][y] << std::endl;
						HM_docs.push_back(
								make_document(kvp("ID", HM_eid),
										kvp("time",
												bsoncxx::types::b_date {
														std::chrono::system_clock::from_time_t(
																HM_time[y])
																+ std::chrono::milliseconds(
																		y * 100) }),
										kvp("i_a", HM_data[0][y]),
										kvp("i_b", HM_data[1][y]),
										kvp("i_c", HM_data[2][y]),
										kvp("i", HM_data[3][y]),
										kvp("f", HM_data[4][y]),
										kvp("vl_ab", HM_data[5][y]),
										kvp("vl_bc", HM_data[6][y]),
										kvp("vl_ca", HM_data[7][y]),
										kvp("vl", HM_data[8][y]),
										kvp("v_a", HM_data[9][y]),
										kvp("v_b", HM_data[10][y]),
										kvp("v_c", HM_data[11][y]),
										kvp("v", HM_data[12][y]),
										kvp("p_a", HM_data[13][y]),
										kvp("p_b", HM_data[14][y]),
										kvp("p_c", HM_data[15][y]),
										kvp("p", HM_data[16][y]),
										kvp("q_a", HM_data[17][y]),
										kvp("q_b", HM_data[18][y]),
										kvp("q_c", HM_data[19][y]),
										kvp("q", HM_data[20][y]),
										kvp("s_a", HM_data[21][y]),
										kvp("s_b", HM_data[22][y]),
										kvp("s_c", HM_data[23][y]),
										kvp("s", HM_data[24][y]),
										kvp("pf_a", HM_data[25][y]),
										kvp("pf_b", HM_data[26][y]),
										kvp("pf_c", HM_data[27][y]),
										kvp("pf", HM_data[28][y]),
										kvp("imp_kwh", HM_data[29][y]),
										kvp("exp_kwh", HM_data[30][y]),
										kvp("tot_kwh", HM_data[31][y]),
										kvp("net_kwh", HM_data[32][y]),
										kvp("imp_kvarh", HM_data[33][y]),
										kvp("exp_kvarh", HM_data[34][y]),
										kvp("tot_kvarh", HM_data[35][y]),
										kvp("net_kvarh", HM_data[36][y]),
										kvp("imp_kvah", HM_data[37][y]),
										kvp("exp_kvah", HM_data[38][y]),
										kvp("tot_kvah", HM_data[39][y]),
										kvp("net_kvah", HM_data[40][y])));
					} else {
//						std::cout << "sssssssssssssssss" << std::endl;
//						std::cout << std::to_string(HM_time[y]) << std::endl;
						HM_docs.push_back(
								make_document(kvp("ID", HM_eid),
										kvp("time",
												bsoncxx::types::b_date {
														std::chrono::system_clock::from_time_t(
																HM_time[y])
																+ std::chrono::milliseconds(
																		y * 100) }),
										kvp("i_a", bsoncxx::types::b_null { }),
										kvp("i_b", bsoncxx::types::b_null { }),
										kvp("i_c", bsoncxx::types::b_null { }),
										kvp("i", bsoncxx::types::b_null { }),
										kvp("f", bsoncxx::types::b_null { }),
										kvp("vl_ab",
												bsoncxx::types::b_null { }),
										kvp("vl_bc",
												bsoncxx::types::b_null { }),
										kvp("vl_ca",
												bsoncxx::types::b_null { }),
										kvp("vl", bsoncxx::types::b_null { }),
										kvp("v_a", bsoncxx::types::b_null { }),
										kvp("v_b", bsoncxx::types::b_null { }),
										kvp("v_c", bsoncxx::types::b_null { }),
										kvp("v", bsoncxx::types::b_null { }),
										kvp("p_a", bsoncxx::types::b_null { }),
										kvp("p_b", bsoncxx::types::b_null { }),
										kvp("p_c", bsoncxx::types::b_null { }),
										kvp("p", bsoncxx::types::b_null { }),
										kvp("q_a", bsoncxx::types::b_null { }),
										kvp("q_b", bsoncxx::types::b_null { }),
										kvp("q_c", bsoncxx::types::b_null { }),
										kvp("q", bsoncxx::types::b_null { }),
										kvp("s_a", bsoncxx::types::b_null { }),
										kvp("s_b", bsoncxx::types::b_null { }),
										kvp("s_c", bsoncxx::types::b_null { }),
										kvp("s", bsoncxx::types::b_null { }),
										kvp("pf_a", bsoncxx::types::b_null { }),
										kvp("pf_b", bsoncxx::types::b_null { }),
										kvp("pf_c", bsoncxx::types::b_null { }),
										kvp("pf", bsoncxx::types::b_null { }),
										kvp("imp_kwh",
												bsoncxx::types::b_null { }),
										kvp("exp_kwh",
												bsoncxx::types::b_null { }),
										kvp("tot_kwh",
												bsoncxx::types::b_null { }),
										kvp("net_kwh",
												bsoncxx::types::b_null { }),
										kvp("imp_kvarh",
												bsoncxx::types::b_null { }),
										kvp("exp_kvarh",
												bsoncxx::types::b_null { }),
										kvp("tot_kvarh",
												bsoncxx::types::b_null { }),
										kvp("net_kvarh",
												bsoncxx::types::b_null { }),
										kvp("imp_kvah",
												bsoncxx::types::b_null { }),
										kvp("exp_kvah",
												bsoncxx::types::b_null { }),
										kvp("tot_kvah",
												bsoncxx::types::b_null { }),
										kvp("net_kvah",
												bsoncxx::types::b_null { })));
					}
				}
				try{
					db["acm"].insert_many(HM_docs);
				}catch(...){}
				try{
					db_local["acm"].insert_many(HM_docs);
				}catch(...){}
				bool check_arr[10];
				for (int y = 0; y < 10; y++) {
					if (HM_data[0][y] != -1 or HM_data[3][y] != -1
							or HM_data[4][y] != -1) {
						check_arr[y] = true;
					} else {
						check_arr[y] = false;
					}
				}
				if ((HM_data[0][0] != -1 or HM_data[3][0] != -1
						or HM_data[4][0] != -1) and (first_run == false)) {
					HM_docs_one.push_back(
							make_document(kvp("ID", HM_eid),
									kvp("time",
											[&HM_time_array](sub_array child) {
												int y = 0;
												for (const auto &element : HM_time_array) {
													child.append(
															bsoncxx::types::b_date {
																	std::chrono::system_clock::from_time_t(
																			element)
																			+ std::chrono::milliseconds(
																					y
																							* 100) });
													y++;
												}
											})
									,
									kvp("v_a",
											[&HM_v_a_array](sub_array child) {
												for (const double &element : HM_v_a_array) {
													child.append(element);
												}
											})
									,
									kvp("v_b",
											[&HM_v_b_array](sub_array child) {
												for (const double &element : HM_v_b_array) {
													child.append(element);
												}
											})
									,
									kvp("v_c",
											[&HM_v_c_array](sub_array child) {
												for (const double &element : HM_v_c_array) {
													child.append(element);
												}
											})
									,
									kvp("i_a",
											[&HM_i_a_array](sub_array child) {
												for (const double &element : HM_i_a_array) {
													child.append(element);
												}
											})
									,
									kvp("i_b",
											[&HM_i_b_array](sub_array child) {
												for (const double &element : HM_i_b_array) {
													child.append(element);
												}
											})
									,
									kvp("i_c",
											[&HM_i_c_array](sub_array child) {
												for (const double &element : HM_i_c_array) {
													child.append(element);
												}
											})
									,
									kvp("f",
											[&HM_f_array](sub_array child) {
												for (const double &element : HM_f_array) {
													child.append(element);
												}
											})
									,
									kvp("p",
											[&HM_p_array](sub_array child) {
												for (const double &element : HM_p_array) {
													child.append(element);
												}
											})
									,
									kvp("q",
											[&HM_q_array](sub_array child) {
												for (const double &element : HM_q_array) {
													child.append(element);
												}
											})
									,
									kvp("pf",
											[&HM_pf_array](sub_array child) {
												for (const double &element : HM_pf_array) {
													child.append(element);
												}
											})
							));
				} else {
					HM_docs_one.push_back(
							make_document(kvp("ID", HM_eid),
									kvp("time",
											[&HM_time_array](sub_array child) {
												int y = 0;
												for (const auto &element : HM_time_array) {
													child.append(
															bsoncxx::types::b_date {
																	std::chrono::system_clock::from_time_t(
																			element)
																			+ std::chrono::milliseconds(
																					y
																							* 100) });
													y++;
												}
											})
									,
									kvp("v_a",
											[&HM_v_a_array](sub_array child) {
												for (const double &element : HM_v_a_array) {
													child.append(
															bsoncxx::types::b_null { });
												}
											})
									,
									kvp("v_b",
											[&HM_v_b_array](sub_array child) {
												for (const double &element : HM_v_b_array) {
													child.append(
															bsoncxx::types::b_null { });
												}
											})
									,
									kvp("v_c",
											[&HM_v_c_array](sub_array child) {
												for (const double &element : HM_v_c_array) {
													child.append(
															bsoncxx::types::b_null { });
												}
											})
									,
									kvp("i_a",
											[&HM_i_a_array](sub_array child) {
												for (const double &element : HM_i_a_array) {
													child.append(
															bsoncxx::types::b_null { });
												}
											})
									,
									kvp("i_b",
											[&HM_i_b_array](sub_array child) {
												for (const double &element : HM_i_b_array) {
													child.append(
															bsoncxx::types::b_null { });
												}
											})
									,
									kvp("i_c",
											[&HM_i_c_array](sub_array child) {
												for (const double &element : HM_i_c_array) {
													child.append(
															bsoncxx::types::b_null { });
												}
											})
									,
									kvp("f",
											[&HM_f_array](sub_array child) {
												for (const double &element : HM_f_array) {
													child.append(
															bsoncxx::types::b_null { });
												}
											})
									,
									kvp("p",
											[&HM_p_array](sub_array child) {
												for (const double &element : HM_p_array) {
													child.append(
															bsoncxx::types::b_null { });
												}
											})
									,
									kvp("q",
											[&HM_q_array](sub_array child) {
												for (const double &element : HM_q_array) {
													child.append(
															bsoncxx::types::b_null { });
												}
											})
									,
									kvp("pf",
											[&HM_pf_array](sub_array child) {
												for (const double &element : HM_pf_array) {
													child.append(
															bsoncxx::types::b_null { });
												}
											})
							));
				}
				try{
					db["TPC"].insert_many(HM_docs_one);
				}catch(...){}
				try{
					db_local["TPC"].insert_many(HM_docs_one);
				}catch(...){}

			}
//			std::cout << "lock_buffer_100ms :" <<lock_buffer_100ms<< std::endl;
			if (lock_buffer_100ms == 0) {
//			if (true) {
//				PCS_ReadData_thread(pcsdata, set_source, pcs_connect_local, pcs_reset_local, pcs_reset,
//						pcs_control,control_flag,pcs_control_time_local, pcs_control_time_remote, pcs_control_time, pcs_read_error_count,
//						pcs_read_arr, pcs_ip, pcs_port, pcs_id, pcs_read_ctx,
//						pcs_read_ret, modbus_link_pcs_read_flag, PCS_eid, db,
//						db_local);
			}

			function_run = false;
//			std::cout << "control_thread end " << std::endl;
		} else { /*已經有程式在運行(上次超過0.1s)*/
			std::cout << " control_thread over 0.1s " << std::endl;
			for (int x = 0; x < 41; x++) {
				HM_data[x][lock_buffer_100ms] = -1;
			}
		}
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
		function_run = false;
	}
	thread_count--;
}

int main() {

	return 0;
}
