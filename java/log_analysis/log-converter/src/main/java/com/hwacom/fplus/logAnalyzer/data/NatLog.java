package com.hwacom.fplus.logAnalyzer.data;
import java.util.ArrayList;
import java.util.List;

public class NatLog {
    public static final String LOG_TYPE_NEW = "N";
    public static final String LOG_TYPE_UPDATE = "U";

    private String logType;
    private Long sid;
    private String macAddress;

    public Long getSid() {
        return sid;
    }

    public void setSid(Long sid) {
        this.sid = sid;
    }

    public String getMacAddress() {
        return macAddress;
    }

    public void setMacAddress(String macAddress) {
        this.macAddress = macAddress;
    }

    public String getLogType() {
        return logType;
    }

    public void setLogType(String logType) {
        this.logType = logType;
    }

    public boolean isNewLog() {
        return LOG_TYPE_NEW.equalsIgnoreCase(this.logType);
    }
}
