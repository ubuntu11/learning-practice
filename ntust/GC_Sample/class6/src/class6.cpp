/**
 * class6.cpp, 用程式的方式讀取GC電腦的CPU與記憶體使用情況並保存到Mongodb中.
 *
 * 智電提供的是windows版本,參考"How to determine CPU and memory consumption from
 * inside a process" in stackoverflow, 改為可在Linux上執行.
 */

#include <iostream>
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include <string>
#include <thread>
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
#include <stdio.h>
#include <unistd.h>
#include "sys/types.h"
#include "sys/sysinfo.h"

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;
using bsoncxx::builder::basic::sub_array;

static unsigned long long lastTotalUser, lastTotalUserLow, lastTotalSys,
		lastTotalIdle;
struct timespec currentTime;
struct sysinfo memInfo;

/**
 * 取得目前系統時間
 * @return current time as struct timespec
 */
void get_current_timestamp(struct timespec *ts) {
	timespec_get(ts, TIME_UTC);
}

/**
 * 從系統時間取得秒數的部份.
 * @param ts current time as struct timespec
 * @return seconds in current time
 */
int get_seconds(struct timespec ts) {
	return ts.tv_sec % 60;
}

/**
 * 從系統時間取得毫秒的部份.
 * @param ts current time as struct timespec
 * @return milliseconds in current time
 */
int get_milliseconds(struct timespec ts) {
	return (ts.tv_sec * 1000 + lround(ts.tv_nsec / 1e6)) % 1000;
}

double GetTotalPhysicalMemoryUsed() {/*memory GB*/
	long long physMemUsed = memInfo.totalram - memInfo.freeram;
	//Multiply in next statement to avoid int overflow on right hand side...
	physMemUsed *= memInfo.mem_unit;
	return physMemUsed / 1024 / 1024 / 1024;
}

double GetPhysicalMemoryUsage() {/*memory %*/
	double MemUsage = (memInfo.totalram - memInfo.freeram)
			/ (double) memInfo.totalram * 100;
	return MemUsage;
}

double getCpuUsage() {
	double percent;
	FILE *file;
	unsigned long long totalUser, totalUserLow, totalSys, totalIdle, total;

	file = fopen("/proc/stat", "r");
	fscanf(file, "cpu %llu %llu %llu %llu", &totalUser, &totalUserLow,
			&totalSys, &totalIdle);
	fclose(file);

	if (totalUser < lastTotalUser || totalUserLow < lastTotalUserLow
			|| totalSys < lastTotalSys || totalIdle < lastTotalIdle) {
		//Overflow detection. Just skip this value.
		percent = -1.0;
	} else {
		total = (totalUser - lastTotalUser) + (totalUserLow - lastTotalUserLow)
				+ (totalSys - lastTotalSys);
		percent = total;
		total += (totalIdle - lastTotalIdle);
		percent /= total;
		percent *= 100;
	}

	lastTotalUser = totalUser;
	lastTotalUserLow = totalUserLow;
	lastTotalSys = totalSys;
	lastTotalIdle = totalIdle;

	return percent;
}

void error_thread(bool &thread_end, int &thread_count,
		int &pcs_read_error_count, int &HM_error_count, std::string HM_eid,
		std::string PCS_eid, mongocxx::database db, mongocxx::database db_local,
		bool &error_flag) {
	int lock_buffer_1s = get_seconds(currentTime);
	int lock_buffer_100ms = int(get_milliseconds(currentTime) / 100);
	std::string fc_name = "error_thread";
	bool pcs_error = false;
	bool HM_error = false;
	while (true) {
		while (lock_buffer_100ms == int(get_milliseconds(currentTime) / 100)) {
			if (thread_end == true)
				return;
			get_current_timestamp(&currentTime);
			// 休眠0.1秒
			usleep(1000 * 100);
		}

		lock_buffer_100ms = int(get_milliseconds(currentTime) / 100);
		if (thread_end == true)
			return;

		if (lock_buffer_1s != get_seconds(currentTime)) {
			lock_buffer_1s = get_seconds(currentTime);
			thread_count++;
			try {
				if (lock_buffer_1s % 10 == 0 or lock_buffer_1s % 10 == 5) {
					// GetTickCount(): 從作業系統啟動到現在所經過的毫秒數
					// int iRunTime = GetTickCount();
					int iRunTime = 32768;
					time_t nowtime = time(0);
					nowtime += 28800;
					time_t DateTime = nowtime - (iRunTime / 1000);
					//CHAR cUserNameBuffer[256];
					//DWORD dwUserNameSize = 256;
					//if (GetUserName(cUserNameBuffer, &dwUserNameSize)) {
					//	printf("The user name is %s \n", cUserNameBuffer);
					//} else {
					// GetLastError(): a Windows API to 獲取錯誤代碼
					//	printf("Get user name failed with error: %lu \n", GetLastError());
					//}
					sysinfo (&memInfo);
					double memory_data = GetTotalPhysicalMemoryUsed();
					double memory_per = GetPhysicalMemoryUsage();
					double cpu = getCpuUsage();
					std::cout << "nowtime :" << nowtime << std::endl;
					// std::cout<<"user name :"<<cUserNameBuffer<<std::endl;
					std::cout << "user name :" << "Ken Hu" << std::endl;
					std::cout << "memory (GB)" << memory_data << std::endl;
					std::cout << "memory (%)" << memory_per << std::endl;
					std::cout << "CPU (%)" << cpu << std::endl;

					std::vector<bsoncxx::document::value> Status;
					Status.push_back(
					// make_document(kvp("ID", cUserNameBuffer),
							make_document(kvp("ID", "Ken Hu"),
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
					try {
						db["pc_info"].insert_many(Status);
					} catch (...) {
					}
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
						try {
							db_local["alarm"].insert_many(error_docs);
						} catch (...) {
						}
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
						try {
							db_local["alarm"].insert_many(error_docs);
						} catch (...) {
						}
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
					try {
						db_local["alarm"].insert_many(error_docs);
					} catch (...) {
					}
					error_flag = true;
				}
			}
			thread_count--;
		}
	}
}

int main() {
	std::thread *t_error_thread = nullptr;

	mongocxx::instance::current();

	mongocxx::database db2;
	std::string url_data =
			"mongodb://root:pc152@127.0.0.1:27017/admin?authSource=admin";
	std::cout << url_data << std::endl;
	mongocxx::uri uri(url_data);
	mongocxx::client client(uri);
	db2 = client["AFC"];

	mongocxx::database db2_local;
	std::string url_data_local =
			"mongodb://root:pc152@127.0.0.1:27017/admin?authSource=admin";
	std::cout << url_data_local << std::endl;
	mongocxx::uri uri_local(url_data_local);
	mongocxx::client client_local(uri_local);
	db2_local = client_local["AFC_local"];

	/*----------------------------------*/
	/*測試參數*/
	bool thread_end = false;
	int thread_count = 0;
	int pcs_read_error_count = 0;
	int HM_error_count = 0;
	std::string HM_eid = "";
	std::string PCS_eid = "";
	bool error_flag2;
	/*----------------------------------*/

	get_current_timestamp(&currentTime);
	t_error_thread = new std::thread(
			[&]() {
				error_thread(std::ref(thread_end), std::ref(thread_count),
						std::ref(pcs_read_error_count),
						std::ref(HM_error_count), HM_eid, PCS_eid, db2,
						db2_local, std::ref(error_flag2));
			});
	while (true) {
		// 休眠1秒
		usleep(1000 * 1000);
	}
	t_error_thread->join();
	delete t_error_thread;
}
