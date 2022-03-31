#include <iostream>
#include <modbus/modbus.h>
#include "dllgc.h"
void modbus_find(modbus_t *ctx, int *fc, int &ret, bool &read_ok, int **arr,
		int *read_start, int *read_length, int order) {
	for (int y = 0; y < order; y++) {
		if (fc[y] == 3) {
			uint16_t table_16[read_length[y]];

			ret = modbus_read_registers(ctx, read_start[y], read_length[y],
					table_16);
			if (ret > 0) {
				for (int x = 0; x < read_length[y]; x++) {
					arr[y][x] = table_16[x];
//					printf("reg[%d]=%d (0x%X)\n", x, table_16[x], table_16[x]);
//					fflush(stdout);
				}
				read_ok = true;
			} else {
				std::cout << "read_start[y] :" << read_start[y]
						<< " read_length[y] :" << read_length[y] << std::endl;
				std::cout << "modbus dont find data" << std::endl;

				read_ok = false;
			}
		}
		if (fc[y] == 4) {
			uint16_t table_16[read_length[y]];

			ret = modbus_read_input_registers(ctx, read_start[y],
					read_length[y], table_16);
			if (ret > 0) {
				for (int x = 0; x < ret; x++) {
					arr[y][x] = table_16[x];
//					printf("reg[%d]=%d (0x%X)\n", x, table_16[x], table_16[x]);
//					fflush(stdout);
				}
				read_ok = true;
			} else {
				std::cout << "read_start[y] :" << read_start[y]
						<< " read_length[y] :" << read_length[y] << std::endl;
				std::cout << "modbus dont find data" << std::endl;
				read_ok = false;
			}
		}
	}
}
int main() {
	ModbusData::PCS_delta PCS_delta;
	ModbusData::PCS_delta::v1_grid v1_grid;

	std::cout<<PCS_delta.fc[0]<<std::endl;
	std::cout<<v1_grid.Address<<std::endl;

	std::string ip="127.0.0.1";
	int port =502;
	int id =2;
	modbus_t *pcs_read_ctx = modbus_new_tcp(ip.c_str(), port);
	if (pcs_read_ctx == NULL) {
		printf("Unable to create the libmodbus  read context\n");
		fflush(stdout);
	}
	modbus_set_slave(pcs_read_ctx, id);
	modbus_set_response_timeout(pcs_read_ctx, 0, 300000);
	int pcs_read_ret = modbus_connect(pcs_read_ctx);

	bool pcs_read_ok = false;

	int pcs_read_arr_1[PCS_delta.order][80];
	int *pcs_read_arr[PCS_delta.order];
	for (int x = 0; x < PCS_delta.order; x++) {
		pcs_read_arr[x] = &pcs_read_arr_1[x][0];
	}
	modbus_find(pcs_read_ctx, PCS_delta.fc, pcs_read_ret,
			pcs_read_ok, pcs_read_arr, PCS_delta.Address,
			PCS_delta.Length, PCS_delta.order);
	std::cout<<pcs_read_arr[0][0]<<std::endl;

}
