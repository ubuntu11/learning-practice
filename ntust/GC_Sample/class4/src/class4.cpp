/*modbus 重新連線*/
#include <iostream>
#include <windows.h>
#include <string>
#include <thread>
#include <modbus/modbus.h>
SYSTEMTIME currentTime;
void HM_link_thread(bool &thread_end, int &thread_count, modbus_t *HM_ctx,
		int &HM_ret, bool &modbus_link_HM_flag, bool &error_flag) {
	int lock_buffer_1s = int(currentTime.wSecond);
	int lock_buffer_100ms = int(currentTime.wMilliseconds / 100);
	std::string fc_name = "HM_link_thread";
	while (true) {
		while (lock_buffer_100ms
				== int(currentTime.wMilliseconds / 100)) {
			if (thread_end == true)
				return;
			GetSystemTime(&currentTime);
			Sleep(100);
		}
		lock_buffer_100ms = int(currentTime.wMilliseconds / 100);
		if (thread_end == true)
			return;
		if (lock_buffer_1s != int(currentTime.wSecond)) {
			lock_buffer_1s = int(currentTime.wSecond);
			thread_count++;
			try {
				if (modbus_link_HM_flag == false and HM_ret == -1) {
					modbus_close(HM_ctx);
					HM_ret = modbus_connect(HM_ctx);
				}
				error_flag = false;
			} catch (...) {
			}
			thread_count--;
		}
	}
}
int main() {
	std::string ip="127.0.0.1";
	int port =502;
	int id =1;
	modbus_t *ctx = modbus_new_tcp(ip.c_str(), port);
	if (ctx == NULL) {
		printf("Unable to create the libmodbus  read context\n");
		fflush(stdout);
	}
	modbus_set_slave(ctx, id);
	modbus_set_response_timeout(ctx, 0, 300000);
	int ret = modbus_connect(ctx);

	uint8_t table_8_1[1];
	uint8_t table_8_2[1];
	uint16_t table_16_1[10];
	uint16_t table_16_2[10];

	modbus_read_bits(ctx, 0, 1, table_8_1);
	modbus_read_input_bits(ctx, 10, 1, table_8_2);
	modbus_read_registers(ctx, 20, 10, table_16_1);
	modbus_read_input_registers(ctx, 30, 10, table_16_2);

	printf("%d\n", table_8_1[0]);
	printf("%d\n", table_8_2[0]);
	printf("%d\n", table_16_1[0]);
	printf("%d\n", table_16_2[0]);

	modbus_write_bit(ctx, 0, 0);
	modbus_write_register(ctx, 20, 2);
	uint8_t write_data_bit[2] = {1, 1};
	uint16_t write_data_register[2] = {20, 30};
	modbus_write_bits(ctx, 1, 2, write_data_bit);
	modbus_write_registers(ctx, 21, 2, write_data_register);
	return 0;
}
