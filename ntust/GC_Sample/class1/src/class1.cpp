#include <iostream>
void Smooth_mode(double pv_p_now, double &total_p_per, double Smooth_p_one, double &pcs_p_now){
	try{
//		std::cout<<"Smooth_p_one :"<<Smooth_p_one<<std::endl;
		if (pv_p_now > total_p_per + Smooth_p_one){
			pcs_p_now = total_p_per + Smooth_p_one - pv_p_now;
		}
		else if (pv_p_now < total_p_per + Smooth_p_one){
			pcs_p_now = total_p_per - Smooth_p_one - pv_p_now;
		}
		else {
			pcs_p_now = 0;
		}
		total_p_per = pv_p_now + pcs_p_now;
	}
	catch(...){}
}
int main() {
	/*膀セ把计砞﹚*/
	double total_p_per = 0;/*(Ω笲︽)PV+PCS*/
	double pv_p_now = 0;/*(弄)*/
	double pcs_p_now = 0;/*(糶)PQ_p_ref*/

	double Smooth_p_variance = 10;/*(北把计)600跑笆秖*/
	double max_pv = 6000;/*(北把计)pv程*/
	double shift_win =1;/*(北把计)北丁丁*/

	double Smooth_p_one;/*(把计)–Ω跑笆秖*/


	Smooth_p_one = max_pv * Smooth_p_variance / 100 * shift_win / 60;

	/*代刚把计*/
	total_p_per = 1000;/*﹍*/
	std::cout<<"-----------------------------------"<<std::endl;
	pv_p_now = 1020;
	Smooth_mode(pv_p_now, total_p_per, Smooth_p_one, pcs_p_now);
	std::cout<<"pv_p_now :"<<pv_p_now<<std::endl;
	std::cout<<"pcs_p_now :"<<pcs_p_now<<std::endl;
	std::cout<<"total_p :"<<pv_p_now + pcs_p_now<<std::endl;
	std::cout<<"-----------------------------------"<<std::endl;
	pv_p_now = 1100;
	Smooth_mode(pv_p_now, total_p_per, Smooth_p_one, pcs_p_now);
	std::cout<<"pv_p_now :"<<pv_p_now<<std::endl;
	std::cout<<"pcs_p_now :"<<pcs_p_now<<std::endl;
	std::cout<<"total_p :"<<pv_p_now + pcs_p_now<<std::endl;
	std::cout<<"-----------------------------------"<<std::endl;
	pv_p_now = 100;
	Smooth_mode(pv_p_now, total_p_per, Smooth_p_one, pcs_p_now);
	std::cout<<"pv_p_now :"<<pv_p_now<<std::endl;
	std::cout<<"pcs_p_now :"<<pcs_p_now<<std::endl;
	std::cout<<"total_p :"<<pv_p_now + pcs_p_now<<std::endl;
	std::cout<<"-----------------------------------"<<std::endl;
	pv_p_now = 2100;
	Smooth_mode(pv_p_now, total_p_per, Smooth_p_one, pcs_p_now);
	std::cout<<"pv_p_now :"<<pv_p_now<<std::endl;
	std::cout<<"pcs_p_now :"<<pcs_p_now<<std::endl;
	std::cout<<"total_p :"<<pv_p_now + pcs_p_now<<std::endl;
	std::cout<<"-----------------------------------"<<std::endl;
	pv_p_now = 2400;
	Smooth_mode(pv_p_now, total_p_per, Smooth_p_one, pcs_p_now);
	std::cout<<"pv_p_now :"<<pv_p_now<<std::endl;
	std::cout<<"pcs_p_now :"<<pcs_p_now<<std::endl;
	std::cout<<"total_p :"<<pv_p_now + pcs_p_now<<std::endl;
	std::cout<<"-----------------------------------"<<std::endl;
	pv_p_now = 200;
	Smooth_mode(pv_p_now, total_p_per, Smooth_p_one, pcs_p_now);
	std::cout<<"pv_p_now :"<<pv_p_now<<std::endl;
	std::cout<<"pcs_p_now :"<<pcs_p_now<<std::endl;
	std::cout<<"total_p :"<<pv_p_now + pcs_p_now<<std::endl;
	std::cout<<"-----------------------------------"<<std::endl;
	return 0;
}
