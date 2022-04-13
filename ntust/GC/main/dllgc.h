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

	class PCS_delta_1 {
		public:
			int order = 5;
			int fc[5] = { 3 ,3 ,3 ,3, 3};
			int data_list[5][20];
			int Address[5] = { 5395 ,5408, 12598 ,5552 , 12576}, Length[5] = { 2,7, 11 ,4 , 5};

//			int order = 2;
//			int fc[2] = { 3 ,3 };
//			int data_list[2][80];
//			int Address[2] = { 5395 , 12592 }, Length[2] = { 80, 80 };
			class status_pcs {
			public:
				int order = 0, Address = 0, length = 1;
				std::string type = "uint";
				double scale = 1.0;
			};
			class state_qs {
			public:
				int order = 0, Address = 1, length = 1;
				std::string type = "uint";
				double scale = 1.0;
			};
			class p_sum {
			public:
				int order = 2, Address = 4, length = 1;
				std::string type = "int";
				double scale = 10.0;
			};
			class q_sum {
			public:
				int order = 2, Address = 5, length = 1;
				std::string type = "int";
				double scale = 10.0;
			};
			class v1_grid {
			public:
				int order = 1, Address = 4, length = 1;
				std::string type = "int";
				double scale = 10.0;
			};
			class v2_grid {
			public:
				int order = 1, Address = 5, length = 1;
				std::string type = "int";
				double scale = 10.0;
			};
			class v3_grid {
			public:
				int order = 1, Address = 6, length = 1;
				std::string type = "int";
				double scale = 10.0;
			};
			class i1_pcs {
			public:
				int order = 2, Address = 0, length = 1;
				std::string type = "int";
				double scale = 10.0;
			};
			class i2_pcs {
			public:
				int order = 2, Address = 1, length = 1;
				std::string type = "int";
				double scale = 10000.0;
			};
			class i3_pcs {
			public:
				int order = 2, Address = 2, length = 1;
				std::string type = "uint";
				double scale = 100.0;
			};
			class f_grid {
			public:
				int order = 1, Address = 0, length = 1;
				std::string type = "int";
				double scale = 100.0;
			};
			class inner_temp {
			public:
				int order = 2, Address = 9, length = 1;
				std::string type = "int";
				double scale = 1.0;
			};
			class sink_temp {
			public:
				int order = 2, Address = 8, length = 1;
				std::string type = "int";
				double scale = 1.0;
			};
			class v_dc {
			public:
				int order = 2, Address = 6, length = 1;
				std::string type = "int";
				double scale = 10.0;
			};
			class i_dc {
			public:
				int order = 2, Address = 7, length = 1;
				std::string type = "int";
				double scale = 10.0;
			};
			class v_bat {
			public:
				int order = 2, Address = 10, length = 1;
				std::string type = "int";
				double scale = 10.0;
			};
			class dam_fault1 {
						public:
							int order = 4, Address = 0, length = 1;
							std::string type = "int";
							double scale = 1.0;
						};
			class dam_fault2 {
						public:
							int order = 4, Address = 1, length = 1;
							std::string type = "int";
							double scale = 1.0;
						};
			class dam_fault3 {
						public:
							int order = 4, Address = 2, length = 1;
							std::string type = "int";
							double scale = 1.0;
						};
			class dam_fault4 {
						public:
							int order = 4, Address = 3, length = 1;
							std::string type = "int";
							double scale = 1.0;
						};
			class dam_fault5 {
						public:
							int order = 4, Address = 4, length = 1;
							std::string type = "int";
							double scale = 1.0;
						};
			class fault1 {
						public:
							int order = 3, Address = 0, length = 1;
							std::string type = "int";
							double scale = 1.0;
						};
			class fault2 {
						public:
							int order = 3, Address = 1, length = 1;
							std::string type = "int";
							double scale = 1.0;
						};
			class fault3 {
						public:
							int order = 3, Address = 2, length = 1;
							std::string type = "int";
							double scale = 1.0;
						};
			class fault4 {
						public:
							int order = 3, Address = 3, length = 1;
							std::string type = "int";
							double scale = 1.0;
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
