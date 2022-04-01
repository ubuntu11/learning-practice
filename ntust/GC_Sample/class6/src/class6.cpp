#include <iostream>
#include <windows.h>
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


using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;
using bsoncxx::builder::basic::sub_array;
SYSTEMTIME currentTime;
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
void error_thread(bool &thread_end, int &thread_count,
		int &pcs_read_error_count, int &HM_error_count, std::string HM_eid,
		std::string PCS_eid, mongocxx::database db, mongocxx::database db_local,
		bool &error_flag) {
	int lock_buffer_1s = int(currentTime.wSecond);
	int lock_buffer_100ms = int(currentTime.wMilliseconds / 100);
	std::string fc_name = "error_thread";
	bool pcs_error = false;
	bool HM_error = false;
	while (true) {
		while (lock_buffer_100ms
				== int(currentTime.wMilliseconds / 100)) {
			if (thread_end == true)
				return;
			GetSystemTime(&currentTime);
			Sleep(100);
		}
		lock_buffer_100ms = int(currentTime.wMilliseconds / 100);
		if (thread_end == true)
			return;

		if (lock_buffer_1s != int(currentTime.wSecond)) {
			lock_buffer_1s = int(currentTime.wSecond);
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
						printf("The user name is %s \n", cUserNameBuffer);
					} else {
						printf("Get user name failed with error: %lu \n", GetLastError());
					}
					double memory_data = GetTotalPhysicalMemoryUsed();
					double memory_per = GetPhysicalMemoryUsage();
					double cpu = getWin_CpuUsage();
					std::cout<<"nowtime :"<<nowtime<<std::endl;
					std::cout<<"user name :"<<cUserNameBuffer<<std::endl;
					std::cout<<"memory (GB)"<<memory_data<<std::endl;
					std::cout<<"memory (%)"<<memory_per<<std::endl;
					std::cout<<"CPU (%)"<<cpu<<std::endl;

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

int main() {
	std::thread *t_error_thread = nullptr;

	mongocxx::instance::current();

	mongocxx::database db2;
	std::string url_data="mongodb://root:pc152@127.0.0.1:27017/admin?authSource=admin";
	std::cout<<url_data<<std::endl;
	mongocxx::uri uri(url_data);
	mongocxx::client client(uri);
	db2 = client["AFC"];

	mongocxx::database db2_local;
	std::string url_data_local="mongodb://root:pc152@127.0.0.1:27017/admin?authSource=admin";
	std::cout<<url_data_local<<std::endl;
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
	t_error_thread = new std::thread(
		[&]() {
			error_thread(std::ref(thread_end), std::ref(thread_count),
					std::ref(pcs_read_error_count),
					std::ref(HM_error_count), HM_eid, PCS_eid, db2,
					db2_local, std::ref(error_flag2));
		});
	while (true){
		Sleep(1000);
	}
	t_error_thread->join();
	delete t_error_thread;
}
