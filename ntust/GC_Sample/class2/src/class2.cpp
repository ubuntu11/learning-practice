/*
 * 用C++ Thread模擬AFC輔助服務裡需要每0.1秒取得電力資料,但以1秒為上傳資料間隔這兩種
 * 需求.
 *
 * Link時需要加上 "-lpthread", ie: g++  -o "class2.exe" ./src/class2.o -lpthread,
 * in Eclipse CDT go to  File->Properties->C/C++ Build->Settings->GCC C++ Linker
 * ->Libraries.
 *
 */

/*線呈開啟*/
#include <iostream>
#include <ctime>
#include <string>
#include <thread>
#include <math.h>
#include <unistd.h>

/**
 * 取得目前系統時間
 * @return current time as struct timespec
 */
struct timespec get_current_timestamp() {
	struct timespec ts;
	timespec_get(&ts, TIME_UTC);
	return ts;
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

/**
 * 在由卢程式建立的一個Thread裡模擬AFC輔助服務中每1秒及每0.1的執行動作.
 *
 * @param thread_end   Thread是否停止
 * @param thread_count Thread數量
 */
void test_thread(bool &thread_end, int &thread_count) {
	struct timespec currentTime = get_current_timestamp();
	int lock_buffer_1s = get_seconds(currentTime);
	int lock_buffer_100ms = int(get_milliseconds(currentTime) / 100);
	std::string fc_name = "control_thread";
	int count = 0;

	// 如果百微秒值在0-8之間且thread沒有停止,則持續檢查系統時間
	while (uint16_t(get_milliseconds(currentTime) / 100) != 9) {
		if (thread_end == true)
			return;
		currentTime = get_current_timestamp();
	}

	// 如果百微秒值每次到達9(ie:1秒1次)
	while (true) {
		while (lock_buffer_100ms == int(get_milliseconds(currentTime) / 100)) {
			if (thread_end == true)
				return;
			currentTime = get_current_timestamp();
			// sleep 100 毫秒
			usleep(1000 * 100);
		}
		lock_buffer_100ms = int(get_milliseconds(currentTime) / 100);
		if (thread_end == true)
			return;
		/*0.1秒動作位置*/
		std::cout << "test_thread 0.1s run :" << lock_buffer_100ms / 10.0
				<< std::endl;
		printf("lock_buffer_1s=%d, current_seconds=%d\n", lock_buffer_1s,
				get_seconds(currentTime));
		if (lock_buffer_1s != get_seconds(currentTime)) {
			lock_buffer_1s = get_seconds(currentTime);
			thread_count++;
			try {
				/*1秒動作位置*/
				std::cout << "test_thread 1s run :" << lock_buffer_1s
						<< std::endl;
				printf("number of threads: %d\n", thread_count);
			} catch (...) {
			}
			thread_count--;
		}

        // 此處為了測試的目地,count累加到30就跳出while(true)
		count++;
		if (count > 30) {
			break;
		}
	}
}

int main() {
	std::cout<<"main start"<<std::endl;
	/*線程參數*/
	bool thread_end = false;
	int thread_count = 0;
	std::thread *t_test_thread = nullptr;
	/*開啟線程*/
	t_test_thread = new std::thread(
		[&]() {
		test_thread(std::ref(thread_end), std::ref(thread_count));
		});

	while (true){
		std::cout<<"main run"<<std::endl;
		usleep(1000 * 1000);
		break;
	}

	t_test_thread->join();
	delete t_test_thread;
	std::cout<<"main end"<<std::endl;
	return 0;
}
