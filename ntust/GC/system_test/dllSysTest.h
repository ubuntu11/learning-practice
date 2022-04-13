/*
 * dllSysTest.h
 *
 *  Created on: 2021¦~12¤ë14¤é
 *      Author: XU
 */
#include <string>
#ifndef DLLSYSTEST_H_
#define DLLSYSTEST_H_

class dllSysTest {
public:
	dllSysTest();
	virtual ~dllSysTest();
	void ping();
	std::string IP = "127.0.0.1";
	std::string user = "root";
	std::string pwd = "pc152";
	int mongo_port = 27017;
	bool mongo();
	void mongo_find_status(double *mongo_return,  std::string *mongo_return_str);
	void mongo_find_alarm(double *mongo_return,  std::string *mongo_return_str);
};

#endif /* DLLSYSTEST_H_ */
