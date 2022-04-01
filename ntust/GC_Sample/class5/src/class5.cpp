#include <iostream>
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
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;
using bsoncxx::builder::basic::sub_array;
void mongo_time(auto doc, std::string point_name, double &return_data){
	try{
		bsoncxx::document::element bson_data = doc[point_name];
		if(bson_data.type() == bsoncxx::type::k_date){
			std::cout << bsoncxx::to_json(doc) << std::endl;
			return_data = bson_data.get_date();
//			std::cout<<"return_data :"<<point_name<<" = "<< std::to_string(return_data) <<std::endl;
		}
		else{
			// Error
		}
	}
	catch(...){}
}
void mongo_data(auto doc, std::string point_name ,double &return_data){
	try{
		bsoncxx::document::element bson_data = doc[point_name];
		if(bson_data.type() == bsoncxx::type::k_double) {
			return_data = bson_data.get_double().value;
		}
		else if(bson_data.type() == bsoncxx::type::k_int32){
			return_data = bson_data.get_int32().value;
		}
		else{
			// Error
			return_data=-1;
		}
	}
	catch(...){
		return_data=-1;
	}
}
void mongo_dic(auto doc, std::string point_name,
		std::string *name, double *data, int scale_len){
	try{
		bsoncxx::document::element bson_data = doc[point_name];
		if(bson_data.type() == bsoncxx::type::k_document){
			bsoncxx::document::view subdoc = bson_data.get_document().value;
			for (int i = 0;i< scale_len;i++){
				bsoncxx::document::element data_buffer = subdoc[name[i]];
				data[i] = data_buffer.get_double().value;
			}
		}
		else{
			for (int i = 0;i< scale_len;i++){
				data[i]=-1;
			}
		}
	}
	catch(...){
		for (int i = 0;i< scale_len;i++){
			data[i]=-1;
		}
	}
}
int main() {


//	mongocxx::instance instance{};
	mongocxx::instance::current();

	mongocxx::database db;
	std::string url_data="mongodb://root:pc152@127.0.0.1:27017/admin?authSource=admin";
	std::cout<<url_data<<std::endl;
	mongocxx::uri uri(url_data);
	mongocxx::client client(uri);
	db = client["AFC"];

	/*----------------------------------*/
	/*測試參數*/
	int scale_len = 1;
	std::string scale_name[scale_len] = { "60f8e79f54841d42146b31df" };
	double scale_data[scale_len];
	double mode;
	double soc_sys;
	std::string MBMS_id = "12345abc";
	std::string PCS_eid = "61e8f76742259b767d248a17";
	/*----------------------------------*/

	/*上傳*/
	time_t now = time(0);
	now += 28800;
	std::vector<bsoncxx::document::value> PCS_docs;
	PCS_docs.push_back(
		make_document(kvp("ID", PCS_eid),
				kvp("time",
						bsoncxx::types::b_date {
								std::chrono::system_clock::from_time_t(
										now) }),
				kvp("v1_grid", 1),
				kvp("v2_grid", 2),
				kvp("v3_grid", 3),
				kvp("i1_grid", 4),
				kvp("i2_grid", 5),
				kvp("i3_grid", 6),
				kvp("p_grid", 7),
				kvp("q_grid", 8),
				kvp("pf_grid", 9)
				));
	db["pcs_test"].insert_many(PCS_docs);
	/*搜尋*/
	mongocxx::options::find opts; /*限制搜尋數量*/
	auto order = bsoncxx::builder::stream::document { } << "time"
			<< -1 << bsoncxx::builder::stream::finalize;
	opts.sort(order.view());
	opts.limit(1);
	mongocxx::collection coll_site = db["site_control"];
	mongocxx::cursor cursor_site = coll_site.find( make_document(kvp("ID", "site_control")),opts);
	int count = 0;
	for(auto&& doc : cursor_site){
		try{
			mongo_dic(doc,"scale",scale_name,scale_data,scale_len);
			mongo_data(doc,"mode",mode);
			count++;
		}
		catch (...) {std::cout<<"error"<<std::endl;}
	}
	mongocxx::collection coll_mbms = db["mbms"];
	mongocxx::cursor cursor_mbms = coll_mbms.find( make_document(kvp("ID", MBMS_id)),opts);
	for(auto&& doc : cursor_mbms){
		try{
			double mbms_time_next;
			mongo_data(doc,"soc_sys",soc_sys);
			mongo_time(doc,"time",mbms_time_next);
		}
		catch (...) {}
	}
	/*更新*/
	db["equipment"].update_one(
		make_document(kvp("_id", bsoncxx::oid(PCS_eid))),
		make_document(kvp("$set",make_document(kvp("count", 0)))));

}
