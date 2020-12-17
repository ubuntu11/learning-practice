package org.projectlombok;

import lombok.Data;

@Data
public class DataUsage {
    private Long id;
    private String title;
    private boolean titleIsSet;

    public static void main(String[] args) {
        DataUsage dataUsage = new DataUsage();
        //with setter
        dataUsage.setId(1L);
        //with getter
        System.out.println(dataUsage.getId());
        //with toString()
        System.out.println(dataUsage.toString());
        //with hashCode()
        System.out.println(dataUsage.hashCode());
        //with equals
        DataUsage dataUsage1 = new DataUsage();
        System.out.println(dataUsage.equals(dataUsage1));
    }
}
