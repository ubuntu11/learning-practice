/*
 * dllgc.cpp
 *
 *  Created on: 2021年12月13日
 *      Author: XU
 */

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
#include <fstream>
#include <cmath>
#include "timercpp.h"
#include <iomanip>
#include <vector>
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;
using bsoncxx::builder::basic::sub_array;

dllgc::dllgc() {
	mongocxx::instance::current();
}
dllgc::~dllgc() {
}
void dllgc::print() {
	printf("AFC_GC.dll1\n");
}
typedef struct CPUPACKED
{
	char name[20];
	unsigned int user;
	unsigned int nice;
	unsigned int system;
	unsigned int idle;
	unsigned int lowait;
	unsigned int irq;
	unsigned int softirq;
} CPU_OCCUPY;
double GetTotalPhysicalMemoryUsed() {/*memory GB*/
	MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memInfo);
	double physMemUsed = (((memInfo.ullTotalPhys - memInfo.ullAvailPhys) / 1024
			/ 1024) * 100) / 1024;
	double fTemMemUsed = physMemUsed / 100.0;
	return fTemMemUsed;
}
double GetPhysicalMemoryUsage() {/*memory %*/
	MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memInfo);
	double MemUsage = memInfo.dwMemoryLoad;
	return MemUsage;
}
__int64 CompareFileTime(FILETIME time1, FILETIME time2) {
	__int64 a = time1.dwHighDateTime << 32 | time1.dwLowDateTime;
	__int64 b = time2.dwHighDateTime << 32 | time2.dwLowDateTime;

	return (b - a);
}
double getWin_CpuUsage() {
	HANDLE hEvent;
	BOOL res;
	FILETIME preidleTime;
	FILETIME prekernelTime;
	FILETIME preuserTime;
	FILETIME idleTime;
	FILETIME kernelTime;
	FILETIME userTime;

	res = GetSystemTimes(&idleTime, &kernelTime, &userTime);
	preidleTime = idleTime;
	prekernelTime = kernelTime;
	preuserTime = userTime;

	hEvent = CreateEventA(NULL, FALSE, FALSE, NULL);
	WaitForSingleObject(hEvent, 300);
	res = GetSystemTimes(&idleTime, &kernelTime, &userTime);

	__int64 idle = CompareFileTime(preidleTime, idleTime);
	__int64 kernel = CompareFileTime(prekernelTime, kernelTime);
	__int64 user = CompareFileTime(preuserTime, userTime);

	__int64 cpu = (kernel + user - idle) * 100 / (kernel + user);
//	__int64 cpuidle = (idle) * 100 / (kernel + user);
//	cout << "CPU_user:" << cpu << "%" << " CPU_space:" << cpuidle << "%" << endl;
	preidleTime = idleTime;
	prekernelTime = kernelTime;
	preuserTime = userTime;
	if (cpu > 100) {
		cpu = 100;
	}
	if (cpu < 0) {
		cpu = 0;
	}
	return cpu;
}
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
		Sleep(10);
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

