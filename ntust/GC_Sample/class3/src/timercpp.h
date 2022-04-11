#include <iostream>
#include <thread>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <ctime>
#include <math.h>
#include <unistd.h>

class Timer {
	bool clear = false;
public:
	std::string test_string;
	template<typename T1, typename T2>
	void setFixedTime_AFG(T1 control_thread, T2 HM_link_thread);
	void stop();
};

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
	return (ts.tv_sec*1000 + lround(ts.tv_nsec/1e6)) % 1000;
}

template<typename T1, typename T2>
void Timer::setFixedTime_AFG(T1 control_thread, T2 HM_link_thread) {
	struct timespec currentTime;
	get_current_timestamp(&currentTime);
	this->clear = false;
	std::thread t([=]() {
		std::cout<<"timercpp.h"<<std::endl;
		int thread_count = 0;
		bool thread_end = false;
		int lock_buffer_10ms = int(get_milliseconds(currentTime) / 10);
		int lock_buffer_100ms = int(get_milliseconds(currentTime) / 100);
		int lock_buffer_1s = int(get_seconds(currentTime));
		bool next_sec = false;
		while (uint16_t(get_milliseconds(currentTime) / 100) != 9) {
			if (this->clear)
				return;
			get_current_timestamp(&currentTime);
		}
		;
		//高壓表thread
		std::thread *t_HM_link_thread = nullptr;
		t_HM_link_thread = new std::thread([&]() {
			HM_link_thread(std::ref(thread_end), std::ref(thread_count));
		});
		while (true) {
			while (lock_buffer_10ms
					== int(get_milliseconds(currentTime) / 10)) { //每10ms跳出去
				if (this->clear) {
					thread_end = true;
					std::cout << "AFC_run STOP start" << std::endl;
					t_HM_link_thread->join();
					delete t_HM_link_thread;
					std::cout << "AFC_run STOP end" << std::endl;
					return;
				}
				get_current_timestamp(&currentTime);
				// sleep 100 毫秒
				usleep(1000 * 10);
			}
			lock_buffer_10ms = int(get_milliseconds(currentTime) / 10);

			if (lock_buffer_100ms
					!= int(get_milliseconds(currentTime) / 100)) { //換0.1秒
				lock_buffer_100ms = int(
						get_milliseconds(currentTime) / 100);

				if (lock_buffer_1s != get_seconds(currentTime)) { //是否換秒
					lock_buffer_1s = get_seconds(currentTime);
					next_sec = true;
				}

				//控制的thread
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
