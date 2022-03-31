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
	/*�򥻰ѼƳ]�w*/
	double total_p_per = 0;/*(�W���B��)PV+PCS*/
	double pv_p_now = 0;/*(Ū��)*/
	double pcs_p_now = 0;/*(�g��)PQ_p_ref*/

	double Smooth_p_variance = 10;/*(����Ѽ�)600��i�ܰʶq*/
	double max_pv = 6000;/*(����Ѽ�)pv�̤j��*/
	double shift_win =1;/*(����Ѽ�)����ɶ�����*/

	double Smooth_p_one;/*(�s���Ѽ�)�C���i�ܰʶq*/


	Smooth_p_one = max_pv * Smooth_p_variance / 100 * shift_win / 60;

	/*���հѼƥN�J*/
	total_p_per = 1000;/*��l��*/
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
