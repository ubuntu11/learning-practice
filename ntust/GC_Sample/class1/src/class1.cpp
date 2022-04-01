/*
 * pv_smooth.cpp PV輸出平滑化
 * 修改智電提供的範例程式
 *
 * Created on: 2022年4月1日
 *     Author: ken
 */
#include <iostream>
#include <cstdlib>
#include <ctime>

// PV案場理論最大輸出值(KW)
#define PV_MAX_OUTPUT 6000
// 60秒後可容許PV案場輸出變動率
#define VARIANCE_PERCENTAGE_60S 10
// 平滑化控制動作執行間隔(秒)
#define CONTROL_INTERVAL 1

// function prototype

/**
 * 計算此次PCS輸出值與此次PV案場輸出值.
 *
 * @param pv_p_now     此次PV輸出,由電表讀出
 * @param total_p_per  前次PV案場輸出值
 * @param Smooth_p_one 此次可變動量
 * @param pcs_p_now    此次PCS輸出值
 */
void Smooth_mode(double pv_p_now, double &total_p_per, double Smooth_p_one, double &pcs_p_now);

int main() {
    // 前次PV案場輸出值(=前次PV輸出值 + PCS輸出值)
	double total_p_per = 0;
	// 此次PV輸出,由電表讀出
	double pv_p_now = 0;
	// 此次PCS需輸出值
	double pcs_p_now = 0;
    // 此次可變動量
	double Smooth_p_one;

	Smooth_p_one = PV_MAX_OUTPUT * VARIANCE_PERCENTAGE_60S / 100 * CONTROL_INTERVAL / 60;

	/*測試參數代入*/
	srand( time(NULL) );

	total_p_per = 2000;/*初始值*/
	printf ("%15s, %15s, %15s, %15s\n", "pv_p_now", "pcs_p_now", "mooth_p_one", "total_p_per");
	for (int i=0; i < 86400; i++) {
		pv_p_now = 1000 + (rand() % 1000 - 500);
		Smooth_mode(pv_p_now, total_p_per, Smooth_p_one, pcs_p_now);
		printf ("%15.4f, %15.4f, %15.4f, %15.4f\n", pv_p_now, pcs_p_now, Smooth_p_one, total_p_per);
	}

	return 0;
}

void Smooth_mode(double pv_p_now, double &total_p_per,
		double Smooth_p_one, double &pcs_p_now){
	try{
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