void mongo_time(bsoncxx::v_noabi::document::view doc, std::string point_name,
		double &return_data) {
	try {
		bsoncxx::document::element bson_data = doc[point_name];
		if (bson_data.type() == bsoncxx::type::k_date) {
//			std::cout << bsoncxx::to_json(doc) << std::endl;
			return_data = bson_data.get_date();
//			std::cout<<"return_data :"<<point_name<<" = "<< std::to_string(return_data) <<std::endl;
		} else {
			/* Error*/
		}
	} catch (...) {
	}
}
void mongo_data(bsoncxx::v_noabi::document::view doc, std::string point_name,
		double &return_data) {
	try {
		bsoncxx::document::element bson_data = doc[point_name];
		if (bson_data.type() == bsoncxx::type::k_double) {
			return_data = bson_data.get_double().value;
//			std::cout <<"get_double return_data :"<< return_data << std::endl;
		} else if (bson_data.type() == bsoncxx::type::k_int32) {
			return_data = bson_data.get_int32().value;
//			std::cout <<"get_int32 return_data :"<< return_data << std::endl;
		} else {
			/* Error*/
			return_data = -1;
			std::cout << "else return_data :" << return_data << std::endl;
		}
	} catch (...) {
		return_data = -1;
	}
}
void mongo_dic(bsoncxx::v_noabi::document::view doc, std::string point_name,
		std::string *name, double *data, int &scale_len) {
	try {
		bsoncxx::document::element bson_data = doc[point_name];
		if (bson_data.type() == bsoncxx::type::k_document) {
			bsoncxx::document::view subdoc = bson_data.get_document().value;
			for (int x = 0; x < scale_len; x++) {
				bsoncxx::document::element data_buffer = subdoc[name[x]];
				data[x] = data_buffer.get_double().value;
			}
		} else {
			for (int x = 0; x < scale_len; x++) {
				data[x] = -1;
			}
		}
	} catch (...) {
		for (int x = 0; x < scale_len; x++) {
			data[x] = -1;
		}
	}
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
void HM_link_thread(bool &thread_end, int &thread_count, modbus_t *HM_ctx,
		int &HM_ret, bool &modbus_link_HM_flag, mongocxx::database db_local,
		bool &error_flag) {
	int lock_buffer_1s = int(currentTime_start_new.wSecond);
	int lock_buffer_100ms = int(currentTime_start_new.wMilliseconds / 100);
	std::string fc_name = "HM_link_thread";
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
//				std::cout << "modbus_link run ret: " << HM_ret << std::endl;
				if (modbus_link_HM_flag == false and HM_ret == -1) {
//					std::cout << "link_flag == false: " << HM_ret << std::endl;
					modbus_close(HM_ctx);
					HM_ret = modbus_connect(HM_ctx);
				}
				error_flag = false;
			} catch (...) {
				std::cout << "unknow error : " << fc_name << std::endl;
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
void error_thread(bool &thread_end, int &thread_count,
		int &pcs_read_error_count, int &HM_error_count, std::string HM_eid,
		std::string PCS_eid, mongocxx::database db, mongocxx::database db_local,
		bool &error_flag) {
	int lock_buffer_1s = int(currentTime_start_new.wSecond);
	int lock_buffer_100ms = int(currentTime_start_new.wMilliseconds / 100);
	std::string fc_name = "error_thread";
	bool pcs_error = false;
	bool HM_error = false;
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
				if (lock_buffer_1s % 10 == 0 or lock_buffer_1s % 10 == 5) {
					int iRunTime = GetTickCount();
					time_t nowtime = time(0);
					nowtime += 28800;
					time_t DateTime = nowtime - (iRunTime / 1000);
					CHAR cUserNameBuffer[256];
					DWORD dwUserNameSize = 256;
					if (GetUserName(cUserNameBuffer, &dwUserNameSize)) {
//						printf("The user name is %s \n", cUserNameBuffer);
					} else {
//						printf("Get user name failed with error: %lu \n", GetLastError());
					}
					double memory_data = GetTotalPhysicalMemoryUsed();
					double memory_per = GetPhysicalMemoryUsage();
					double cpu = getWin_CpuUsage();
//					std::cout<<"nowtime :"<<nowtime<<std::endl;
//					std::cout<<"user name :"<<cUserNameBuffer<<std::endl;
//					std::cout<<"memory (GB)"<<memory_data<<std::endl;
//					std::cout<<"memory (%)"<<memory_per<<std::endl;
//					std::cout<<"CPU (%)"<<cpu<<std::endl;
					std::vector<bsoncxx::document::value> Status;
					Status.push_back(
							make_document(kvp("ID", cUserNameBuffer),
									kvp("time",
											bsoncxx::types::b_date {
													std::chrono::system_clock::from_time_t(
															nowtime) }),
									kvp("boot_time",
											bsoncxx::types::b_date {
													std::chrono::system_clock::from_time_t(
															DateTime) }),
									kvp("cpu", cpu),
									kvp("memory",
											make_document(
													kvp("total", memory_data),
													kvp("mem_per",
															memory_per)))));
					try{
						db["pc_info"].insert_many(Status);
					}catch(...){}
				}
//				std::cout<<"pcs_read_error_count :"<<pcs_read_error_count<<std::endl;
				if (pcs_read_error_count < 1) {/*1s(1s)*/
				db["equipment"].update_one(
						make_document(kvp("_id", bsoncxx::oid(PCS_eid))),
						make_document(
								kvp("$set",
										make_document(kvp("count", 0)))));
					pcs_error = false;
				} else {
					if (pcs_error == false) {
						std::vector<bsoncxx::document::value> error_docs;
						time_t now = time(0);
						now += 28800;
						error_docs.push_back(
								make_document(kvp("ID", "GC"),
										kvp("time",
												bsoncxx::types::b_date {
														std::chrono::system_clock::from_time_t(
																now) }),
										kvp("event",
												"PCS meter dont read data"),
										kvp("group", fc_name), kvp("level", 1),
										kvp("show", 1)));
						try{
							db_local["alarm"].insert_many(error_docs);
						}catch(...){}
						pcs_error = true;
					}
				}
//				std::cout<<"HM_error_count :"<<HM_error_count<<std::endl;

				if (HM_error_count < 10) {/*1s(0.1s)*/
					db["equipment"].update_one(
							make_document(kvp("_id", bsoncxx::oid(HM_eid))),
							make_document(
									kvp("$set",
											make_document(kvp("count", 0)))));
					HM_error = false;
				} else {
					if (HM_error == false) {
						std::vector<bsoncxx::document::value> error_docs;
						time_t now = time(0);
						now += 28800;
						error_docs.push_back(
								make_document(kvp("ID", "GC"),
										kvp("time",
												bsoncxx::types::b_date {
														std::chrono::system_clock::from_time_t(
																now) }),
										kvp("event", "HV meter dont read data"),
										kvp("group", fc_name), kvp("level", 1),
										kvp("show", 1)));
						try{
							db_local["alarm"].insert_many(error_docs);
						}catch(...){}
						HM_error = true;
					}
				}
//				std::cout<<"HM_error_countXXX :"<<HM_error_count<<std::endl;
				error_flag = false;
			} catch (...) {
				std::cout << "unknow error : " << fc_name << std::endl;
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

void PCS_ReadData_thread(double *pcsdata, std::string set_source, double &pcs_connect_local, double &pcs_reset_local, double &pcs_reset,
		double &pcs_control, double &control_flag, double &pcs_control_time_local, double &pcs_control_time_remote, double &pcs_control_time, int &pcs_read_error_count,
		int **pcs_read_arr, std::string pcs_ip, int pcs_port, int pcs_id,
		modbus_t *pcs_read_ctx, int &pcs_read_ret,
		bool &modbus_link_pcs_read_flag, std::string PCS_eid,
		mongocxx::database db, mongocxx::database db_local) {
	try {
		bool function_run2 = false;
		if (function_run2 == false) {
			function_run2 = true;
			/*modbus 讀取PCS資訊*/
//			std::cout << "modbus read PCS start" << std::endl;
			time_t now = time(0);
			now += 28800;
			ModbusData::PCS_delta_1 PCS_delta;
			ModbusData::PCS_delta_1::status_pcs status_pcs;
			ModbusData::PCS_delta_1::state_qs state_qs;
			ModbusData::PCS_delta_1::p_sum p_sum;
			ModbusData::PCS_delta_1::q_sum q_sum;
			ModbusData::PCS_delta_1::v1_grid v1_grid;
			ModbusData::PCS_delta_1::v2_grid v2_grid;
			ModbusData::PCS_delta_1::v3_grid v3_grid;
			ModbusData::PCS_delta_1::i1_pcs i1_pcs;
			ModbusData::PCS_delta_1::i2_pcs i2_pcs;
			ModbusData::PCS_delta_1::i3_pcs i3_pcs;
			ModbusData::PCS_delta_1::f_grid f_grid;
			ModbusData::PCS_delta_1::inner_temp inner_temp;
			ModbusData::PCS_delta_1::sink_temp sink_temp;
			ModbusData::PCS_delta_1::v_dc v_dc;
			ModbusData::PCS_delta_1::i_dc i_dc;
			ModbusData::PCS_delta_1::v_bat v_bat;

			ModbusData::PCS_delta_1::dam_fault1 dam_fault1;
			ModbusData::PCS_delta_1::dam_fault2 dam_fault2;
			ModbusData::PCS_delta_1::dam_fault3 dam_fault3;
			ModbusData::PCS_delta_1::dam_fault4 dam_fault4;
			ModbusData::PCS_delta_1::dam_fault5 dam_fault5;
			ModbusData::PCS_delta_1::fault1 fault1;
			ModbusData::PCS_delta_1::fault2 fault2;
			ModbusData::PCS_delta_1::fault3 fault3;
			ModbusData::PCS_delta_1::fault4 fault4;


//			std::cout << "pcs_write_ret run ret: "<< pcs_write_ret << std::endl;
			try {
//				std::cout << "modbus_link_pcs_read_flag run ret: "
//						<< pcs_read_ret << std::endl;
				if (modbus_link_pcs_read_flag == false and pcs_read_ret != 0) {
//					std::cout << "modbus_link_pcs_read_flag == false: "
//							<< pcs_read_ret << std::endl;
					modbus_close(pcs_read_ctx);
					pcs_read_ret = modbus_connect(pcs_read_ctx);
				}
				if (pcs_read_ret == -1) {
					modbus_close(pcs_read_ctx);
					pcs_read_ret = modbus_connect(pcs_read_ctx);
//					std::cout << "modbus_close ret: "<< pcs_write_ret << std::endl;
				}

			} catch (...) {
			}
			double per_time;
			if (set_source == "Remote"){
				per_time = pcs_control_time_remote;
			}
			else if (set_source == "Local"){
				per_time = pcs_control_time_local;
			}
//			std::cout<<"pcs_control_time_local :"<<pcs_control_time_local<<std::endl;
//			std::cout << "pcs_connect_local :" << pcs_connect_local << std::endl;
//			std::cout << "pcs_reset_local :" << pcs_reset_local << std::endl;
			if (pcs_control_time != per_time){
				double connect_buffer;
				double reset_buffer;
				if (set_source == "Remote"){
					connect_buffer = pcs_control;
					reset_buffer = pcs_reset;
				}
				else if (set_source == "Local"){
					connect_buffer = pcs_connect_local;
					reset_buffer = pcs_reset_local;
				}
//				std::cout << "connect_buffer :" << connect_buffer << std::endl;
//				std::cout << "reset_buffer :" << reset_buffer << std::endl;

				/*pcs reset*/
				if (reset_buffer == 1 and pcs_read_ret != -1) {
					bool pcs_read_ok = false;
					uint16_t table_16[1];
					modbus_read_registers(pcs_read_ctx, 0, 1, table_16);
//					std::cout << table_16[0] <<std::endl;
					int control_bit = 1;
					int write_data = ( table_16[0] & (control_bit xor 0b1111111111111111) ) | control_bit;
//					std::cout << write_data <<std::endl;
					if (modbus_write_register(pcs_read_ctx, 0 , write_data) != -1) {
						/*p寫入成功*/
						/*上傳啟動資訊*/
//						std::cout << "PCS reset" << std::endl;
						std::vector<bsoncxx::document::value> status_docs;
						time_t now = time(0);
						now += 28800;
						status_docs.push_back(
								make_document(kvp("ID", "GC"),
										kvp("time", bsoncxx::types::b_date {
												std::chrono::system_clock::from_time_t(now) }),
										kvp("event", "PCS reset success"), kvp("group", "PCS_ReadData_thread"),
										kvp("level", 1), kvp("show", 1)));
						db_local["status"].insert_many(status_docs);
					} else {
						/*p寫入失敗*/
//						std::cout << "PCS reset" << std::endl;
						std::vector<bsoncxx::document::value> status_docs;
						time_t now = time(0);
						now += 28800;
						status_docs.push_back(
								make_document(kvp("ID", "GC"),
										kvp("time", bsoncxx::types::b_date {
												std::chrono::system_clock::from_time_t(now) }),
										kvp("event", "PCS reset fail"), kvp("group", "PCS_ReadData_thread"),
										kvp("level", 1), kvp("show", 1)));
						try{
							db_local["status"].insert_many(status_docs);
						}catch(...){}
					}
					pcs_control_time = per_time;
				}
				/*pcs control*/
				if (control_flag != connect_buffer and pcs_read_ret != -1) {
					uint16_t table_16[4];
					modbus_read_registers(pcs_read_ctx, 52, 4, table_16);
					int RdyRef = 4;
					int PQ_mode = 1;
					if ( ((table_16[0] & RdyRef) != RdyRef) or ((table_16[3] & PQ_mode) != PQ_mode)){/*RdyRef_bit == 0 or PQ_mode_bit == 0*/
					}
					else{/*可控制*/
					}
				}
			}
			/*上傳PCS資訊*/

			try {
				if (pcs_read_ret == -1) {
//					std::cout << "pcs_read_ret change" << std::endl;
					modbus_link_pcs_read_flag = false;
				}
//				std::cout<<"pcs_read_ret :"<<pcs_read_ret<<std::endl;
				bool pcs_read_ok = false;
//				std::cout<<"oooooooooooooooooooo"<<std::endl;
				std::vector<bsoncxx::document::value> PCS_docs;
				if (modbus_link_pcs_read_flag == true
						or (modbus_link_pcs_read_flag == false
								and pcs_read_error_count < 3)) {
					if (modbus_link_pcs_read_flag == true) {
						pcs_read_error_count = 0;
						modbus_find(pcs_read_ctx, PCS_delta.fc, pcs_read_ret,
								pcs_read_ok, pcs_read_arr, PCS_delta.Address,
								PCS_delta.Length, PCS_delta.order);
//						std::cout<<"xxxxxxxxxxxxxxxxxx"<<std::endl;
					} else if (modbus_link_pcs_read_flag == false
							and pcs_read_error_count < 3) {
						pcs_read_error_count++;
					}

					if (pcs_read_ok == true) {/*PCS讀取成功*/

						for (int x = 0 ;x<45 ; x++){
							pcsdata[x] = -1;
//							std::cout<<"pcsdata"<<x<<" :"<< pcsdata[x]<<std::endl;
						}
						pcsdata[0] = Read_upload(v1_grid, pcs_read_arr);
						pcsdata[1] = Read_upload(v2_grid, pcs_read_arr);
						pcsdata[2] = Read_upload(v3_grid, pcs_read_arr);
						pcsdata[9] = Read_upload(f_grid, pcs_read_arr);
						pcsdata[13] = Read_upload(i1_pcs, pcs_read_arr);
						pcsdata[14] = Read_upload(i2_pcs, pcs_read_arr);
						pcsdata[15] = Read_upload(i3_pcs, pcs_read_arr);
						pcsdata[16] = Read_upload(p_sum, pcs_read_arr);
						pcsdata[17] = Read_upload(q_sum, pcs_read_arr);

						pcsdata[18] = Read_upload(status_pcs, pcs_read_arr);
						pcsdata[19] = Read_upload(state_qs, pcs_read_arr);
						pcsdata[20] = Read_upload(inner_temp, pcs_read_arr);
						pcsdata[21] = Read_upload(sink_temp, pcs_read_arr);
						pcsdata[22] = Read_upload(v_dc, pcs_read_arr);
						pcsdata[23] = Read_upload(i_dc, pcs_read_arr);
						pcsdata[24] = Read_upload(v_bat, pcs_read_arr);

						pcsdata[25] = Read_upload(dam_fault1, pcs_read_arr);
						pcsdata[26] = Read_upload(dam_fault2, pcs_read_arr);
						pcsdata[27] = Read_upload(dam_fault3, pcs_read_arr);
						pcsdata[28] = Read_upload(dam_fault4, pcs_read_arr);
						pcsdata[29] = Read_upload(dam_fault5, pcs_read_arr);
						pcsdata[30] = Read_upload(fault1, pcs_read_arr);
						pcsdata[31] = Read_upload(fault2, pcs_read_arr);
						pcsdata[32] = Read_upload(fault3, pcs_read_arr);
						pcsdata[33] = Read_upload(fault4, pcs_read_arr);


//						for (int x = 0 ;x<45 ; x++){
//							std::cout<<"pcsdata"<<x<<" :"<< pcsdata[x]<<std::endl;
//						}
						PCS_docs.push_back(
								make_document(kvp("ID", PCS_eid),
										kvp("time",
												bsoncxx::types::b_date {
														std::chrono::system_clock::from_time_t(
																now) }),
										kvp("v1_grid", pcsdata[0]),
										kvp("v2_grid", pcsdata[1]),
										kvp("v3_grid", pcsdata[2]),
										kvp("f_grid", pcsdata[9]),
										kvp("i1_pcs", pcsdata[13]),
										kvp("i2_pcs", pcsdata[14]),
										kvp("i3_pcs", pcsdata[15]),
										kvp("p_sum", pcsdata[16]),
										kvp("q_sum", pcsdata[17]),

										kvp("status_pcs", (int)pcsdata[18]),
										kvp("state_qs", (int)pcsdata[19]),
										kvp("inner_temp", (int)pcsdata[20]),
										kvp("sink_temp", (int)pcsdata[21]),
										kvp("v_dc", pcsdata[22]),
										kvp("i_dc", pcsdata[23]),
										kvp("v_bat", pcsdata[24]),

										kvp("dam_fault1", (int)pcsdata[25]),
										kvp("dam_fault2", (int)pcsdata[26]),
										kvp("dam_fault3", (int)pcsdata[27]),
										kvp("dam_fault4", (int)pcsdata[28]),
										kvp("dam_fault5", (int)pcsdata[29]),
										kvp("fault1", (int)pcsdata[30]),
										kvp("fault2", (int)pcsdata[31]),
										kvp("fault3", (int)pcsdata[32]),
										kvp("fault4", (int)pcsdata[33])
										));
					}
				} else {
//					std::cout << "modbus pcs dont find >>>3"<< std::endl;
					pcs_read_error_count++;
					for (int x = 0; x < 48; x++) {
						pcsdata[x] = -1;
					}
					PCS_docs.push_back(
							make_document(kvp("ID", PCS_eid),
									kvp("time",
											bsoncxx::types::b_date {
													std::chrono::system_clock::from_time_t(
															now) }),
									kvp("v1_grid", bsoncxx::types::b_null { }),
									kvp("v2_grid", bsoncxx::types::b_null { }),
									kvp("v3_grid", bsoncxx::types::b_null { }),

									kvp("f_grid", bsoncxx::types::b_null { }),

									kvp("i1_pcs", bsoncxx::types::b_null { }),
									kvp("i2_pcs", bsoncxx::types::b_null { }),
									kvp("i3_pcs", bsoncxx::types::b_null { }),
									kvp("p_sum", bsoncxx::types::b_null { }),
									kvp("q_sum", bsoncxx::types::b_null { }),
									kvp("status_pcs", bsoncxx::types::b_null { }),
									kvp("state_qs", bsoncxx::types::b_null { }),
									kvp("inner_temp", bsoncxx::types::b_null { }),
									kvp("sink_temp", bsoncxx::types::b_null { }),
									kvp("v_dc", bsoncxx::types::b_null { }),
									kvp("i_dc", bsoncxx::types::b_null { }),
									kvp("v_bat", bsoncxx::types::b_null { }),
									kvp("dam_fault1", bsoncxx::types::b_null { }),
									kvp("dam_fault2", bsoncxx::types::b_null { }),
									kvp("dam_fault3", bsoncxx::types::b_null { }),
									kvp("dam_fault4", bsoncxx::types::b_null { }),
									kvp("dam_fault5", bsoncxx::types::b_null { }),
									kvp("fault1", bsoncxx::types::b_null { }),
									kvp("fault2", bsoncxx::types::b_null { }),
									kvp("fault3", bsoncxx::types::b_null { }),
									kvp("fault4", bsoncxx::types::b_null { })
									));
				}
//				time_print_m("control_thread running", 1);
//				std::cout<<"iiiiiiiiiiiiiiiiiiiiiiiii"<<std::endl;
				try{
					db["PCS"].insert_many(PCS_docs);
				}catch(...){}
				try{
					db_local["pcs"].insert_many(PCS_docs);
				}catch(...){}
				if (pcs_read_ret == 0) {/*連線成功*/
					modbus_link_pcs_read_flag = true;
				} else {
				}
//				std::cout << "main run  " << "ret :" << pcs_read_ret
//						<< " link_flag :" << modbus_link_pcs_read_flag
//						<< std::endl;
			} catch (...) {
			}

			function_run2 = false;
		}
	} catch (...) {
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
		bool &modbus_link_pcs_read_flag, std::string PCS_eid, int pv_len ,double *pv_list, int &pv_count, bool &gd ,double &total_p_per) {
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
				/*能力測試*/
//				std::cout << "ability_test :" << ability_test << std::endl;
				if (ability_test != "None") {
					HM_data[4][lock_buffer_100ms] = HM_f;
//					std::cout << "ability_test!=None :" << ability_test << " HM_f :" << HM_f<< " ability_test_count :" << ability_test_count << std::endl;
					ability_test_count++;
				} else {
					ability_test_count = 0;
				}
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
					} else if (mode == 2) {

					}else if (mode == 3){

						double pv_p_now = 0;/*(讀值)*/
						double pcs_p_now = 0;/*(寫值)PQ_p_ref*/

						double Smooth_p_variance = 10;/*(控制參數)60秒可變動量*/
						double max_pv = 10;/*(控制參數)pv最大值*/
						double shift_win =1;/*(控制參數)控制時間間格*/

						double Smooth_p_one;/*(存取參數)每次可變動量*/


						Smooth_p_one = max_pv * Smooth_p_variance / 100 * shift_win / 60;
						std::cout<< "pv_listxxxxxxxxxxx :" <<Smooth_p_one<<std::endl;
						std::cout<< "pv_listxxxxxxxxxxx :" <<pv_list[pv_count]<<std::endl;
						if (pv_count<pv_len){
							pv_p_now = pv_list[pv_count];
							pv_count ++;
						}
						else{
							pv_p_now=0;
							}
						Smooth_mode(pv_p_now, total_p_per, Smooth_p_one, pcs_p_now);
						std::cout<< "pcs_listxxxxxxxxxxx :" <<pcs_p_now<<std::endl;
						pcs_out_p = pcs_p_now;
					}

					else if ((mode == 5 or mode == 11)
							and (FP_type == 0 or FP_type == 1 or FP_type == 2
									or FP_type == 4)) { //dReg

						if (FP_type == 4) {
							pcs_out_p = pcs_out_p + PQ_p_ref;
						}
						if (mode == 11) {

						}
					} else if ((mode == 5 or mode == 11) and (FP_type == 3)) { /*sReg*/

						if (mode == 11) {

						}
					} else if (mode == 6) { /*PQ*/
						pcs_out_p = PQ_p_ref;
						pcs_out_q = PQ_q_ref;
					} else {
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

				if (mode == 5 or mode == 11 ) {
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
				int pcs_start_p = 4115; /*P寫入*/
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
//					std::cout<<"OKKKKKKK"<<std::endl;
					pcs_out_pre_p = pcs_out_p_now;/*有寫入成功才能代表有輸出*/

				} else {
					/*p寫入失敗*/
//					std::cout<<"errorrrrr"<<std::endl;
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
				PCS_ReadData_thread(pcsdata, set_source, pcs_connect_local, pcs_reset_local, pcs_reset,
						pcs_control,control_flag,pcs_control_time_local, pcs_control_time_remote, pcs_control_time, pcs_read_error_count,
						pcs_read_arr, pcs_ip, pcs_port, pcs_id, pcs_read_ctx,
						pcs_read_ret, modbus_link_pcs_read_flag, PCS_eid, db,
						db_local);


				std::cout << pcs_read_arr[1][0] <<std::endl;
				if (pcs_read_arr[1][0] < 5000){
					if (modbus_write_register(pcs_read_ctx, 4101,
							1) == 1){

					}
					/*切離開關*/
					Sleep(10);
					if (modbus_write_register(pcs_read_ctx, 4096,
							1) == 1){

					}
					else{

					}
					gd = false;

				}
				else if (gd == false){
					gd = true;
					if (modbus_write_register(pcs_read_ctx, 4101,
								0) == 1){

						}
				}
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

void dllgc::AFC_run(bool &stopflag, int &error_code, std::string &ability_test,
		double &pcs_connect_local, double &pcs_reset_local, double &pcs_control_time_local, double *emsdata, double *mbmsdata,
		double *HMdata, double *pcsdata, std::string MBMS_eid,
		std::string HM_eid, std::string PCS_eid, std::string set_source,
		int mode, double scale, double soc_max, double soc_min,
		double System_p_max, double System_p_min, double System_q_max,
		double System_q_min, double *f_arr, double *p_arr, double *v_arr,
		double *q_arr, double PQ_p_ref, double PQ_q_ref, int FP_type,
		double FP_soc_goal_percent, double FP_main_f_set, double FP_line_p_base,
		double FP_soc_dead_band, double FP_soc_adjust, int VC_type,
		double Vq_v_base, double Vq_q_base, double Vq_droop, double Vq_v_set,
		double PF_pf_ref, std::string pcs_ip, int pcs_port, int pcs_id,
		std::string HM_ip, int HM_port, int HM_id) {
	try {
//		std::cout << " AFC_run start " << std::endl;
//		std::cout << "pcs_ip :" << pcs_ip << " pcs_port :" << pcs_port
//				<< std::endl;
//		std::cout << "HM_ip :" << HM_ip << " HM_port :" << HM_port << std::endl;
		/*搜尋Mongo 初始化*/

		mongocxx::database db; /*定義資料庫*/
		mongocxx::database db2; /*定義資料庫*/
//		mongocxx::database db3; /*定義資料庫*/
		mongocxx::database db4; /*定義資料庫*/
//		std::cout << "ems_redundant :" << ems_redundant << std::endl;
		std::string url_data;
		if (ems_redundant == "0") {
			url_data = "mongodb://" + user + ":" + pwd + "@" + IP1 + ":"
					+ std::to_string(mongo_port1) + "/admin?authSource=admin&serverSelectionTimeoutMS=100";
		} else if (ems_redundant == "2") {
			url_data = "mongodb://" + user + ":" + pwd + "@" + IP1 + ":"
					+ std::to_string(mongo_port1) + "," + IP2 + ":"
					+ std::to_string(mongo_port2) + "/admin?authSource=admin&serverSelectionTimeoutMS=100";
		}
		std::cout << url_data << std::endl;
		mongocxx::uri uri(url_data);
		mongocxx::client client(uri);
		mongocxx::client client2(uri);
//		mongocxx::client client3(uri);
		mongocxx::client client4(uri);
		db = client["rookie"];
		db2 = client2["rookie"];
//		db3 = client3["AFC"];
		db4 = client4["rookie"];

		mongocxx::database db_local; /*定義資料庫*/
		mongocxx::database db2_local; /*定義資料庫*/
//		mongocxx::database db3_local; /*定義資料庫*/
		mongocxx::database db4_local; /*定義資料庫*/
		mongocxx::database db5_local; /*定義資料庫*/
		std::string url_data_local =
				"mongodb://root:pc152@127.0.0.1:27017/admin?authSource=admin&serverSelectionTimeoutMS=100";
		std::cout << url_data_local << std::endl;
		mongocxx::uri uri_local(url_data_local);
		mongocxx::client client_local(uri_local);
		mongocxx::client client2_local(uri_local);
//		mongocxx::client client3_local(uri_local);
		mongocxx::client client4_local(uri_local);
		mongocxx::client client5_local(uri_local);
		db_local = client_local["AFC_local"];
		db2_local = client2_local["AFC_local"];
//		db3_local = client3_local["AFC_local"];
		db4_local = client4_local["AFC_local"];
		db5_local = client5_local["AFC_local"];

		/*PCS modbus TCP 宣告*/
//		modbus_t *pcs_read_ctx = modbus_new_tcp(pcs_ip.c_str(), pcs_port);
		modbus_t *pcs_read_ctx = modbus_new_rtu("COM5", 38400, 'N', 8, 1);
//		modbus_t *pcs_write_ctx = modbus_new_tcp(pcs_ip.c_str(), pcs_port);
		if (pcs_read_ctx == NULL) {
			printf("Unable to create the libmodbus PCS read context\n");
			std::cout << "pcs_ip : " << pcs_ip << "pcs_port : " << pcs_port
					<< std::endl;
			fflush(stdout);
		}
//		if (pcs_write_ctx == NULL) {
//			printf("Unable to create the libmodbus PCS write context\n");
//			std::cout << "pcs_ip : " << pcs_ip << "pcs_port : " << pcs_port
//					<< std::endl;
//			fflush(stdout);
//		}
		int pcs_read_ret;
//		int pcs_write_ret;
//		std::cout  << "pcs_id : " << pcs_id<< std::endl;
		modbus_set_slave(pcs_read_ctx, pcs_id);
//		modbus_set_slave(pcs_write_ctx, pcs_id);
		modbus_set_response_timeout(pcs_read_ctx, 0, 3000000);
		pcs_read_ret = modbus_connect(pcs_read_ctx);
//		pcs_write_ret = modbus_connect(pcs_write_ctx);

//		bool pcs_rdy = false;
		/*High Meter modbus TCP 宣告*/
		modbus_t *HM_ctx = modbus_new_tcp(HM_ip.c_str(), HM_port);
		if (HM_ctx == NULL) {
			printf("Unable to create the libmodbus context HM_ctx\n");
			std::cout << "HM_ip : " << HM_ip << "HM_port : " << HM_port
					<< std::endl;
			fflush(stdout);
		}
		int HM_ret;

		modbus_set_slave(HM_ctx, HM_id);
		modbus_set_response_timeout(HM_ctx, 0, 30000);
		HM_ret = modbus_connect(HM_ctx);

		/*參數宣告*/
		double HM_f = 0;
		double HM_p = 0;
		double HM_v = 0;
		double HM_q = 0;

		Sleep(1000);
		if (modbus_write_register(pcs_read_ctx, 4096,
				1) == 1){
//			std::cout <<"xxxxxxxxxxxxxxxxxxxx"<< std::endl;
		}
		else{
//			std::cout <<"qqqqqqqqqqqqqqqqq"<< std::endl;
		}
		/*dReg 計算*/
		FP_soc_adjust = FP_soc_adjust / 100.0;
		p_arr[0] = p_arr[0] * FP_line_p_base / 100;
		p_arr[1] = p_arr[1] * FP_line_p_base / 100;
		p_arr[2] = p_arr[2] * FP_line_p_base / 100;
		p_arr[3] = p_arr[3] * FP_line_p_base / 100;
		p_arr[4] = p_arr[4] * FP_line_p_base / 100;
		p_arr[5] = p_arr[5] * FP_line_p_base / 100;
		double a1 = (p_arr[1] - p_arr[0]) / (f_arr[1] - f_arr[0]) * 1.000;
		double a21 = ((p_arr[2] + (FP_soc_adjust * FP_line_p_base)) - p_arr[1])
				/ (f_arr[2] - f_arr[1]) * 1.000;
		double a22 = (p_arr[2] - p_arr[1]) / (f_arr[2] - f_arr[1]) * 1.000;
		double a23 = ((p_arr[2] - (FP_soc_adjust * FP_line_p_base)) - p_arr[1])
				/ (f_arr[2] - f_arr[1]) * 1.000;
//		double a3 = 0; /*中間值自由調整為0無須斜率*/
		double a41 = (p_arr[4] - (p_arr[3] + (FP_soc_adjust * FP_line_p_base)))
				/ (f_arr[4] - f_arr[3]) * 1.000;
		double a42 = (p_arr[4] - p_arr[3]) / (f_arr[4] - f_arr[3]) * 1.000;
		double a43 = (p_arr[4] - (p_arr[3] - (FP_soc_adjust * FP_line_p_base)))
				/ (f_arr[4] - f_arr[3]) * 1.000;
		double a5 = (p_arr[5] - p_arr[4]) / (f_arr[5] - f_arr[4]) * 1.000;

		double b1 = p_arr[0] - a1 * f_arr[0] * 1.000;
		double b21 = p_arr[1] - a21 * f_arr[1] * 1.000;
		double b22 = p_arr[1] - a22 * f_arr[1] * 1.000;
		double b23 = p_arr[1] - a23 * f_arr[1] * 1.000;
		double b3 = FP_soc_adjust * FP_line_p_base;
		double b41 = p_arr[4] - a41 * f_arr[4] * 1.000;
		double b42 = p_arr[4] - a42 * f_arr[4] * 1.000;
		double b43 = p_arr[4] - a43 * f_arr[4] * 1.000;
		double b5 = p_arr[5] - a5 * f_arr[5] * 1.000;
		/*sReg計算*/
		double a_sReg = (p_arr[3] - p_arr[2]) / (f_arr[3] - f_arr[2]) * 1.000;
		;
		double b_sReg = p_arr[3] - a_sReg * f_arr[3] * 1.000;
		/*VQ計算*/
		double a_qAuto = Vq_droop;
		double b_qAuto = Vq_q_base - (Vq_droop * Vq_v_base);

		/*上傳啟動資訊*/
		std::cout << "AFC start" << std::endl;
		std::vector<bsoncxx::document::value> status_docs;
		time_t now = time(0);
		now += 28800;
		status_docs.push_back(
				make_document(kvp("ID", "GC"),
						kvp("time", bsoncxx::types::b_date {
								std::chrono::system_clock::from_time_t(now) }),
						kvp("event", "AFC start"), kvp("group", "AFC_run"),
						kvp("level", 1), kvp("show", 1)));
		try{
			db_local["status"].insert_many(status_docs);
		}catch(...){}
		/*excel定義*/
//		std::ofstream oFile;
//		oFile.open("deg_output_data.csv", std::ios::out | std::ios::trunc);
		/*其餘參數設定*/
		Timer *t = new Timer();
//		std::cout << " AFC_run setFixedTime_AFG start " << std::endl;
		std::string ability_test_buffer = ability_test;
		double pcs_connect_local_buffer = 0;
		double pcs_reset_local_buffer = 0;
		double pcs_control_time_local_buffer = 0;
		t->ability_test = "None";
		t->pcs_connect_local = 0;
		t->pcs_reset_local = 0;
		t->pcs_control_time_local = 0;
		t->setFixedTime_AFG(control_thread, HM_link_thread, error_thread,
				PCS_ReadData_thread, EMS_ReadData_thread, set_source,
				emsdata, mbmsdata, HMdata, pcsdata, mode, scale, soc_max,
				soc_min, System_p_max, System_p_min, System_q_max, System_q_min,
				f_arr, p_arr, v_arr, q_arr, PQ_p_ref, PQ_q_ref, FP_type,
				FP_soc_goal_percent, FP_main_f_set, FP_line_p_base,
				FP_soc_dead_band, FP_soc_adjust, VC_type, Vq_v_base, Vq_q_base,
				Vq_droop, Vq_v_set, PF_pf_ref, HM_ctx, HM_ret, HM_f, HM_p, HM_v,
				HM_q, pcs_read_ctx, pcs_read_ret, a1, b1, a21, b21, a22, b22,
				a23, b23, b3, a41, b41, a42, b42, a43, b43, a5, b5, a_sReg,
				b_sReg, a_qAuto, b_qAuto, MBMS_eid, HM_eid, PCS_eid, pcs_ip,
				pcs_port, pcs_id, HM_ip, HM_port, HM_id, db, db2, db4, db_local,
				db2_local, db4_local, db5_local);
		while (true) {
			if (ability_test_buffer != ability_test) {
				t->ability_test = ability_test;
				ability_test_buffer = ability_test;
				std::cout << "ability_test ui change" << std::endl;
			}
			if (pcs_connect_local_buffer != pcs_connect_local) {
				t->pcs_connect_local = pcs_connect_local;
				pcs_connect_local_buffer = pcs_connect_local;
			}
			if (pcs_reset_local_buffer != pcs_reset_local) {
				t->pcs_reset_local = pcs_reset_local;
				pcs_reset_local_buffer = pcs_reset_local;
			}
			if (pcs_control_time_local_buffer != pcs_control_time_local) {
				t->pcs_control_time_local = pcs_control_time_local;
				pcs_control_time_local_buffer = pcs_control_time_local;
			}
			if (stopflag == true) {
				t->stop();
				Sleep(1000);
				break;
			}

//			time_print_m("main running",1);
			Sleep(100);
		}
		/*上傳結束資訊*/
		std::cout << "AFC end" << std::endl;
		now = time(0);
		now += 28800;
		status_docs.push_back(
				make_document(kvp("ID", "GC"),
						kvp("time", bsoncxx::types::b_date {
								std::chrono::system_clock::from_time_t(now) }),
						kvp("event", "AFC end"), kvp("group", "AFC_run"),
						kvp("level", 1), kvp("show", 1)));
		try{
			db_local["status"].insert_many(status_docs);
		}catch(...){}
		delete t;

		modbus_close(pcs_read_ctx);
		modbus_free(pcs_read_ctx);
//		modbus_close(pcs_write_ctx);
//		modbus_free(pcs_write_ctx);
		modbus_close(HM_ctx);
		modbus_free(HM_ctx);

//		oFile.close();
	} catch (...) {
		time_print_m("main error", 1);
	}
}

