/*
 * dllgc.h
 *
 *  Created on: 2021¦~12¤ë13¤é
 *      Author: XU
 */

#ifndef DLLGC_H_
#define DLLGC_H_
#include <string>
class ModbusData {
public:
	class PCS_delta {
	public:
		int order = 2;
		int fc[2] = { 4, 3 };
		int data_list[2][80];
		int Address[2] = { 0, 0 }, Length[2] = { 80, 20 };
		class v1_grid {
		public:
			int order = 0, Address = 2, length = 1;
			std::string type = "int";
			double scale = 1.0;
		};
		class v2_grid {
		public:
			int order = 0, Address = 3, length = 1;
			std::string type = "int";
			double scale = 1.0;
		};
		class v3_grid {
		public:
			int order = 0, Address = 4, length = 1;
			std::string type = "int";
			double scale = 1.0;
		};
		class i1_grid {
		public:
			int order = 0, Address = 5, length = 1;
			std::string type = "int";
			double scale = 10.0;
		};
		class i2_grid {
		public:
			int order = 0, Address = 6, length = 1;
			std::string type = "int";
			double scale = 10.0;
		};
		class i3_grid {
		public:
			int order = 0, Address = 7, length = 1;
			std::string type = "int";
			double scale = 10.0;
		};
		class p_grid {
		public:
			int order = 0, Address = 8, length = 1;
			std::string type = "int";
			double scale = 10.0;
		};
		class q_grid {
		public:
			int order = 0, Address = 9, length = 1;
			std::string type = "int";
			double scale = 10.0;
		};
		class pf_grid {
		public:
			int order = 0, Address = 10, length = 1;
			std::string type = "int";
			double scale = 10000.0;
		};
		class f_grid {
		public:
			int order = 0, Address = 11, length = 1;
			std::string type = "uint";
			double scale = 100.0;
		};
		class v1_pcs {
		public:
			int order = 0, Address = 12, length = 1;
			std::string type = "int";
			double scale = 10.0;
		};
		class v2_pcs {
		public:
			int order = 0, Address = 13, length = 1;
			std::string type = "int";
			double scale = 10.0;
		};
		class v3_pcs {
		public:
			int order = 0, Address = 14, length = 1;
			std::string type = "int";
			double scale = 10.0;
		};
		class i1_pcs {
		public:
			int order = 0, Address = 15, length = 1;
			std::string type = "int";
			double scale = 100.0;
		};
		class i2_pcs {
		public:
			int order = 0, Address = 16, length = 1;
			std::string type = "int";
			double scale = 100.0;
		};
		class i3_pcs {
		public:
			int order = 0, Address = 17, length = 1;
			std::string type = "int";
			double scale = 100.0;
		};
		class p_sum {
		public:
			int order = 0, Address = 18, length = 1;
			std::string type = "int";
			double scale = 10.0;
		};
		class q_sum {
		public:
			int order = 0, Address = 19, length = 1;
			std::string type = "int";
			double scale = 10.0;
		};
		class pf {
		public:
			int order = 0, Address = 20, length = 1;
			std::string type = "int";
			double scale = 10000.0;
		};
		class f_pcs_low {
		public:
			int order = 0, Address = 21, length = 1;
			std::string type = "uint";
			double scale = 100.0;
		};
//		class i_BAT12	{public:int order = 0, Address = 22, length = 1; std::string type = "int"; double scale = 1.0;};
//		class i_BAT34	{public:int order = 0, Address = 23, length = 1; std::string type = "int"; double scale = 1.0;};
		class p_dc {
		public:
			int order = 0, Address = 26, length = 1;
			std::string type = "int";
			double scale = 10.0;
		};
//		class v_BAT12	{public:int order = 0, Address = 30, length = 1; std::string type = "int"; double scale = 1.0;};
//		class v_BAT34	{public:int order = 0, Address = 31, length = 1; std::string type = "int"; double scale = 1.0;};
//		class v_DC12	{public:int order = 0, Address = 34, length = 1; std::string type = "uint"; double scale = 1.0;};
//		class v_DC34	{public:int order = 0, Address = 35, length = 1; std::string type = "uint"; double scale = 1.0;};
//		class s			{public:int order = 0, Address = 50, length = 1; std::string type = "uint"; double scale = 1.0;};
//		class v			{public:int order = 0, Address = 51, length = 1; std::string type = "uint"; double scale = 1.0;};
		class status_pcs_1 {
		public:
			int order = 0, Address = 52, length = 1;
			std::string type = "bit";
			double scale = 1.0;
		};
		class status_pcs_2 {
		public:
			int order = 0, Address = 53, length = 1;
			std::string type = "bit";
			double scale = 1.0;
		};
		class status_system {
		public:
			int order = 0, Address = 54, length = 1;
			std::string type = "bit";
			double scale = 1.0;
		};
		class mode_pcs {
		public:
			int order = 0, Address = 55, length = 1;
			std::string type = "bit";
			double scale = 1.0;
		};
		class module1_fault1 {
		public:
			int order = 0, Address = 56, length = 1;
			std::string type = "bit";
			double scale = 1.0;
		};
		class module1_fault2 {
		public:
			int order = 0, Address = 57, length = 1;
			std::string type = "bit";
			double scale = 1.0;
		};
		class module1_fault3 {
		public:
			int order = 0, Address = 58, length = 1;
			std::string type = "bit";
			double scale = 1.0;
		};
		class module2_fault1 {
		public:
			int order = 0, Address = 59, length = 1;
			std::string type = "bit";
			double scale = 1.0;
		};
		class module2_fault2 {
		public:
			int order = 0, Address = 60, length = 1;
			std::string type = "bit";
			double scale = 1.0;
		};
		class module2_fault3 {
		public:
			int order = 0, Address = 61, length = 1;
			std::string type = "bit";
			double scale = 1.0;
		};
		class module3_fault1 {
		public:
			int order = 0, Address = 62, length = 1;
			std::string type = "bit";
			double scale = 1.0;
		};
		class module3_fault2 {
		public:
			int order = 0, Address = 63, length = 1;
			std::string type = "bit";
			double scale = 1.0;
		};
		class module3_fault3 {
		public:
			int order = 0, Address = 64, length = 1;
			std::string type = "bit";
			double scale = 1.0;
		};
		class module4_fault1 {
		public:
			int order = 0, Address = 65, length = 1;
			std::string type = "bit";
			double scale = 1.0;
		};
		class module4_fault2 {
		public:
			int order = 0, Address = 66, length = 1;
			std::string type = "bit";
			double scale = 1.0;
		};
		class module4_fault3 {
		public:
			int order = 0, Address = 67, length = 1;
			std::string type = "bit";
			double scale = 1.0;
		};
		class system_fault1 {
		public:
			int order = 0, Address = 68, length = 1;
			std::string type = "bit";
			double scale = 1.0;
		};
		class system_fault2 {
		public:
			int order = 0, Address = 69, length = 1;
			std::string type = "bit";
			double scale = 1.0;
		};
		class system_fault3 {
		public:
			int order = 0, Address = 70, length = 1;
			std::string type = "bit";
			double scale = 1.0;
		};
		class cooling_fault {
		public:
			int order = 0, Address = 71, length = 1;
			std::string type = "bit";
			double scale = 1.0;
		};
		class fan_fault {
		public:
			int order = 0, Address = 72, length = 1;
			std::string type = "bit";
			double scale = 1.0;
		};
		class system_warning {
		public:
			int order = 0, Address = 73, length = 1;
			std::string type = "bit";
			double scale = 1.0;
		};
//		class SysEff	{public:int order = 0, Address = 74, length = 1; std::string type = "bit"; double scale = 1.0;};
//		class charge	{public:int order = 0, Address = 75, length = 1; std::string type = "bit"; double scale = 1.0;};
//		class discharge	{public:int order = 0, Address = 76, length = 1; std::string type = "bit"; double scale = 1.0;};
		class rst_word {
		public:
			int order = 1, Address = 3, length = 1;
			std::string type = "bit";
			double scale = 1.0;
		};
		class PQ_p_out {
		public:
			int order = 1, Address = 10, length = 1;
			std::string type = "int";
			double scale = 10.0;
		};
		class PQ_q_out {
		public:
			int order = 1, Address = 11, length = 1;
			std::string type = "int";
			double scale = 10.0;
		};
	};
	class Meter_ION9000 {
	public:
		int order = 2;
		int fc[2] = { 3, 3 };
		int data_list[2][80];
		int Address[2] = { 2099, 2699 }, Length[2] = { 72, 24 }; //20999
		class i_a {
		public:
			int order = 0, Address = 0, length = 2;
			std::string type = "float";
			double scale = 1.0;
		};
		class i_b {
		public:
			int order = 0, Address = 2, length = 2;
			std::string type = "float";
			double scale = 1.0;
		};
		class i_c {
		public:
			int order = 0, Address = 4, length = 2;
			std::string type = "float";
			double scale = 1.0;
		};
		class i {
		public:
			int order = 0, Address = 10, length = 2;
			std::string type = "float";
			double scale = 1.0;
		};
		class f {
		public:
			int order = 0, Address = 16, length = 2;
			std::string type = "float";
			double scale = 1.0;
		};
		class vl_ab {
		public:
			int order = 0, Address = 18, length = 2;
			std::string type = "float";
			double scale = 1.0;
		};
		class vl_bc {
		public:
			int order = 0, Address = 20, length = 2;
			std::string type = "float";
			double scale = 1.0;
		};
		class vl_ca {
		public:
			int order = 0, Address = 22, length = 2;
			std::string type = "float";
			double scale = 1.0;
		};
		class vl {
		public:
			int order = 0, Address = 24, length = 2;
			std::string type = "float";
			double scale = 1.0;
		};
		class v_a {
		public:
			int order = 0, Address = 26, length = 2;
			std::string type = "float";
			double scale = 1.0;
		};
		class v_b {
		public:
			int order = 0, Address = 28, length = 2;
			std::string type = "float";
			double scale = 1.0;
		};
		class v_c {
		public:
			int order = 0, Address = 30, length = 2;
			std::string type = "float";
			double scale = 1.0;
		};
		class v {
		public:
			int order = 0, Address = 34, length = 2;
			std::string type = "float";
			double scale = 1.0;
		};
		class p_a {
		public:
			int order = 0, Address = 40, length = 2;
			std::string type = "float";
			double scale = 1000.0;
		};
		class p_b {
		public:
			int order = 0, Address = 42, length = 2;
			std::string type = "float";
			double scale = 1000.0;
		};
		class p_c {
		public:
			int order = 0, Address = 44, length = 2;
			std::string type = "float";
			double scale = 1000.0;
		};
		class p {
		public:
			int order = 0, Address = 46, length = 2;
			std::string type = "float";
			double scale = 1000.0;
		};
		class q_a {
		public:
			int order = 0, Address = 48, length = 2;
			std::string type = "float";
			double scale = 1000.0;
		};
		class q_b {
		public:
			int order = 0, Address = 50, length = 2;
			std::string type = "float";
			double scale = 1000.0;
		};
		class q_c {
		public:
			int order = 0, Address = 52, length = 2;
			std::string type = "float";
			double scale = 1000.0;
		};
		class q {
		public:
			int order = 0, Address = 54, length = 2;
			std::string type = "float";
			double scale = 1000.0;
		};
		class s_a {
		public:
			int order = 0, Address = 56, length = 2;
			std::string type = "float";
			double scale = 1000.0;
		};
		class s_b {
		public:
			int order = 0, Address = 58, length = 2;
			std::string type = "float";
			double scale = 1000.0;
		};
		class s_c {
		public:
			int order = 0, Address = 60, length = 2;
			std::string type = "float";
			double scale = 1000.0;
		};
		class s {
		public:
			int order = 0, Address = 62, length = 2;
			std::string type = "float";
			double scale = 1000.0;
		};
		class pf_a {
		public:
			int order = 0, Address = 64, length = 2;
			std::string type = "float";
			double scale = 1.0;
		};
		class pf_b {
		public:
			int order = 0, Address = 66, length = 2;
			std::string type = "float";
			double scale = 1.0;
		};
		class pf_c {
		public:
			int order = 0, Address = 68, length = 2;
			std::string type = "float";
			double scale = 1.0;
		};
		class pf {
		public:
			int order = 0, Address = 70, length = 2;
			std::string type = "float";
			double scale = 1.0;
		};
		class imp_kwh {
		public:
			int order = 1, Address = 0, length = 2;
			std::string type = "float";
			double scale = 1.0;
		};
		class exp_kwh {
		public:
			int order = 1, Address = 2, length = 2;
			std::string type = "float";
			double scale = 1.0;
		};
		class tot_kwh {
		public:
			int order = 1, Address = 4, length = 2;
			std::string type = "float";
			double scale = 1.0;
		};
		class net_kwh {
		public:
			int order = 1, Address = 6, length = 2;
			std::string type = "float";
			double scale = 1.0;
		};
		class imp_kvarh {
		public:
			int order = 1, Address = 8, length = 2;
			std::string type = "float";
			double scale = 1.0;
		};
		class exp_kvarh {
		public:
			int order = 1, Address = 10, length = 2;
			std::string type = "float";
			double scale = 1.0;
		};
		class tot_kvarh {
		public:
			int order = 1, Address = 12, length = 2;
			std::string type = "float";
			double scale = 1.0;
		};
		class net_kvarh {
		public:
			int order = 1, Address = 14, length = 2;
			std::string type = "float";
			double scale = 1.0;
		};
		class imp_kvah {
		public:
			int order = 1, Address = 16, length = 2;
			std::string type = "float";
			double scale = 1.0;
		};
		class exp_kvah {
		public:
			int order = 1, Address = 18, length = 2;
			std::string type = "float";
			double scale = 1.0;
		};
		class tot_kvah {
		public:
			int order = 1, Address = 20, length = 2;
			std::string type = "float";
			double scale = 1.0;
		};
		class net_kvah {
		public:
			int order = 1, Address = 22, length = 2;
			std::string type = "float";
			double scale = 1.0;
		};
	};
};

