package com.hwacom.fplus.logAnalyzer.data;
//sid,macAddress,txPkts,txBytes,rxPkts,rxBytes,lastActiveTime
public class NatLogUpdate extends NatLog {
    private int txPkts;
    private int txBytes;
    private int rxPkts;
    private int rxBytes;
    private Long lastActiveTime;

    public int getTxPkts() {
        return txPkts;
    }

    public void setTxPkts(int txPkts) {
        this.txPkts = txPkts;
    }

    public int getTxBytes() {
        return txBytes;
    }

    public void setTxBytes(int txBytes) {
        this.txBytes = txBytes;
    }

    public int getRxPkts() {
        return rxPkts;
    }

    public void setRxPkts(int rxPkts) {
        this.rxPkts = rxPkts;
    }

    public int getRxBytes() {
        return rxBytes;
    }

    public void setRxBytes(int rxBytes) {
        this.rxBytes = rxBytes;
    }

    public Long getLastActiveTime() {
        return lastActiveTime;
    }

    public void setLastActiveTime(Long lastActiveTime) {
        this.lastActiveTime = lastActiveTime;
    }
}
