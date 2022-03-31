#include <iostream>
#include <modbus/modbus.h>
#include "dllgc.h"
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
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;
using bsoncxx::builder::basic::sub_array;
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
			ModbusData::PCS_delta PCS_delta;
			ModbusData::PCS_delta::v1_grid v1_grid;
			ModbusData::PCS_delta::v2_grid v2_grid;
			ModbusData::PCS_delta::v3_grid v3_grid;
			ModbusData::PCS_delta::i1_grid i1_grid;
			ModbusData::PCS_delta::i2_grid i2_grid;
			ModbusData::PCS_delta::i3_grid i3_grid;
			ModbusData::PCS_delta::p_grid p_grid;
			ModbusData::PCS_delta::q_grid q_grid;
			ModbusData::PCS_delta::pf_grid pf_grid;
			ModbusData::PCS_delta::f_grid f_grid;
			ModbusData::PCS_delta::v1_pcs v1_pcs;
			ModbusData::PCS_delta::v2_pcs v2_pcs;
			ModbusData::PCS_delta::v3_pcs v3_pcs;
			ModbusData::PCS_delta::i1_pcs i1_pcs;
			ModbusData::PCS_delta::i2_pcs i2_pcs;
			ModbusData::PCS_delta::i3_pcs i3_pcs;
			ModbusData::PCS_delta::p_sum p_sum;
			ModbusData::PCS_delta::q_sum q_sum;
			ModbusData::PCS_delta::pf pf;
			ModbusData::PCS_delta::f_pcs_low f_pcs_low;
			ModbusData::PCS_delta::p_dc p_dc;
			ModbusData::PCS_delta::status_pcs_1 status_pcs_1;
			ModbusData::PCS_delta::status_pcs_2 status_pcs_2;
			ModbusData::PCS_delta::status_system status_system;
			ModbusData::PCS_delta::mode_pcs mode_pcs;
			ModbusData::PCS_delta::module1_fault1 module1_fault1;
			ModbusData::PCS_delta::module1_fault2 module1_fault2;
			ModbusData::PCS_delta::module1_fault3 module1_fault3;
			ModbusData::PCS_delta::module2_fault1 module2_fault1;
			ModbusData::PCS_delta::module2_fault2 module2_fault2;
			ModbusData::PCS_delta::module2_fault3 module2_fault3;
			ModbusData::PCS_delta::module3_fault1 module3_fault1;
			ModbusData::PCS_delta::module3_fault2 module3_fault2;
			ModbusData::PCS_delta::module3_fault3 module3_fault3;
			ModbusData::PCS_delta::module4_fault1 module4_fault1;
			ModbusData::PCS_delta::module4_fault2 module4_fault2;
			ModbusData::PCS_delta::module4_fault3 module4_fault3;
			ModbusData::PCS_delta::system_fault1 system_fault1;
			ModbusData::PCS_delta::system_fault2 system_fault2;
			ModbusData::PCS_delta::system_fault3 system_fault3;
			ModbusData::PCS_delta::cooling_fault cooling_fault;
			ModbusData::PCS_delta::fan_fault fan_fault;
			ModbusData::PCS_delta::system_warning system_warning;
			ModbusData::PCS_delta::rst_word rst_word;
			ModbusData::PCS_delta::PQ_p_out PQ_p_out;
			ModbusData::PCS_delta::PQ_q_out PQ_q_out;
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
			int status_pcs_1_arr[16];
			int status_pcs_2_arr[16];
			int status_system_arr[16];
			int mode_pcs_arr[16];
			int module1_fault1_arr[16];
			int module1_fault2_arr[16];
			int module1_fault3_arr[16];
			int module2_fault1_arr[16];
			int module2_fault2_arr[16];
			int module2_fault3_arr[16];
			int module3_fault1_arr[16];
			int module3_fault2_arr[16];
			int module3_fault3_arr[16];
			int module4_fault1_arr[16];
			int module4_fault2_arr[16];
			int module4_fault3_arr[16];
			int system_fault1_arr[16];
			int system_fault2_arr[16];
			int system_fault3_arr[16];
			int cooling_fault_arr[16];
			int fan_fault_arr[16];
			int system_warning_arr[16];
			int rst_word_arr[16];
			try {
				if (pcs_read_ret == -1) {
//					std::cout << "pcs_read_ret change" << std::endl;
					modbus_link_pcs_read_flag = false;
				}
//				std::cout<<"pcs_read_ret :"<<pcs_read_ret<<std::endl;
				bool pcs_read_ok = false;
				std::vector<bsoncxx::document::value> PCS_docs;
				if (modbus_link_pcs_read_flag == true
						or (modbus_link_pcs_read_flag == false
								and pcs_read_error_count < 3)) {
					if (modbus_link_pcs_read_flag == true) {
						pcs_read_error_count = 0;
						modbus_find(pcs_read_ctx, PCS_delta.fc, pcs_read_ret,
								pcs_read_ok, pcs_read_arr, PCS_delta.Address,
								PCS_delta.Length, PCS_delta.order);
					} else if (modbus_link_pcs_read_flag == false
							and pcs_read_error_count < 3) {
						pcs_read_error_count++;
					}

					if (pcs_read_ok == true) {/*PCS讀取成功*/

//						std::cout << "pcs_read_arr:" << pcs_read_arr[0]
//								<< std::endl;
						Read_upload_bit(status_pcs_1, pcs_read_arr,
								status_pcs_1_arr);
						Read_upload_bit(status_pcs_2, pcs_read_arr,
								status_pcs_2_arr);
						Read_upload_bit(status_system, pcs_read_arr,
								status_system_arr);
						Read_upload_bit(mode_pcs, pcs_read_arr, mode_pcs_arr);
						Read_upload_bit(module1_fault1, pcs_read_arr,
								module1_fault1_arr);
						Read_upload_bit(module1_fault2, pcs_read_arr,
								module1_fault2_arr);
						Read_upload_bit(module1_fault3, pcs_read_arr,
								module1_fault3_arr);
						Read_upload_bit(module2_fault1, pcs_read_arr,
								module2_fault1_arr);
						Read_upload_bit(module2_fault2, pcs_read_arr,
								module2_fault2_arr);
						Read_upload_bit(module2_fault3, pcs_read_arr,
								module2_fault3_arr);
						Read_upload_bit(module3_fault1, pcs_read_arr,
								module3_fault1_arr);
						Read_upload_bit(module3_fault2, pcs_read_arr,
								module3_fault2_arr);
						Read_upload_bit(module3_fault3, pcs_read_arr,
								module3_fault3_arr);
						Read_upload_bit(module4_fault1, pcs_read_arr,
								module4_fault1_arr);
						Read_upload_bit(module4_fault2, pcs_read_arr,
								module4_fault2_arr);
						Read_upload_bit(module4_fault3, pcs_read_arr,
								module4_fault3_arr);
						Read_upload_bit(system_fault1, pcs_read_arr,
								system_fault1_arr);
						Read_upload_bit(system_fault2, pcs_read_arr,
								system_fault2_arr);
						Read_upload_bit(system_fault3, pcs_read_arr,
								system_fault3_arr);
						Read_upload_bit(cooling_fault, pcs_read_arr,
								cooling_fault_arr);
						Read_upload_bit(fan_fault, pcs_read_arr, fan_fault_arr);
						Read_upload_bit(system_warning, pcs_read_arr,
								system_warning_arr);
						Read_upload_bit(rst_word, pcs_read_arr, rst_word_arr);
						pcsdata[0] = Read_upload(v1_grid, pcs_read_arr);
						pcsdata[1] = Read_upload(v2_grid, pcs_read_arr);
						pcsdata[2] = Read_upload(v3_grid, pcs_read_arr);
						pcsdata[3] = Read_upload(i1_grid, pcs_read_arr);
						pcsdata[4] = Read_upload(i2_grid, pcs_read_arr);
						pcsdata[5] = Read_upload(i3_grid, pcs_read_arr);
						pcsdata[6] = Read_upload(p_grid, pcs_read_arr);
						pcsdata[7] = Read_upload(q_grid, pcs_read_arr);
						pcsdata[8] = Read_upload(pf_grid, pcs_read_arr);
						pcsdata[9] = Read_upload(f_grid, pcs_read_arr);
						pcsdata[10] = Read_upload(v1_pcs, pcs_read_arr);
						pcsdata[11] = Read_upload(v2_pcs, pcs_read_arr);
						pcsdata[12] = Read_upload(v3_pcs, pcs_read_arr);
						pcsdata[13] = Read_upload(i1_pcs, pcs_read_arr);
						pcsdata[14] = Read_upload(i2_pcs, pcs_read_arr);
						pcsdata[15] = Read_upload(i3_pcs, pcs_read_arr);
						pcsdata[16] = Read_upload(p_sum, pcs_read_arr);
						pcsdata[17] = Read_upload(q_sum, pcs_read_arr);
						pcsdata[18] = Read_upload(pf, pcs_read_arr);
						pcsdata[19] = Read_upload(f_pcs_low, pcs_read_arr);
						pcsdata[20] = Read_upload(p_dc, pcs_read_arr);
						pcsdata[21] = Read_upload(status_pcs_1, pcs_read_arr);
						pcsdata[22] = Read_upload(status_pcs_2, pcs_read_arr);
						pcsdata[23] = Read_upload(status_system, pcs_read_arr);
						pcsdata[24] = Read_upload(mode_pcs, pcs_read_arr);
						pcsdata[25] = Read_upload(module1_fault1, pcs_read_arr);
						pcsdata[26] = Read_upload(module1_fault2, pcs_read_arr);
						pcsdata[27] = Read_upload(module1_fault3, pcs_read_arr);
						pcsdata[28] = Read_upload(module2_fault1, pcs_read_arr);
						pcsdata[29] = Read_upload(module2_fault2, pcs_read_arr);
						pcsdata[30] = Read_upload(module2_fault3, pcs_read_arr);
						pcsdata[31] = Read_upload(module3_fault1, pcs_read_arr);
						pcsdata[32] = Read_upload(module3_fault2, pcs_read_arr);
						pcsdata[33] = Read_upload(module3_fault3, pcs_read_arr);
						pcsdata[34] = Read_upload(module4_fault1, pcs_read_arr);
						pcsdata[35] = Read_upload(module4_fault2, pcs_read_arr);
						pcsdata[36] = Read_upload(module4_fault3, pcs_read_arr);
						pcsdata[37] = Read_upload(system_fault1, pcs_read_arr);
						pcsdata[38] = Read_upload(system_fault2, pcs_read_arr);
						pcsdata[39] = Read_upload(system_fault3, pcs_read_arr);
						pcsdata[40] = Read_upload(cooling_fault, pcs_read_arr);
						pcsdata[41] = Read_upload(fan_fault, pcs_read_arr);
						pcsdata[42] = Read_upload(system_warning, pcs_read_arr);
						pcsdata[43] = Read_upload(rst_word, pcs_read_arr);
						pcsdata[44] = Read_upload(PQ_p_out, pcs_read_arr);
						pcsdata[45] = Read_upload(PQ_q_out, pcs_read_arr);
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
										kvp("i1_grid", pcsdata[3]),
										kvp("i2_grid", pcsdata[4]),
										kvp("i3_grid", pcsdata[5]),
										kvp("p_grid", pcsdata[6]),
										kvp("q_grid", pcsdata[7]),
										kvp("pf_grid", pcsdata[8]),
										kvp("f_grid", pcsdata[9]),
										kvp("v1_pcs", pcsdata[10]),
										kvp("v2_pcs", pcsdata[11]),
										kvp("v3_pcs", pcsdata[12]),
										kvp("i1_pcs", pcsdata[13]),
										kvp("i2_pcs", pcsdata[14]),
										kvp("i3_pcs", pcsdata[15]),
										kvp("p_sum", pcsdata[16]),
										kvp("q_sum", pcsdata[17]),
										kvp("pf", pcsdata[18]),
										kvp("f_pcs_low", pcsdata[19]),
										kvp("p_dc", pcsdata[20]),
										kvp("status_pcs_1",
												[&status_pcs_1_arr](
														sub_array child) {
													for (const int &element : status_pcs_1_arr) {
														child.append(element);
													}
												})
										,
										kvp("status_pcs_2",
												[&status_pcs_2_arr](
														sub_array child) {
													for (const int &element : status_pcs_2_arr) {
														child.append(element);
													}
												})
										,
										kvp("status_system",
												[&status_system_arr](
														sub_array child) {
													for (const int &element : status_system_arr) {
														child.append(element);
													}
												})
										,
										kvp("mode_pcs",
												[&mode_pcs_arr](
														sub_array child) {
													for (const int &element : mode_pcs_arr) {
														child.append(element);
													}
												})
										,
										kvp("module1_fault1",
												[&module1_fault1_arr](
														sub_array child) {
													for (const int &element : module1_fault1_arr) {
														child.append(element);
													}
												})
										,
										kvp("module1_fault2",
												[&module1_fault2_arr](
														sub_array child) {
													for (const int &element : module1_fault2_arr) {
														child.append(element);
													}
												})
										,
										kvp("module1_fault3",
												[&module1_fault3_arr](
														sub_array child) {
													for (const int &element : module1_fault3_arr) {
														child.append(element);
													}
												})
										,
										kvp("module2_fault1",
												[&module2_fault1_arr](
														sub_array child) {
													for (const int &element : module2_fault1_arr) {
														child.append(element);
													}
												})
										,
										kvp("module2_fault2",
												[&module2_fault2_arr](
														sub_array child) {
													for (const int &element : module2_fault2_arr) {
														child.append(element);
													}
												})
										,
										kvp("module2_fault3",
												[&module2_fault3_arr](
														sub_array child) {
													for (const int &element : module2_fault3_arr) {
														child.append(element);
													}
												})
										,
										kvp("module3_fault1",
												[&module3_fault1_arr](
														sub_array child) {
													for (const int &element : module3_fault1_arr) {
														child.append(element);
													}
												})
										,
										kvp("module3_fault2",
												[&module3_fault2_arr](
														sub_array child) {
													for (const int &element : module3_fault2_arr) {
														child.append(element);
													}
												})
										,
										kvp("module3_fault3",
												[&module3_fault3_arr](
														sub_array child) {
													for (const int &element : module3_fault3_arr) {
														child.append(element);
													}
												})
										,
										kvp("module4_fault1",
												[&module4_fault1_arr](
														sub_array child) {
													for (const int &element : module4_fault1_arr) {
														child.append(element);
													}
												})
										,
										kvp("module4_fault2",
												[&module4_fault2_arr](
														sub_array child) {
													for (const int &element : module4_fault2_arr) {
														child.append(element);
													}
												})
										,
										kvp("module4_fault3",
												[&module4_fault3_arr](
														sub_array child) {
													for (const int &element : module4_fault3_arr) {
														child.append(element);
													}
												})
										,
										kvp("system_fault1",
												[&system_fault1_arr](
														sub_array child) {
													for (const int &element : system_fault1_arr) {
														child.append(element);
													}
												})
										,
										kvp("system_fault2",
												[&system_fault2_arr](
														sub_array child) {
													for (const int &element : system_fault2_arr) {
														child.append(element);
													}
												})
										,
										kvp("system_fault3",
												[&system_fault3_arr](
														sub_array child) {
													for (const int &element : system_fault3_arr) {
														child.append(element);
													}
												})
										,
										kvp("cooling_fault",
												[&cooling_fault_arr](
														sub_array child) {
													for (const int &element : cooling_fault_arr) {
														child.append(element);
													}
												})
										,
										kvp("fan_fault",
												[&fan_fault_arr](
														sub_array child) {
													for (const int &element : fan_fault_arr) {
														child.append(element);
													}
												})
										,
										kvp("system_warning",
												[&system_warning_arr](
														sub_array child) {
													for (const int &element : system_warning_arr) {
														child.append(element);
													}
												})
										,
										kvp("rst_word",
												[&rst_word_arr](
														sub_array child) {
													for (const int &element : rst_word_arr) {
														child.append(element);
													}
												}) ,
										kvp("PQ_p_out", pcsdata[44]),
										kvp("PQ_q_out", pcsdata[45])));
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
									kvp("i1_grid", bsoncxx::types::b_null { }),
									kvp("i2_grid", bsoncxx::types::b_null { }),
									kvp("i3_grid", bsoncxx::types::b_null { }),
									kvp("p_grid", bsoncxx::types::b_null { }),
									kvp("q_grid", bsoncxx::types::b_null { }),
									kvp("pf_grid", bsoncxx::types::b_null { }),
									kvp("f_grid", bsoncxx::types::b_null { }),
									kvp("v1_pcs", bsoncxx::types::b_null { }),
									kvp("v2_pcs", bsoncxx::types::b_null { }),
									kvp("v3_pcs", bsoncxx::types::b_null { }),
									kvp("i1_pcs", bsoncxx::types::b_null { }),
									kvp("i2_pcs", bsoncxx::types::b_null { }),
									kvp("i3_pcs", bsoncxx::types::b_null { }),
									kvp("p_sum", bsoncxx::types::b_null { }),
									kvp("q_sum", bsoncxx::types::b_null { }),
									kvp("pf", bsoncxx::types::b_null { }),
									kvp("f_pcs_low",
											bsoncxx::types::b_null { }),
									kvp("p_dc", bsoncxx::types::b_null { }),
									kvp("status_pcs_1",
											bsoncxx::types::b_null { }),
									kvp("status_pcs_2",
											bsoncxx::types::b_null { }),
									kvp("status_system",
											bsoncxx::types::b_null { }),
									kvp("mode_pcs", bsoncxx::types::b_null { }),
									kvp("module1_fault1",
											bsoncxx::types::b_null { }),
									kvp("module1_fault2",
											bsoncxx::types::b_null { }),
									kvp("module1_fault3",
											bsoncxx::types::b_null { }),
									kvp("module2_fault1",
											bsoncxx::types::b_null { }),
									kvp("module2_fault2",
											bsoncxx::types::b_null { }),
									kvp("module2_fault3",
											bsoncxx::types::b_null { }),
									kvp("module3_fault1",
											bsoncxx::types::b_null { }),
									kvp("module3_fault2",
											bsoncxx::types::b_null { }),
									kvp("module3_fault3",
											bsoncxx::types::b_null { }),
									kvp("module4_fault1",
											bsoncxx::types::b_null { }),
									kvp("module4_fault2",
											bsoncxx::types::b_null { }),
									kvp("module4_fault3",
											bsoncxx::types::b_null { }),
									kvp("system_fault1",
											bsoncxx::types::b_null { }),
									kvp("system_fault2",
											bsoncxx::types::b_null { }),
									kvp("system_fault3",
											bsoncxx::types::b_null { }),
									kvp("cooling_fault",
											bsoncxx::types::b_null { }),
									kvp("fan_fault",
											bsoncxx::types::b_null { }),
									kvp("system_warning",
											bsoncxx::types::b_null { }),
									kvp("rst_word", bsoncxx::types::b_null { }),
									kvp("PQ_p_out", bsoncxx::types::b_null { }),
									kvp("PQ_q_out",
											bsoncxx::types::b_null { })));
				}
				try{
					db["pcs"].insert_many(PCS_docs);
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


int main() {
	return 0;
}
