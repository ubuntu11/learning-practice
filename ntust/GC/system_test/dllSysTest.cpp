/*
 * dllSysTest.cpp
 *
 *  Created on: 2021年12月14日
 *      Author: XU
 */
#include "dllSysTest.h"
#include <iostream>

#include <modbus/modbus.h>

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/pool.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;
void mongo_time(bsoncxx::v_noabi::document::view doc, std::string point_name,
		double &return_data) {
	try {
		bsoncxx::document::element bson_data = doc[point_name];
		if (bson_data.type() == bsoncxx::type::k_date) {
//			std::cout << bsoncxx::to_json(doc) << std::endl;
			return_data = bson_data.get_date();
//			std::cout<<"return_data :"<<point_name<<" = "<< std::to_string(return_data) <<std::endl;
		} else {
			/* Error*/
		}
	} catch (...) {
	}
}
void mongo_data(bsoncxx::v_noabi::document::view doc, std::string point_name,
		double &return_data) {
	try {
		bsoncxx::document::element bson_data = doc[point_name];
		if (bson_data.type() == bsoncxx::type::k_double) {
			return_data = bson_data.get_double().value;
//			std::cout <<"get_double return_data :"<< return_data << std::endl;
		} else if (bson_data.type() == bsoncxx::type::k_int32) {
			return_data = bson_data.get_int32().value;
//			std::cout <<"get_int32 return_data :"<< return_data << std::endl;
		} else {
			/* Error*/
			return_data = -1;
			std::cout << "else return_data :" << return_data << std::endl;
		}
	} catch (...) {
		return_data = -1;
	}
}
void mongo_string(bsoncxx::v_noabi::document::view doc, std::string point_name,
		std::string &return_data) {
	try {
		bsoncxx::document::element bson_data = doc[point_name];
		if (bson_data.type() == bsoncxx::type::k_utf8) {
			return_data = bson_data.get_utf8().value.to_string();
//			std::cout <<"get_double return_data :"<< return_data << std::endl;
		} else {
			/* Error*/
			return_data = "";
			std::cout << "else return_data :" << return_data << std::endl;
		}
	} catch (...) {
		return_data = "";
	}
}
dllSysTest::dllSysTest() {
	// TODO Auto-generated constructor stub
	mongocxx::instance::current();
}

