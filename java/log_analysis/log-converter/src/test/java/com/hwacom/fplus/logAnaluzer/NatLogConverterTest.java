package com.hwacom.fplus.logAnaluzer;

import com.hwacom.fplus.logAnalyzer.data.Example;
import com.hwacom.fplus.logAnalyzer.data.NatLogNew;
import org.junit.BeforeClass;
import org.junit.Test;
import java.io.File;
import java.time.ZoneOffset;
import java.util.Arrays;

import static org.junit.Assert.*;

public class NatLogConverterTest {
    private static String csvFileLocation = "";
    private static NatLogConverter converter = new NatLogConverter();
    static{
        ClassLoader classLoader = NatLogConverterTest.class.getClassLoader();
        File file = new File(classLoader.getResource("macbook.pcap-NAT.csv").getFile());
        csvFileLocation = file.getAbsolutePath();
    }

    @BeforeClass
    public static void loadCsvData() {
        converter.convertNatLog(csvFileLocation);
    }

    @Test
    public void testLogNumbers() {
       assertEquals(1779, converter.getLogs().size());
    }

    @Test
    public void testLogUpdateNumbers() {
        assertEquals(2, ((NatLogNew) converter.getLogs().get(1L)).getUpdates().size());
        assertEquals(1, ((NatLogNew) converter.getLogs().get(6L)).getUpdates().size());
    }

    @Test
    public void testTimeboundires() {
        assertEquals(1521086342778l, converter.getEarliestTimestamp().longValue());
        assertEquals(1521097016178l, converter.getLatestTimestamp().longValue());
    }

    @Test
    public void testExampleSize() {
        assertEquals(178, converter.getExamples().length);
        Arrays.stream(converter.getExamples()).forEach(e -> {
            Example ex = (Example) e;
            System.out.println(String.format("Timestamp(%s) - Connections(%s)", ex.getTime() , ex.getConnections()));
        });
    }

    @Test
    public void testConnections() {
        /*
        第一筆樣本的時間落在2018年3月15日星期四 11:59:00 GMT+08:00 1521086340000和
        2018年3月15日星期四 12:00:00 GMT+08:00 1521086400000之間
        該範圍內的connection應有181筆: grep "N,.*15210863[4-9].*"  macbook.pcap-NAT.TXT | wc -l
         */
        assertEquals(181, converter.getExamples()[0].getConnections());
        /*
        第31筆樣本應為2018年3月15日星期四 12:29:00 GMT+08:00 1521088140000和
        2018年3月15日星期四 12:30:00 GMT+08:00 1521088200000
        該範圍內的connection應有148筆: grep "N,.*15210881[4-9].*"  macbook.pcap-NAT.TXT | wc -l
         */
        assertEquals(181, converter.getExamples()[0].getConnections());
        assertEquals(1521088140000l, converter.getExamples()[30].getTime().toEpochSecond(ZoneOffset.ofHours(8)) * 1000);
        assertEquals(148, converter.getExamples()[30].getConnections());
    }
}