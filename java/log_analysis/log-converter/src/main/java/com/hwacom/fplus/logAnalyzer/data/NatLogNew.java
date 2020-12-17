package com.hwacom.fplus.logAnalyzer.data;

import java.sql.Timestamp;
import java.time.LocalDateTime;
import java.time.temporal.ChronoUnit;
import java.util.ArrayList;
import java.util.List;

public class NatLogNew extends NatLog {
    private String privateIp;
    private int privatePort;
    private String peerIP;
    private int peerPort;
    private Long openTimestamp;

    private List<NatLogUpdate> updates = new ArrayList<>();

    public String getPrivateIp() {
        return privateIp;
    }

    public void setPrivateIp(String privateIp) {
        this.privateIp = privateIp;
    }

    public int getPrivatePort() {
        return privatePort;
    }

    public void setPrivatePort(int privatePort) {
        this.privatePort = privatePort;
    }

    public String getPeerIP() {
        return peerIP;
    }

    public void setPeerIP(String peerIP) {
        this.peerIP = peerIP;
    }

    public int getPeerPort() {
        return peerPort;
    }

    public void setPeerPort(int peerPort) {
        this.peerPort = peerPort;
    }

    public Long getOpenTimestamp() {
        return openTimestamp;
    }

    public void setOpenTimestamp(Long openTimestamp) {
        this.openTimestamp = openTimestamp;
    }

    public List<NatLogUpdate> getUpdates() {
        return updates;
    }

    public void addUpdate(NatLogUpdate update) {
        this.updates.add(update);
    }

    public boolean residesInTimeRange(LocalDateTime time) {
        LocalDateTime begin = new Timestamp(this.openTimestamp).toLocalDateTime();
        return (begin.isEqual(time) || begin.isAfter(time)) && (begin.isBefore(time.plusMinutes(1l)));
    }

    public int getTxPkts() {
        return getLastUpdate().getTxPkts();
    }

    public int getTxBytes() {
        return getLastUpdate().getTxBytes();
    }

    public int getRxPkts() {
        return getLastUpdate().getRxPkts();
    }

    public double getDurationInSecond() {
        return (getLastUpdate().getLastActiveTime() - this.openTimestamp) / 1000;
    }

    public int getRxBytes() {
        return getLastUpdate().getRxBytes();
    }

    private NatLogUpdate getLastUpdate() {
        return this.getUpdates().get(updates.size() - 1);
    }
}
