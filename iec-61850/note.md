- IEC-61850-7-4 P45 GGIO有IntIn(Integer status input)和Ind(general indication)兩種
status information，前者出現在輔助服務傳輸格式說明中的AI，後者則出現在DI。但IntIn1、IntIn2
裡的數字index要如何設定？
- 在7-1裡用GGIO做搜尋，找到幾個例子，看來DO名就直接寫Ind1
- LD/GROGGIO01.AnIn1.mag.i、LD/GROGGIO01.AnIn2.mag.i標註為Unix Timestamp-H與Unix Timestamp-L，
參考[How Do I Calculate the Timestamp in UTC in an Enhanced Packet Block?](https://ask.wireshark.org/question/15177/how-do-i-calculate-the-timestamp-in-utc-in-an-enhanced-packet-block/)，所謂的High和Low是64 bits timestmp的前32與後32
- `sudo make INSTALL_PREFIX=/usr/local install`解決iec61850_server.h等unresolved inclusion的問題
> mkdir -p /usr/local/include
mkdir -p /usr/local/lib
cp hal/inc/hal_time.h  hal/inc/hal_thread.h hal/inc/hal_filesystem.h hal/inc/tls_config.h hal/inc/lib_memory.h src/common/inc/libiec61850_common_api.h src/common/inc/linked_list.h src/iec61850/inc/iec61850_client.h src/iec61850/inc/iec61850_common.h src/iec61850/inc/iec61850_server.h src/iec61850/inc/iec61850_model.h src/iec61850/inc/iec61850_cdc.h src/iec61850/inc/iec61850_dynamic_model.h src/iec61850/inc/iec61850_config_file_parser.h src/mms/inc/mms_value.h src/mms/inc/mms_common.h src/mms/inc/mms_types.h src/mms/inc/mms_type_spec.h src/mms/inc/mms_client_connection.h src/mms/inc/iso_connection_parameters.h src/goose/goose_subscriber.h src/goose/goose_receiver.h src/goose/goose_publisher.h src/sampled_values/sv_subscriber.h src/sampled_values/sv_publisher.h src/logging/logging_api.h /usr/local/include
cp ./build/libiec61850.a /usr/local/lib
- 執行'./bess_server -p 8080'，出現：
  > ./bess_server: error while loading shared libraries: libmodbus.so.5: cannot open shared object file: No such file or directory
'
sudo ldconfig
sudo ldconfig -v | grep libmodbus