dllSysTest::~dllSysTest() {
	// TODO Auto-generated destructor stub
}
void dllSysTest::ping(){
	std::string data="ping "+IP;
	system(data.c_str());
}
void dllSysTest::mongo_find_status(double *mongo_return, std::string *mongo_return_str){
	try{
//		std::cout << "mongo_find_status start"  <<std::endl;
		std::string url_data_local = "mongodb://root:pc152@127.0.0.1:27017/admin?authSource=admin";
//		std::cout<<url_data_local<<std::endl;
		mongocxx::uri uri(url_data_local);
		mongocxx::client client(uri);
		mongocxx::database db = client["AFC_local"];
		mongocxx::collection coll = db["status"];
		mongocxx::options::find opts; /*限制搜尋數量*/
		auto order = bsoncxx::builder::stream::document { } << "time" << -1
				<< bsoncxx::builder::stream::finalize;
		opts.sort(order.view());
		opts.limit(20);
		mongocxx::cursor cursor = coll.find(
				make_document(kvp("ID", "GC")), opts);
		int count = 0;
		for (bsoncxx::v_noabi::document::view doc : cursor) {
//			std::cout<< bsoncxx::to_json(doc)<<std::endl;
//			std::cout<< count <<std.::endl;
			try {
				mongo_string(doc, "ID", mongo_return_str[count]);
				mongo_time(doc, "time", mongo_return[count]);
				mongo_string(doc, "event", mongo_return_str[count + 1]);
				mongo_string(doc, "group", mongo_return_str[count + 2]);
				mongo_data(doc, "level", mongo_return[count + 1]);
				mongo_data(doc, "show", mongo_return[count + 2]);
			}catch(...){
			}
//			std::cout << "ID :"  << mongo_return_str[count] <<std::endl;
//			std::cout << "time :"  << mongo_return[count] <<std::endl;
//			std::cout << "event :"  << mongo_return_str[count+1] <<std::endl;
//			std::cout << "group :"  << mongo_return_str[count+2] <<std::endl;
//			std::cout << "level :"  << mongo_return[count+1] <<std::endl;
//			std::cout << "show :"  << mongo_return[count+2] <<std::endl;
			count = count + 3;
		}
	}catch(...){
	}

}
void dllSysTest::mongo_find_alarm(double *mongo_return, std::string *mongo_return_str){
	try{
//		std::cout << "mongo_find_alarm start"  <<std::endl;
		std::string url_data_local = "mongodb://root:pc152@127.0.0.1:27017/admin?authSource=admin";
//		std::cout<<url_data_local<<std::endl;
		mongocxx::uri uri(url_data_local);
		mongocxx::client client(uri);
		mongocxx::database db = client["AFC_local"];
		mongocxx::collection coll = db["alarm"];
		mongocxx::options::find opts; /*限制搜尋數量*/
		auto order = bsoncxx::builder::stream::document { } << "time" << -1
				<< bsoncxx::builder::stream::finalize;
		opts.sort(order.view());
		opts.limit(20);
		mongocxx::cursor cursor = coll.find(
				make_document(kvp("ID", "GC")), opts);
		int count = 0;
		for (bsoncxx::v_noabi::document::view doc : cursor) {
//			std::cout<< bsoncxx::to_json(doc)<<std::endl;
//			std::cout<< count <<std.::endl;
			try {
				mongo_string(doc, "ID", mongo_return_str[count]);
				mongo_time(doc, "time", mongo_return[count]);
				mongo_string(doc, "event", mongo_return_str[count + 1]);
				mongo_string(doc, "group", mongo_return_str[count + 2]);
				mongo_data(doc, "level", mongo_return[count + 1]);
				mongo_data(doc, "show", mongo_return[count + 2]);
			}catch(...){
			}
//			std::cout << "ID :"  << mongo_return_str[count] <<std::endl;
//			std::cout << "time :"  << mongo_return[count] <<std::endl;
//			std::cout << "event :"  << mongo_return_str[count+1] <<std::endl;
//			std::cout << "group :"  << mongo_return_str[count+2] <<std::endl;
//			std::cout << "level :"  << mongo_return[count+1] <<std::endl;
//			std::cout << "show :"  << mongo_return[count+2] <<std::endl;
			count = count + 3;
		}
	}catch(...){
	}

}
bool dllSysTest::mongo(){
	bool start_flag =false;
	std::string fc ="None";

	printf("dllSysTest::mongo run\n");
	fflush(stdout);
//	while (true){
//		if (start_flag==true){
//			break;
//		}
//		printf("dllSysTest run\n");
//		fflush(stdout);
//		Sleep(1);
//	}
//	if (fc=="test_mongo"){

	try{
		std::string url_data="mongodb://"+user+":"+pwd+"@"+IP+":"+std::to_string(mongo_port)+"/admin?authSource=admin";
		std::cout<<url_data<<std::endl;
		mongocxx::uri uri(url_data);
		mongocxx::client client(uri);
		mongocxx::database db = client["AFC"];
		db["inventory"].insert_one(make_document(
					kvp("item", "canvas"),
					kvp("qty", 100),
					kvp("tags", make_array("cotton")),
					kvp("size", make_document(kvp("h", 28), kvp("w", 35.5), kvp("uom", "cm")))
					)
				);
		if (db["inventory"].count_documents({}) == 0) {
			throw std::logic_error("wrong count in example 1");
			return false;
		}

//		auto order =bsoncxx::builder::stream::document{} << "time" << -1  <<bsoncxx::builder::stream::finalize;
//		mongocxx::options::find opts; //限制搜尋數量
//		opts.sort(order.view());
//		opts.limit(1);
//		mongocxx::collection coll;
//		coll = db["mbms"];
//		auto cursor = coll.find(make_document(kvp("ID", "12345abc")), opts);
//		for(auto&& doc : cursor){
//			std::cout<< bsoncxx::to_json(doc)<<std::endl;
//			try{
//				bsoncxx::document::element now_soc_bson = doc["soc"];
//				std::cout << " wwwwwwwww "<< std::endl;
//				double now_soc = now_soc_bson.get_double().value;
//				std::cout << " SOC : "<< now_soc << std::endl;
//			}
//			catch (...) {
//				std::cout << "SoC point data error"  <<std::endl;
//			}
//		}

		return true;
	}
	catch(...){
		return false;
	}

//	}
}


