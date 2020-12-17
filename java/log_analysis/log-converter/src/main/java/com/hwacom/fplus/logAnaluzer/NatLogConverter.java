package com.hwacom.fplus.logAnaluzer;

import com.hwacom.fplus.logAnalyzer.data.Example;
import com.hwacom.fplus.logAnalyzer.data.NatLog;
import com.hwacom.fplus.logAnalyzer.data.NatLogNew;
import com.hwacom.fplus.logAnalyzer.data.NatLogUpdate;
import org.supercsv.cellprocessor.ift.CellProcessor;
import org.supercsv.exception.SuperCsvException;
import org.supercsv.io.CsvListReader;
import org.supercsv.io.ICsvListReader;
import org.supercsv.prefs.CsvPreference;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.sql.Timestamp;
import java.time.LocalDateTime;
import java.time.ZoneOffset;
import java.time.temporal.ChronoUnit;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.stream.IntStream;
import java.util.stream.Stream;

public class NatLogConverter {
    private HashMap<Long, NatLog> logs = new HashMap<>();
    private Long earliestTimestamp = 0L;
    private Long latestTimestamp = 0L;
    private Example[] examples;

    public static void main(String[] args) {
        NatLogConverter converter = new NatLogConverter();
        converter.convertNatLog(args[0]);
    }

    public void convertNatLog(String csvFileLocation) {
        readCsvContent(csvFileLocation).stream().forEach(l -> {
            processLineData(convertLineIntent(l));
        });
        generateExamples();
        outputCsv();
    }

    private void outputCsv() {
        System.out.println("time,connections,txPkts,txBytes,rxPkts,rxBytes");
        Stream.of(this.getExamples()).forEach(e ->
                System.out.println(String.format("%s,%s,%s,%s,%s,%s",e.getTime(),
                        e.getConnections(),e.getTxPkts(),e.getTxBytes(),e.getRxPkts(),e.getRxBytes())));
    }

    private void generateExamples() {
        LocalDateTime beginTime = new Timestamp(this.earliestTimestamp).toLocalDateTime().truncatedTo(ChronoUnit.MINUTES);
        LocalDateTime truncatedBeginTime = beginTime.truncatedTo(ChronoUnit.MINUTES);
        LocalDateTime endTime = new Timestamp(this.latestTimestamp).toLocalDateTime();
        LocalDateTime truncatedEndTime = endTime.truncatedTo(ChronoUnit.MINUTES);

        long range = (endTime.toEpochSecond(ZoneOffset.ofHours(8)) - beginTime.toEpochSecond(ZoneOffset.ofHours(8))) / 60;
        if (endTime.isAfter(truncatedEndTime)) {
            range++;
        }
        examples = new Example[Long.valueOf(range).intValue()];
        for(int i=0; i<examples.length; i++) {
            examples[i] = new Example(truncatedBeginTime.plusMinutes(Integer.valueOf(i).longValue()));
        }

        Arrays.stream(examples).forEach(e -> {
            for(NatLog log : logs.values()) {
                NatLogNew nLog = (NatLogNew) log;
                if(nLog.residesInTimeRange(e.getTime())) {
                    e.addConnection();
                    if (nLog.getDurationInSecond() <= 0) continue;
                    e.addTxPkts(nLog.getTxPkts() / nLog.getDurationInSecond() * 60);
                    e.addTxBytes(nLog.getTxBytes() / nLog.getDurationInSecond() * 60);
                    e.addRxPkts(nLog.getRxPkts() / nLog.getDurationInSecond() * 60);
                    e.addRxBytes(nLog.getRxBytes() / nLog.getDurationInSecond() * 60);
                }
            }
        });
    }

    private void processLineData(NatLog natLog) {
        if(natLog.isNewLog()) {
            logs.put(natLog.getSid(), natLog);
        } else if (logs.containsKey(natLog.getSid())){
            ((NatLogNew) logs.get(natLog.getSid())).addUpdate((NatLogUpdate) natLog);
        }
    }

    private NatLog convertLineIntent(List<Object> lineData) {
        NatLog log = new NatLog();
        log.setLogType((String) lineData.get(0));
        if(log.isNewLog()) {
            return makeNatLogNew(log, lineData);
        } else {
            return makeNatLogUpdate(log, lineData);
        }
    }

    private NatLog makeNatLogUpdate(NatLog log, List<Object> lineData) {
        NatLogUpdate uLog  = new NatLogUpdate();
        uLog.setLogType(NatLog.LOG_TYPE_UPDATE);
        uLog.setSid(Long.parseLong(String.valueOf(lineData.get(1))));
        uLog.setMacAddress(String.valueOf(lineData.get(2)));
        uLog.setTxPkts(Integer.parseInt(String.valueOf(lineData.get(3))));
        uLog.setTxBytes(Integer.parseInt(String.valueOf(lineData.get(4))));
        uLog.setRxPkts(Integer.parseInt(String.valueOf(lineData.get(5))));
        uLog.setRxBytes(Integer.parseInt(String.valueOf(lineData.get(6))));
        uLog.setLastActiveTime(Long.parseLong(String.valueOf(lineData.get(7))));
        computeTimeBoundaries(uLog.getLastActiveTime());
        return uLog;
    }

    private NatLog makeNatLogNew(NatLog log, List<Object> lineData) {
        NatLogNew nLog = new NatLogNew();
        nLog.setLogType(NatLog.LOG_TYPE_NEW);
        nLog.setSid(Long.parseLong(String.valueOf(lineData.get(1))));
        nLog.setPrivateIp(String.valueOf(lineData.get(2)));
        nLog.setPrivatePort(Integer.parseInt(String.valueOf(lineData.get(3))));
        nLog.setPeerIP(String.valueOf(lineData.get(4)));
        nLog.setPeerPort(Integer.parseInt(String.valueOf(lineData.get(5))));
        nLog.setMacAddress(String.valueOf(lineData.get(6)));
        nLog.setOpenTimestamp(Long.parseLong(String.valueOf(lineData.get(7))));
        computeTimeBoundaries(nLog.getOpenTimestamp());
        return nLog;
    }

    private void computeTimeBoundaries(Long timestamp) {
        if(this.earliestTimestamp == 0L || this.earliestTimestamp > timestamp) {
           this.earliestTimestamp = timestamp;
        }
        if(this.latestTimestamp < timestamp) {
            this.latestTimestamp = timestamp;
        }
    }

    private List<List<Object>> readCsvContent(String fileName) {
        List<List<Object>> result = new ArrayList<>();
        int colNum = 8, lineNumber = 0;
        try (ICsvListReader listReader = new CsvListReader(
                new FileReader(new File(fileName)),
                CsvPreference.STANDARD_PREFERENCE)) {

            CellProcessor[] allProcessors = new CellProcessor[colNum];
            IntStream.of(0, colNum - 1).forEach(i -> {
                allProcessors[i] = new org.supercsv.cellprocessor.Optional();
            });

            while ((listReader.read()) != null) {
                lineNumber++;
                result.add(listReader.executeProcessors(allProcessors));
            }
        } catch (IOException | SuperCsvException e) {
            System.out.println(e);
        }
        return result;
    }

    public HashMap<Long, NatLog> getLogs() {
        return logs;
    }

    public Long getEarliestTimestamp() {
        return earliestTimestamp;
    }

    public Long getLatestTimestamp() {
        return latestTimestamp;
    }

    public Example[] getExamples() {
        return examples;
    }
}
