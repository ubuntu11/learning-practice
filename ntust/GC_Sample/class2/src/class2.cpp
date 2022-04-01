/*線呈開啟*/
#include <iostream>
#include <windows.h>
#include <string>
#include <thread>
SYSTEMTIME currentTime;
void test_thread(bool &thread_end, int &thread_count){
	int lock_buffer_1s = int(currentTime.wSecond);
	int lock_buffer_100ms = int(currentTime.wMilliseconds / 100);
	std::string fc_name = "control_thread";
	/*----------------------------------*/
	/*測試參數*/
	int count = 0;
	/*----------------------------------*/
	while (uint16_t(currentTime.wMilliseconds / 100) != 9) {
		if (thread_end == true)
			return;
		GetSystemTime(&currentTime);
	}
	while (true) {
		while (lock_buffer_100ms
				== int(currentTime.wMilliseconds / 100)) {
			if (thread_end == true)
				return;
			GetSystemTime(&currentTime);
			Sleep(10);
		}
		lock_buffer_100ms = int(currentTime.wMilliseconds / 100);
		if (thread_end == true)
			return;
		/*0.1秒動作位置*/
		std::cout << "test_thread 0.1s run :"<< lock_buffer_100ms/10.0 <<std::endl;
		if (lock_buffer_1s != int(currentTime.wSecond)) {
			lock_buffer_1s = int(currentTime.wSecond);
			thread_count++;
			try {
				/*1秒動作位置*/
				std::cout<<"test_thread 1s run :" << lock_buffer_1s <<std::endl;
			}
			catch(...){}
			}
			thread_count--;
		/*----------------------------------*/
		/*測試參數*/
		count++;
		if (count > 30){
			break;
		}
		/*----------------------------------*/
	}
}
int main() {
	std::cout<<"main start"<<std::endl;
	/*線呈參數*/
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
		Sleep(1000);
		/*----------------------------------*/
		/*測試參數*/
		break;
		/*----------------------------------*/
	}
	t_test_thread->join();
	delete t_test_thread;
	std::cout<<"main end"<<std::endl;
	return 0;
}
