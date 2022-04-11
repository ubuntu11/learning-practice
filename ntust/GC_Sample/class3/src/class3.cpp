/**
 * 程式調度器. 示範用自己定義的類別(Timer, in timercpp.h)及裡頭的兩個thread來進行兩
 * 種動作: 查高壓電表與控制PCS.
 *
 * compile時要加-fpermissive, ex: g++ -O0 -g3 -Wall -c -fmessage-length=0 \
 *    -fpermissive -MMD -MP -MF"src/class3.d" -MT"src/class3.o" \
 *    -o "src/class3.o" "../src/class3.cpp"
 *
 * In Eclipse CDT, go to  File->Properties->C/C++ Build->Settings->GCC C++ Compiler
 * ->Miscellaneous, add "-fpermissive". IED will show some errors, but class3.exe
 * is generated and executable.
 * */
#include "timercpp.h"
#include <stdio.h>
#include <string>
#include <thread>
#include <unistd.h>

void control_thread(bool &thread_end, int &thread_count){
	thread_count++;
	while (true){
		if (thread_end == true)
			return;
		std::cout<<"control_thread"<<std::endl;
		// 休眠1秒
		usleep(1000 * 1000);
	}
	thread_count--;
}

void HM_link_thread(bool &thread_end, int &thread_count){
	thread_count++;
	while (true){
		if (thread_end == true)
			return;
		std::cout<<"HM_link_thread"<<std::endl;
		// 休眠2秒
		usleep(1000 * 2000);
	}
	thread_count--;
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

		// 休眠100毫秒
		usleep(1000 * 1000);
		if (stopflag == true) {
			t->stop();
			// 休眠1秒
			usleep(1000 * 1000);
			break;
		}
	}

	delete t;
	return 0;
}
