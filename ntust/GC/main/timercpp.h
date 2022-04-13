#include <iostream>
#include <thread>
#include <chrono>
#include <fstream>
#include <iomanip>

SYSTEMTIME currentTime_start_new;
SYSTEMTIME currentTime_start_d;
SYSTEMTIME currentTime_end;
SYSTEMTIME currentTime;

void time_print_m(std::string data = "", int mod = 1) { // mod = 1 2021/11/10 19:10:58 000
	GetSystemTime(&currentTime);
	if (mod == 1) {
		std::cout << data << " : " << currentTime.wYear << "/" << std::setw(2)
				<< std::setfill('0') << currentTime.wMonth << "/"
				<< std::setw(2) << std::setfill('0') << currentTime.wDay << " "
				<< std::setw(2) << std::setfill('0') << currentTime.wHour << ":"
				<< std::setw(2) << std::setfill('0') << currentTime.wMinute
				<< ":" << std::setw(2) << std::setfill('0')
				<< currentTime.wSecond << " " << std::setw(3)
				<< std::setfill('0') << currentTime.wMilliseconds << std::endl;
	} else {
		std::cout << "time_print mod setting error" << std::endl;
	}
}
class Timer {
	bool clear = false;

public:
	std::string ability_test;
	double pcs_connect_local;
	double pcs_reset_local;
	double pcs_control_time_local;
	template<typename T1, typename T2, typename T3, typename T4, typename T5>
	void setFixedTime_AFG(T1 control_thread, T2 HM_link_thread, T3 error_thread,
			T4 PCS_ReadData_thread, T5 EMS_ReadData_thread,
			std::string set_source,
			double *emsdata, double *mbmsdata, double *HMdata, double *pcsdata,
			int mode, double scale, double soc_max, double soc_min,
			double System_p_max, double System_p_min, double System_q_max,
			double System_q_min, double *f_arr, double *p_arr, double *v_arr,
			double *q_arr, double PQ_p_ref, double PQ_q_ref, int FP_type,
			double FP_soc_goal_percent, double FP_main_f_set,
			double FP_line_p_base, double FP_soc_dead_band,
			double FP_soc_adjust, int VC_type, double Vq_v_base,
			double Vq_q_base, double Vq_droop, double Vq_v_set,
			double PF_pf_ref, modbus_t *HM_ctx, int HM_ret, double HM_f,
			double HM_p, double HM_v, double HM_q, modbus_t *pcs_read_ctx,
			int pcs_read_ret, double &a1, double &b1, double &a21, double &b21,
			double &a22, double &b22, double &a23, double &b23, double &b3,
			double &a41, double &b41, double &a42, double &b42, double &a43,
			double &b43, double &a5, double &b5, double &a_sReg, double &b_sReg,
			double &a_qAuto, double &b_qAuto, std::string MBMS_eid,
			std::string HM_eid, std::string PCS_eid, std::string pcs_ip,
			int pcs_port, int pcs_id, std::string HM_ip, int HM_port, int HM_id,
			mongocxx::database db, mongocxx::database db2,
			mongocxx::database db4, mongocxx::database db_local,
			mongocxx::database db2_local, mongocxx::database db4_local,
			mongocxx::database db5_local);
	void stop();

};
template<typename T1, typename T2, typename T3, typename T4, typename T5>
void Timer::setFixedTime_AFG(T1 control_thread, T2 HM_link_thread,
		T3 error_thread, T4 PCS_ReadData_thread, T5 EMS_ReadData_thread,
		std::string set_source,double *emsdata,
		double *mbmsdata, double *HMdata, double *pcsdata, int mode,
		double scale, double soc_max, double soc_min, double System_p_max,
		double System_p_min, double System_q_max, double System_q_min,
		double *f_arr, double *p_arr, double *v_arr, double *q_arr,
		double PQ_p_ref, double PQ_q_ref, int FP_type,
		double FP_soc_goal_percent, double FP_main_f_set, double FP_line_p_base,
		double FP_soc_dead_band, double FP_soc_adjust, int VC_type,
		double Vq_v_base, double Vq_q_base, double Vq_droop, double Vq_v_set,
		double PF_pf_ref, modbus_t *HM_ctx, int HM_ret, double HM_f,
		double HM_p, double HM_v, double HM_q, modbus_t *pcs_read_ctx,
		int pcs_read_ret, double &a1, double &b1, double &a21, double &b21,
		double &a22, double &b22, double &a23, double &b23, double &b3,
		double &a41, double &b41, double &a42, double &b42, double &a43,
		double &b43, double &a5, double &b5, double &a_sReg, double &b_sReg,
		double &a_qAuto, double &b_qAuto, std::string MBMS_eid,
		std::string HM_eid, std::string PCS_eid, std::string pcs_ip,
		int pcs_port, int pcs_id, std::string HM_ip, int HM_port, int HM_id,
		mongocxx::database db, mongocxx::database db2, mongocxx::database db4,
		mongocxx::database db_local, mongocxx::database db2_local,
		mongocxx::database db4_local, mongocxx::database db5_local) {
	this->clear = false;
	std::thread t([=]() {

		std::string control_point = "aa";
		GetSystemTime(&currentTime_start_new);
		bool first_run = true;
		bool next_sec = false;

		bool remote_first_run = true;/*防止第一次執行去做local的參數 直到讀到remote*/
		bool function_run = false;

		double soc = 50.0;
		char path_dReg = 'N'; //dReg
		char path_sReg = 'N'; //sReg
		bool sReg_flag = false;
		double pcs_out_pre_p;
		double pcs_out_pre_q;

		double pcs_reset;
		double pcs_control;
		double pcs_control_time;
		double pcs_control_time_remote;
		double control_flag = 0;

		int pcs_read_error_count = 0;
		int pcs_write_error_count = 0;
		int HM_error_count = 0;
		int ems_count = 0;
		double mbms_time;
		int mbms_count = 0;
		int ability_test_count = 0;

		bool modbus_link_HM_flag = true;
//		bool modbus_link_pcs_write_flag = true;
		bool modbus_link_pcs_read_flag = true;

		int modea = mode;
//		double scalea = scale;
		double soc_maxa = soc_max;
		double soc_mina = soc_min;
		double System_p_maxa = System_p_max;
		double System_p_mina = System_p_min;
		double System_q_maxa = System_q_max;
		double System_q_mina = System_q_min;
		double f_arra[6] = { f_arr[0], f_arr[1], f_arr[2], f_arr[3], f_arr[4],
				f_arr[5] };
		double p_arra[6] = { p_arr[0], p_arr[1], p_arr[2], p_arr[3], p_arr[4],
				p_arr[5] };
		double v_arra[6] = { v_arr[0], v_arr[1], v_arr[2], v_arr[3], v_arr[4],
				v_arr[5] };
		double q_arra[6] = { q_arr[0], q_arr[1], q_arr[2], q_arr[3], q_arr[4],
				q_arr[5] };

		double PQ_p_refa = PQ_p_ref;
		double PQ_q_refa = PQ_q_ref;
		int FP_typea = FP_type;
		double FP_soc_goal_percenta = FP_soc_goal_percent;
		double FP_main_f_seta = FP_main_f_set;
		double FP_line_p_basea = FP_line_p_base;
		double FP_soc_dead_banda = FP_soc_dead_band;
		double FP_soc_adjusta = FP_soc_adjust;
		int VC_typea = VC_type;
		double Vq_v_basea = Vq_v_base;
		double Vq_q_basea = Vq_q_base;
		double Vq_droopa = Vq_droop;
		double Vq_v_seta = Vq_v_set;
		double PF_pf_refa = PF_pf_ref;

		double pcs_out_p = 0.0; /*P目標值*/
		double pcs_out_q = 0.0; /*Q目標值*/

		bool VQ_Hys_line = false;
		double HM_v_last = 0;
		int scale_len = 1;
		std::string scale_name[scale_len] = { "60f8e79f54841d42146b31df" };
		double scale_data[scale_len];

		double HM_time[10] = { 0 };
		double HM_data_1[41][10];
		double *HM_data[41] = { 0 };
		for (int x = 0; x < 41; x++) {
			HM_data[x] = &HM_data_1[x][0];
		}
		ModbusData::PCS_delta_1 PCS_delta;
		int pcs_read_arr_1[PCS_delta.order][80];
		int *pcs_read_arr[PCS_delta.order];
		for (int x = 0; x < PCS_delta.order; x++) {
			pcs_read_arr[x] = &pcs_read_arr_1[x][0];
		}

		int lock_buffer_10ms = int(currentTime_start_new.wMilliseconds / 10);
		int lock_buffer_100ms = int(currentTime_start_new.wMilliseconds / 100);
		int lock_buffer_1s = int(currentTime_start_new.wSecond);

		int HM_ret_ = HM_ret;
		int pcs_read_ret_ = pcs_read_ret;
//		int pcs_write_ret_ = pcs_write_ret;

		int thread_count = 0;

		bool error_flag1 = false;
		bool error_flag2 = false;
//		bool error_flag3 = false;
		bool error_flag4 = false;
		bool error_flag5 = false;

		double total_p_per = 10 ;
		int pv_len = 600;
		int pv_count = 0;
		bool gd = true;
		double pv_list[600]={1.119885462	,
				1.902578238	,
				18.17623437	,
				1.605961891	,
				1.005157385	,
				1.439822232	,
				19.49256343	,
				18.47581871	,
				19.43915304	,
				18.80887867	,
				1.583651867	,
				19.28176097	,
				3.012019351	,
				19.07286291	,
				18.21149838	,
				1.0521018	,
				19.52470791	,
				1.067421854	,
				3.387277834	,
				18.53306612	,
				19.91131068	,
				18.73386657	,
				1.76997504	,
				18.94801688	,
				3.478158859	,
				18.18442622	,
				3.556723295	,
				18.35399819	,
				18.89741863	,
				19.97253893	,
				18.38559382	,
				1.502634665	,
				18.87258809	,
				1.508838411	,
				3.831438444	,
				1.61461866	,
				1.602804834	,
				1.371794873	,
				19.17915509	,
				18.91633416	,
				1.574876165	,
				1.146801525	,
				19.35301064	,
				18.72572739	,
				1.886976656	,
				18.96473826	,
				1.96247103	,
				19.91930915	,
				18.05916452	,
				3.579088004	,
				19.75820692	,
				18.20066364	,
				19.9236022	,
				19.66852165	,
				3.996439626	,
				19.01560527	,
				1.574464979	,
				19.9948556	,
				18.24715526	,
				18.91409541	,
				3.512282309	,
				18.51195033	,
				3.55721194	,
				18.46181376	,
				19.53583937	,
				18.04408473	,
				1.585589336	,
				1.860066293	,
				19.2396653	,
				19.55878439	,
				3.501878767	,
				1.518166085	,
				19.6033432	,
				1.132577746	,
				19.30778133	,
				18.1319917	,
				3.845729397	,
				18.86847455	,
				1.586946911	,
				1.040037292	,
				18.88517786	,
				18.29130337	,
				18.8941732	,
				3.066534773	,
				19.38234701	,
				3.865488369	,
				3.5036392	,
				3.122615715	,
				3.41591685	,
				19.94171495	,
				3.166372066	,
				18.24537606	,
				1.479995748	,
				3.696734356	,
				18.09338935	,
				1.004887521	,
				1.88754994	,
				1.729163116	,
				3.127517644	,
				18.78683108	,
				1.009004177	,
				1.940208159	,
				19.75398679	,
				19.6251967	,
				18.57503358	,
				1.090402943	,
				18.99285749	,
				19.58380573	,
				18.88908731	,
				18.05406556	,
				3.65434301	,
				3.094144765	,
				3.391628327	,
				18.91081757	,
				19.63947773	,
				18.61213714	,
				3.196256933	,
				1.528058186	,
				1.06367645	,
				1.620730756	,
				19.53440827	,
				19.20641492	,
				18.80081481	,
				18.06904832	,
				1.056954185	,
				18.03631315	,
				19.75597047	,
				3.809836566	,
				19.3924119	,
				19.71508554	,
				3.587864157	,
				3.728146645	,
				1.672074191	,
				3.799895282	,
				1.351001354	,
				3.071109579	,
				3.965618685	,
				1.642762926	,
				1.455999806	,
				18.39996771	,
				18.37622063	,
				1.88310843	,
				19.02180472	,
				3.36341201	,
				18.3748063	,
				1.795033851	,
				3.025316959	,
				18.73898666	,
				19.94947148	,
				19.40727736	,
				19.35857251	,
				19.8491264	,
				19.19227532	,
				1.436163841	,
				19.5048309	,
				1.172408636	,
				3.411263585	,
				19.75939393	,
				19.93267308	,
				19.17722328	,
				3.434545387	,
				19.49512636	,
				1.19214394	,
				19.19061994	,
				1.585490673	,
				3.626039303	,
				19.81578442	,
				19.780448	,
				1.377892809	,
				3.080233675	,
				1.114261252	,
				19.44120646	,
				18.39469554	,
				1.833285854	,
				19.71062908	,
				18.58600782	,
				3.339499574	,
				19.070294	,
				3.370685781	,
				1.43245511	,
				19.43469099	,
				3.613114905	,
				18.74273547	,
				3.955721526	,
				1.518926118	,
				18.48211556	,
				18.4196453	,
				3.24691737	,
				3.482319176	,
				1.235252982	,
				18.2301169	,
				3.156700438	,
				19.36525201	,
				1.622256076	,
				3.097021699	,
				19.7091309	,
				19.4602386	,
				18.33061692	,
				18.68430415	,
				3.195317873	,
				19.19317344	,
				3.412215435	,
				1.208657468	,
				3.989326445	,
				19.13817497	,
				1.088624384	,
				18.85499823	,
				3.219438352	,
				3.956486129	,
				18.03316578	,
				18.30941751	,
				19.16257476	,
				19.58651618	,
				3.670291298	,
				1.618450923	,
				1.929463674	,
				3.609181021	,
				1.569033424	,
				19.78453685	,
				18.82780823	,
				19.95205804	,
				19.47861927	,
				19.92000269	,
				18.3428765	,
				18.58340361	,
				19.09045326	,
				19.5964199	,
				1.242085356	,
				18.3536035	,
				18.20383782	,
				19.74284056	,
				19.91955298	,
				18.44981846	,
				19.67525034	,
				1.378816673	,
				1.208865714	,
				19.82901137	,
				3.798677975	,
				1.955412166	,
				1.151715151	,
				1.181566443	,
				3.980262382	,
				19.20774117	,
				19.86396399	,
				18.31431875	,
				18.60992718	,
				3.246210314	,
				1.189792928	,
				3.390318278	,
				18.67328822	,
				1.427924869	,
				18.73874321	,
				19.27994397	,
				19.33892836	,
				18.48420227	,
				3.532949712	,
				3.270111589	,
				3.711729387	,
				19.24547572	,
				18.66488264	,
				18.05069737	,
				19.86797072	,
				1.650053367	,
				18.11585015	,
				3.240221746	,
				1.192966065	,
				1.358084355	,
				3.636682756	,
				19.10554328	,
				1.66935477	,
				3.569199013	,
				19.26867072	,
				1.992154675	,
				19.22349393	,
				19.02017814	,
				18.24877832	,
				1.85109236	,
				18.56519428	,
				18.80143755	,
				18.32098725	,
				1.418117828	,
				19.09773791	,
				18.20177546	,
				18.26046564	,
				19.22810221	,
				18.35878291	,
				19.92983077	,
				18.60077661	,
				1.926353482	,
				1.585090477	,
				19.50203012	,
				19.87775099	,
				1.860793656	,
				1.962607407	,
				19.98074092	,
				1.269029641	,
				18.09725971	,
				18.63950686	,
				1.177219921	,
				18.92805277	,
				19.93758818	,
				1.661307522	,
				3.074374047	,
				18.30515359	,
				18.09391937	,
				3.121671519	,
				3.510055169	,
				3.865924653	,
				18.87702526	,
				3.211402298	,
				18.84278737	,
				18.33088797	,
				18.24733438	,
				1.160662536	,
				1.633759973	,
				3.381080866	,
				19.96433828	,
				18.98995826	,
				19.32843104	,
				1.921688109	,
				18.98210105	,
				1.273396924	,
				3.650199536	,
				3.682468352	,
				18.187278	,
				3.278684105	,
				18.5680922	,
				19.36045106	,
				19.12497085	,
				19.82702561	,
				18.3807825	,
				19.95690238	,
				1.024562857	,
				3.221604921	,
				3.340873323	,
				1.56540415	,
				3.130757814	,
				19.01308042	,
				18.97467852	,
				18.24762668	,
				3.90185373	,
				3.410960407	,
				18.56400085	,
				18.70234053	,
				18.33179156	,
				18.59571792	,
				3.933158747	,
				18.27221023	,
				18.9988575	,
				3.495428112	,
				3.571251913	,
				19.00795436	,
				1.425590835	,
				19.62290992	,
				1.851275478	,
				19.15257959	,
				18.13946362	,
				19.24150013	,
				3.376267365	,
				3.347454381	,
				3.141288317	,
				1.661274014	,
				19.10076413	,
				3.590075781	,
				3.602579113	,
				1.232917736	,
				18.62242551	,
				1.594172779	,
				18.36625896	,
				3.406244194	,
				18.58105666	,
				1.094741794	,
				18.36185751	,
				3.041878762	,
				1.392697157	,
				18.41522623	,
				19.06284412	,
				1.72181952	,
				19.28026375	,
				18.39817049	,
				18.13522699	,
				1.524329786	,
				1.784413902	,
				19.46858027	,
				18.28613193	,
				18.79083074	,
				3.878423152	,
				1.614817848	,
				3.740174569	,
				19.78510667	,
				1.073051925	,
				3.311245899	,
				18.47669248	,
				1.008431663	,
				1.73801652	,
				18.07157238	,
				3.628100765	,
				19.13652056	,
				18.43169638	,
				3.231687962	,
				18.96872357	,
				18.76878392	,
				19.82013078	,
				1.169413322	,
				1.074685625	,
				19.9675489	,
				3.379947327	,
				1.734374177	,
				1.957970299	,
				3.17171862	,
				1.066232122	,
				18.64243831	,
				19.21910401	,
				3.483672349	,
				19.30043736	,
				19.9734203	,
				3.934217754	,
				3.497810237	,
				18.87328725	,
				18.20420408	,
				1.617614182	,
				19.76089169	,
				1.560899371	,
				3.884875287	,
				19.63735261	,
				1.402195512	,
				18.65950592	,
				19.72223256	,
				1.367075851	,
				18.212904	,
				19.59297636	,
				3.193797412	,
				19.64029984	,
				1.237454639	,
				3.164833626	,
				3.409241585	,
				18.66678666	,
				18.32690767	,
				18.94726992	,
				19.56632852	,
				18.36680634	,
				3.322681287	,
				1.62580179	,
				19.82325965	,
				1.988137782	,
				3.215114627	,
				1.183981095	,
				18.10794088	,
				19.45948328	,
				3.107577515	,
				1.06771747	,
				19.45788807	,
				18.57415924	,
				18.55575308	,
				18.03949485	,
				1.542234971	,
				3.986915233	,
				3.873957469	,
				1.534489182	,
				19.09832068	,
				18.01256951	,
				18.72891431	,
				1.872291908	,
				19.25904222	,
				1.763459906	,
				1.014898847	,
				18.80877334	,
				1.615134942	,
				18.27041323	,
				19.63524483	,
				18.38728699	,
				1.926104598	,
				18.90545591	,
				18.68664942	,
				18.33712093	,
				1.890627153	,
				19.29247944	,
				18.63591773	,
				18.44676814	,
				19.88349981	,
				3.338312368	,
				1.604602812	,
				3.868934708	,
				3.435879781	,
				1.260378501	,
				1.290990506	,
				18.38791597	,
				19.01589491	,
				19.96286261	,
				3.635575547	,
				19.6972705	,
				1.194961677	,
				1.040421565	,
				19.83364006	,
				19.16979924	,
				18.02089137	,
				3.477019697	,
				1.070894137	,
				3.740358936	,
				19.82303883	,
				1.767340964	,
				3.687717647	,
				3.015893411	,
				19.18017738	,
				19.70720232	,
				1.971182312	,
				18.90193997	,
				19.05654965	,
				3.895565886	,
				19.40613639	,
				3.882117733	,
				18.46772113	,
				19.44239271	,
				18.15780759	,
				19.89904138	,
				3.851379739	,
				1.81470115	,
				3.442381943	,
				19.27388895	,
				19.50325476	,
				19.61587722	,
				18.85948211	,
				3.714587009	,
				1.14613772	,
				1.344437551	,
				3.446032802	,
				18.00620315	,
				19.77817503	,
				19.16521063	,
				1.289269224	,
				19.3768545	,
				19.67732178	,
				18.85929059	,
				1.338910232	,
				18.42489541	,
				18.40269053	,
				19.66258963	,
				1.232111827	,
				19.51154498	,
				19.50114719	,
				19.82203314	,
				19.32724527	,
				1.202067345	,
				18.25761753	,
				3.742331784	,
				1.742321894	,
				18.18792741	,
				18.01989129	,
				18.56062243	,
				1.314153016	,
				3.12656838	,
				18.53772544	,
				18.54840466	,
				1.504547101	,
				3.386500824	,
				18.13229157	,
				1.597237467	,
				3.194401039	,
				3.479361638	,
				1.917777545	,
				19.38227278	,
				19.67614576	,
				3.765612936	,
				1.81932562	,
				19.56641408	,
				3.9427408	,
				18.76969282	,
				18.65622748	,
				18.3137857	,
				18.52277625	,
				19.45292624	,
				1.031383787	,
				19.9545839	,
				18.81659069	,
				3.700147795	,
				1.364938618	,
				3.974296005	,
				1.789700266	,
				19.55504585	,
				19.09971	,
				18.6785128	,
				3.880554645	,
				18.36411002	,
				19.06086525	,
				1.524902749	,
				3.441390975	,
				18.69947189	,
				1.019506204	,
				19.52233438	,
				1.207412456	,
				3.484504425	,
				19.44404382	,
				18.2492278	,
				3.90236631	,
				19.46220914	,
				18.26587437	,
				18.9721173	,
				19.62522331	,
				18.56050009	,
				1.529679165	,





		};

		while (uint16_t(currentTime_start_new.wMilliseconds / 100) != 9) {
			if (this->clear)
				return;
			GetSystemTime(&currentTime_start_new);
		}
		;
		if (this->clear)
			return;
		std::thread *t_error_thread = nullptr;
//		std::thread *t_PCS_ReadData_thread = nullptr;
		std::thread *t_EMS_ReadData_thread = nullptr;
		std::thread *t_HM_link_thread = nullptr;
		bool thread_end = false;
		t_HM_link_thread = new std::thread([&]() {
			HM_link_thread(std::ref(thread_end), std::ref(thread_count), std::ref(HM_ctx), std::ref(HM_ret_),
	std::ref(modbus_link_HM_flag), db5_local, std::ref(error_flag5));
});
//		t_error_thread = new std::thread(
//				[&]() {
//					error_thread(std::ref(thread_end), std::ref(thread_count),
//							std::ref(pcs_read_error_count),
//							std::ref(HM_error_count), HM_eid, PCS_eid, db2,
//							db2_local, std::ref(error_flag2));
//				});
//		t_PCS_ReadData_thread = new std::thread(
//				[&]() {
//					PCS_ReadData_thread(
//							std::ref(thread_count),
//							std::ref(pcsdata),
//							std::ref(pcs_read_error_count),
//							pcs_read_arr, pcs_ip, pcs_port, pcs_id,
//							pcs_read_ctx, std::ref(pcs_read_ret_),
//							std::ref(pcs_write_ctx),
//							std::ref(pcs_write_ret_),
//							std::ref(modbus_link_pcs_read_flag),
//							PCS_eid, db3, db3_local);
//				});
		t_EMS_ReadData_thread = new std::thread(
				[&]() {
					EMS_ReadData_thread(std::ref(thread_end),
							std::ref(thread_count), std::ref(emsdata),
							std::ref(mbmsdata), std::ref(remote_first_run),
							set_source, scale_name, scale_data, scale_len,
							std::ref(modea), std::ref(soc_maxa),
							std::ref(soc_mina), std::ref(System_p_maxa),
							std::ref(System_p_mina), std::ref(System_q_maxa),
							std::ref(System_q_mina), std::ref(f_arra),
							std::ref(p_arra), std::ref(v_arra),
							std::ref(q_arra), std::ref(PQ_p_refa),
							std::ref(PQ_q_refa), std::ref(FP_typea),
							std::ref(FP_soc_goal_percenta),
							std::ref(FP_main_f_seta), std::ref(FP_line_p_basea),
							std::ref(FP_soc_dead_banda),
							std::ref(FP_soc_adjusta), std::ref(VC_typea),
							std::ref(Vq_v_basea), std::ref(Vq_q_basea),
							std::ref(Vq_droopa), std::ref(Vq_v_seta),
							std::ref(PF_pf_refa), std::ref(a1), std::ref(b1),
							std::ref(a21), std::ref(b21), std::ref(a22),
							std::ref(b22), std::ref(a23), std::ref(b23),
							std::ref(b3), std::ref(a41), std::ref(b41),
							std::ref(a42), std::ref(b42), std::ref(a43),
							std::ref(b43), std::ref(a5), std::ref(b5),
							std::ref(a_sReg), std::ref(b_sReg), a_qAuto,
							b_qAuto, std::ref(soc), std::ref(ems_count),
							std::ref(mbms_time), std::ref(mbms_count),
							std::ref(pcs_write_error_count),
							std::ref(pcs_reset), std::ref(pcs_control),std::ref(pcs_control_time_remote),
							pcs_read_ctx, std::ref(pcs_read_ret_), MBMS_eid,
							PCS_eid, db4, db4_local, std::ref(error_flag4));
				});
		while (true) {
			while (lock_buffer_10ms
					== int(currentTime_start_new.wMilliseconds / 10)) { //每10ms跳出去
				if (this->clear) {
					thread_end = true;
					std::cout << "AFC_run STOP start" << std::endl;
					t_HM_link_thread->join();
					t_error_thread->join();
//					t_PCS_ReadData_thread->join();
					t_EMS_ReadData_thread->join();
					delete t_HM_link_thread;
					delete t_error_thread;
//					delete t_PCS_ReadData_thread;
					delete t_EMS_ReadData_thread;
					std::cout << "AFC_run STOP end" << std::endl;
					return;
				}
				GetSystemTime(&currentTime_start_new);
				Sleep(10);
			}
			;
			lock_buffer_10ms = int(currentTime_start_new.wMilliseconds / 10);
//			std::cout <<"lock_buffer_10ms :"<< lock_buffer_10ms <<" xxx : "<<int(currentTime_start_new.wMilliseconds/10)<< std::endl;


			if (lock_buffer_100ms
					!= int(currentTime_start_new.wMilliseconds / 100)) { //換0.1秒

				lock_buffer_100ms = int(
						currentTime_start_new.wMilliseconds / 100);

				if (lock_buffer_1s != int(currentTime_start_new.wSecond)) { //是否換秒
					std::cout <<"lock_buffer_1s :"<< lock_buffer_1s <<" xxx : "<<int(currentTime_start_new.wSecond)<< std::endl;
					lock_buffer_1s = int(currentTime_start_new.wSecond);
					next_sec = true;
					std::thread *t_control_thread = nullptr;
										t_control_thread = new std::thread(
												[&]() {
													control_thread(std::ref(function_run),
															std::ref(thread_count), remote_first_run,
															set_source, pcs_connect_local,
															pcs_reset_local, pcs_control_time_local,std::ref(HMdata),
															std::ref(next_sec), std::ref(ability_test),
															std::ref(ability_test_count),
															std::ref(lock_buffer_100ms),
															std::ref(HM_error_count),
															std::ref(first_run), std::ref(path_dReg),
															std::ref(path_sReg), std::ref(sReg_flag),
															std::ref(pcs_out_pre_p),
															std::ref(pcs_out_pre_q), std::ref(HM_time),
															std::ref(HM_data), ems_count, mbms_count,
															soc, HM_ctx, std::ref(HM_ret_),
															std::ref(modbus_link_HM_flag), HM_f, HM_p,
															HM_v, HM_q, std::ref(pcs_out_p), PQ_p_refa,
															std::ref(pcs_out_q), PQ_q_refa, modea,
															soc_maxa, soc_mina, System_p_maxa,
															System_p_mina, System_q_maxa, System_q_mina,
															f_arra, p_arra, v_arra, q_arra, FP_typea,
															VC_typea, FP_soc_goal_percenta,
															FP_soc_dead_banda, PF_pf_refa, Vq_v_basea,
															Vq_q_basea, Vq_droopa, Vq_v_seta, a1, b1,
															a21, b21, a22, b22, a23, b23, b3, a41, b41,
															a42, b42, a43, b43, a5, b5, a_sReg, b_sReg,
															a_qAuto, b_qAuto, std::ref(VQ_Hys_line),
															std::ref(HM_v_last),
															std::ref(control_point), HM_eid, db,
															db_local, std::ref(error_flag1),
															std::ref(pcsdata),
															std::ref(pcs_reset),
															std::ref(pcs_control),std::ref(control_flag),std::ref(pcs_control_time_remote),std::ref(pcs_control_time),
															std::ref(pcs_read_error_count),
															pcs_read_arr, pcs_ip, pcs_port, pcs_id,
															pcs_read_ctx, std::ref(pcs_read_ret_),
															std::ref(modbus_link_pcs_read_flag),
															PCS_eid,pv_len,pv_list,std::ref(pv_count),std::ref(gd),std::ref(total_p_per));
												});

										t_control_thread->join();
										delete t_control_thread;
				}
//				time_print_m("control_thread running", 1);
//				std::cout <<"next_sec :"<< next_sec << std::endl;
//				std::cout <<"modea :"<< modea << std::endl;
//				std::cout <<"control_flag :"<< control_flag << std::endl;



//				std::cout <<"thread_count :"<< thread_count <<std::endl;
			}
		}

	});

	t.detach();
}
void Timer::stop() {
	this->clear = true;
}
