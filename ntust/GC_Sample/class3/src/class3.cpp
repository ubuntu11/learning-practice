/*程式調度器*/
#include "timercpp.h"
#include <windows.h>
#include <stdio.h>
#include <string>
#include <thread>
void control_thread(bool &thread_end, int &thread_count){
	thread_end++;
	while (true){
		if (thread_end == true)
			return;
		std::cout<<"control_thread"<<std::endl;
		Sleep(1000);
	}
	thread_end--;
}
void HM_link_thread(bool &thread_end, int &thread_count){
	thread_end++;
	while (true){
		if (thread_end == true)
			return;
		std::cout<<"HM_link_thread"<<std::endl;
		Sleep(2000);
	}
	thread_end--;
}
int main() {
	std::string test_string = "None";/*(外部帶入)*/
	bool stopflag = false;/*(外部帶入)*/
	Timer *t = new Timer();
	std::string test_string_buffer = test_string;
	t->test_string = "None";
	t->setFixedTime_AFG(control_thread, HM_link_thread);
	while (true) {
		if (test_string_buffer != test_string) {
			t->test_string = test_string;
			test_string_buffer = test_string;
			std::cout << "test_string change" << std::endl;
		}
		Sleep(100);
		if (stopflag == true) {
			t->stop();
			Sleep(1000);
			break;
		}
	}
	delete t;
	return 0;
}
