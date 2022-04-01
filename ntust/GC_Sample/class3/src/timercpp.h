#include <iostream>
#include <thread>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <windows.h>
SYSTEMTIME currentTime;

class Timer {
	bool clear = false;
public:
	std::string test_string;
	template<typename T1, typename T2>
	void setFixedTime_AFG(T1 control_thread, T2 HM_link_thread);
	void stop();
};
template<typename T1, typename T2>
void Timer::setFixedTime_AFG(T1 control_thread, T2 HM_link_thread) {
	this->clear = false;
	std::thread t([=]() {
		std::cout<<"timercpp.h"<<std::endl;
		int thread_count = 0;
		bool thread_end = false;
		int lock_buffer_10ms = int(currentTime.wMilliseconds / 10);
		int lock_buffer_100ms = int(currentTime.wMilliseconds / 100);
		int lock_buffer_1s = int(currentTime.wSecond);
		bool next_sec = false;
		while (uint16_t(currentTime.wMilliseconds / 100) != 9) {
			if (this->clear)
				return;
			GetSystemTime(&currentTime);
		}
		;
		std::thread *t_HM_link_thread = nullptr;
		t_HM_link_thread = new std::thread([&]() {
			HM_link_thread(std::ref(thread_end), std::ref(thread_count));
		});
		while (true) {
			while (lock_buffer_10ms
					== int(currentTime.wMilliseconds / 10)) { //每10ms跳出去
				if (this->clear) {
					thread_end = true;
					std::cout << "AFC_run STOP start" << std::endl;
					t_HM_link_thread->join();
					delete t_HM_link_thread;
					std::cout << "AFC_run STOP end" << std::endl;
					return;
				}
				GetSystemTime(&currentTime);
				Sleep(10);
			}
			lock_buffer_10ms = int(currentTime.wMilliseconds / 10);

			if (lock_buffer_100ms
					!= int(currentTime.wMilliseconds / 100)) { //換0.1秒
				lock_buffer_100ms = int(
						currentTime.wMilliseconds / 100);

				if (lock_buffer_1s != int(currentTime.wSecond)) { //是否換秒
					lock_buffer_1s = int(currentTime.wSecond);
					next_sec = true;
				}
				std::thread *t_control_thread = nullptr;
				t_control_thread = new std::thread(
					[&]() {
						control_thread(std::ref(thread_end),
								std::ref(thread_count));
					});

				t_control_thread->join();
				delete t_control_thread;
			}
		}
	});
	t.detach();
}
void Timer::stop() {
	this->clear = true;
}