class dllgc {
public:
	dllgc();
	virtual ~dllgc();
	void print();
	std::string MBMS_eid;
	int EMSIP;
	std::string ems_redundant = "2";
	std::string IP1 = "127.0.0.1";
	std::string IP2 = "127.0.0.1";
	std::string user = "root";
	std::string pwd = "pc152";
	int mongo_port1 = 27017;
	int mongo_port2 = 27017;
	void AFC_run(bool &stopflag, int &error_code, std::string &ability_test,
			double &pcs_connect_local, double &pcs_reset_local, double &pcs_control_time_local,
			double *emsdata, double *mbmsdata, double *HMdata, double *pcsdata,
			std::string MBMS_eid, std::string HM_eid, std::string PCS_eid,
			std::string set_source, int mode, double scale, double soc_max,
			double soc_min, double System_p_max, double System_p_min,
			double System_q_max, double System_q_min, double *f_arr,
			double *p_arr, double *v_arr, double *q_arr, double PQ_p_ref,
			double PQ_q_ref, int FP_type, double FP_soc_goal_percent,
			double FP_main_f_set, double FP_line_p_base,
			double FP_soc_dead_band, double FP_soc_adjust, int VC_type,
			double Vq_v_base, double Vq_q_base, double Vq_droop,
			double Vq_v_set, double PF_pf_ref, std::string pcs_ip, int pcs_port,
			int pcs_id, std::string HM_ip, int HM_port, int HM_id);


};
#endif /* DLLGC_H_ */
