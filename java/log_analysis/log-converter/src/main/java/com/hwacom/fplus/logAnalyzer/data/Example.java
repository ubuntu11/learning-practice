package com.hwacom.fplus.logAnalyzer.data;

import java.time.LocalDateTime;

public class Example {
    private LocalDateTime time;
    private int connections = 0;
    private double txPkts = 0;
    private double txBytes = 0;
    private double rxPkts = 0;
    private double rxBytes = 0;

    public Example(LocalDateTime time) {
        this.time = time;
    }
    public void addConnection() {
        this.connections ++;
    }

    public void addTxPkts(double pkts) {
        this.txPkts = this.txPkts + pkts;
    }

    public void addTxBytes(double bytes) {
        this.txBytes = this.txBytes + bytes;
    }

    public void addRxPkts(double pkts) {
        this.rxPkts = this.rxPkts + pkts;
    }

    public void addRxBytes(double bytes) {
        this.rxBytes = this.rxBytes + bytes;
    }

    public LocalDateTime getTime() {
        return time;
    }

    public int getConnections() {
        return connections;
    }

    public int getTxPkts() {
        return Double.valueOf(txPkts).intValue();
    }

    public int getTxBytes() {
        return Double.valueOf(txBytes).intValue();
    }

    public int getRxPkts() {
        return Double.valueOf(rxPkts).intValue();
    }

    public int getRxBytes() {
        return Double.valueOf(rxBytes).intValue();
    }
}
