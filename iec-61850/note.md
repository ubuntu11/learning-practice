- IEC-61850-7-4 P45 GGIO有IntIn(Integer status input)和Ind(general indication)兩種
status information，前者出現在輔助服務傳輸格式說明中的AI，後者則出現在DI。但IntIn1、IntIn2
裡的數字index要如何設定？
- 在7-1裡用GGIO做搜尋，找到幾個例子，看來DO名就直接寫Ind1
- LD/GROGGIO01.AnIn1.mag.i、LD/GROGGIO01.AnIn2.mag.i標註為Unix Timestamp-H與Unix Timestamp-L，
參考[How Do I Calculate the Timestamp in UTC in an Enhanced Packet Block?](https://ask.wireshark.org/question/15177/how-do-i-calculate-the-timestamp-in-utc-in-an-enhanced-packet-block/)，所謂的High和Low是64 bits timestmp的前32與後32
