/*
 * dllgc.h
 *
 *  Created on: 2021年12月13日
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
class AbilityTest {
public:
	class dReg025_1 {
	public:
		int time = 36;
		int run_feq = 30;
		double data[36] = {	60, 60.01, 60, 59.99,
							60, 60.02, 60, 59.98,
							60, 60.07, 60, 59.93,
							60, 60.14, 60, 59.86,
							60, 60.20, 60, 59.80,
							60, 60.25, 60, 59.75,
							60, 60.30, 60, 59.70,
							60, 60.40, 60, 59.60,
							60, 60.50, 60, 59.50 };
		int total_time = 1079;
	};
	class dReg025_2 {
	public:
		int time = 60;
		int run_feq = 1;
		double data[61] = {	60.30, 60.29, 60.28, 60.27,
							60.26, 60.25, 60.24, 60.23,
							60.22, 60.21, 60.20, 60.19,
							60.18, 60.17, 60.16, 60.15,
							60.14, 60.13, 60.12, 60.11,
							60.10, 60.09, 60.08, 60.07,
							60.06, 60.05, 60.04, 60.03,
							60.02, 60.01,
							60.00,
							59.99, 59.98,
							59.97, 59.96, 59.95, 59.94,
							59.93, 59.92, 59.91, 59.90,
							59.89, 59.88, 59.87, 59.86,
							59.85, 59.84, 59.83, 59.82,
							59.81, 59.80, 59.79, 59.78,
							59.77, 59.76, 59.75, 59.74,
							59.73, 59.72, 59.71, 59.70 };
		int total_time = 60;
	};
	class dReg025_3 {
	public:
		int time = 60;
		int run_feq = 1;
		double data[61] = {	59.70, 59.71, 59.72, 59.73,
							59.74, 59.75, 59.76, 59.77,
							59.78, 59.79, 59.80, 59.81,
							59.82, 59.83, 59.84, 59.85,
							59.86, 59.87, 59.88, 59.89,
							59.90, 59.91, 59.92, 59.93,
							59.94, 59.95, 59.96, 59.97,
							59.98, 59.99,
							60.00,
							60.01, 60.02,
							60.03, 60.04, 60.05, 60.06,
							60.07, 60.08, 60.09, 60.10,
							60.11, 60.12, 60.13, 60.14,
							60.15, 60.16, 60.17, 60.18,
							60.19, 60.20, 60.21, 60.22,
							60.23, 60.24, 60.25, 60.26,
							60.27, 60.28, 60.29, 60.30 };
		int total_time = 60;
	};
	class dReg025_4 {
	public:
		int time[3] = {0, 60, 1050};
		double data[3] = {60, 59.70, 60};
		int total_time = 1059;
	};
	class dReg025_5 {
	public:
		int time[3] = {0, 60, 1050};
		double data[3] = {60, 60.03, 60};
		int total_time = 1059;
	};
	class dReg05_6 {
	public:
		int time = 36;
		int run_feq = 30;
		double data[36] = {	60, 60.01, 60, 59.99,
							60, 60.02, 60, 59.98,
							60, 60.10, 60, 59.90,
							60, 60.25, 60, 59.75,
							60, 60.40, 60, 59.60,
							60, 60.50, 60, 59.50,
							60, 60.60, 60, 59.40,
							60, 60.70, 60, 59.30,
							60, 60.80, 60, 59.20 };
		int total_time = 1079;
	};
	class dReg05_7 {
	public:
		int time = 60;
		int run_feq = 1;
		double data[61] = {	60.60, 60.58, 60.56, 60.54,
							60.52, 60.50, 60.48, 60.46,
							60.44, 60.42, 60.40, 60.38,
							60.36, 60.34, 60.32, 60.30,
							60.28, 60.26, 60.24, 60.22,
							60.20, 60.18, 60.16, 60.14,
							60.12, 60.10, 60.08, 60.06,
							60.04, 60.02,
							60.00,
							59.98, 59.96,
							59.94, 59.92, 59.90, 59.88,
							59.86, 59.84, 59.82, 59.80,
							59.78, 59.76, 59.74, 59.72,
							59.70, 59.68, 59.66, 59.64,
							59.62, 59.60, 59.58, 59.56,
							59.54, 59.52, 59.50, 59.48,
							59.46, 59.44, 59.42, 59.40 };
		int total_time = 60;
	};
	class dReg05_8 {
	public:
		int time = 60;
		int run_feq = 1;
		double data[61] = {	59.40, 59.42, 59.44, 59.46,
							59.48, 59.50, 59.52, 59.54,
							59.56, 59.58, 59.60, 59.62,
							59.64, 59.66, 59.68, 59.70,
							59.72, 59.74, 59.76, 59.78,
							59.80, 59.82, 59.84, 59.86,
							59.88, 59.90, 59.92, 59.94,
							59.96, 59.98,
							60.00,
							60.02, 60.04,
							60.06, 60.08, 60.10, 60.12,
							60.14, 60.16, 60.18, 60.20,
							60.22, 60.24, 60.26, 60.28,
							60.30, 60.32, 60.34, 60.36,
							60.38, 60.40, 60.42, 60.44,
							60.46, 60.48, 60.50, 60.52,
							60.54, 60.56, 60.58, 60.60 };
		int total_time = 60;
	};
	class dReg05_9 {
	public:
		int time[3] = {0, 60, 1050};
		double data[3] = {60, 59.40, 60};
		int total_time = 1059;
	};
	class dReg05_10 {
	public:
		int time[3] = {0, 60, 1050};
		double data[3] = {60, 60.70, 60};
		int total_time = 1059;
	};
	class sReg_11 {
	public:
		int time = 20;
		int run_feq = 30;
		double data[20] = {	60.00, 59.96, 60.00, 59.92,
							60.00, 59.88, 60.00, 59.84,
							59.92, 59.88, 59.96, 59.92,
							59.98, 60.00, 59.80, 60.00,
							59.70, 60.00, 59.60, 59.98};
		int total_time = 599;
	};
	class sReg_12 {
	public:
		int time = 60;
		int run_feq = 1;
		double data[61] = {60.00,
							59.99, 59.98,
							59.97, 59.96, 59.95, 59.94,
							59.93, 59.92, 59.91, 59.90,
							59.89, 59.88, 59.87, 59.86,
							59.85, 59.84, 59.83, 59.82,
							59.81, 59.80, 59.79, 59.78,
							59.77, 59.76, 59.75, 59.74,
							59.73, 59.72, 59.71,
							59.70,
							59.71, 59.72, 59.73,
							59.74, 59.75, 59.76, 59.77,
							59.78, 59.79, 59.80, 59.81,
							59.82, 59.83, 59.84, 59.85,
							59.86, 59.87, 59.88, 59.89,
							59.90, 59.91, 59.92, 59.93,
							59.94, 59.95, 59.96, 59.97,
							59.98, 59.99,
							60.00 };
		int total_time = 60;
	};
	class sReg_13 {
	public:
		int time[3] = {0, 59, 1050};
		double data[3] = {60, 59.88, 60};
		int total_time = 1059;
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
