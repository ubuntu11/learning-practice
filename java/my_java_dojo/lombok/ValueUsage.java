package org.projectlombok;

import lombok.Value;

@Value public class ValueUsage {
    private Long id;
    private String title;
    private boolean titleIsSet;

    public static void main(String[] args) {
        //has only all args constructor
        ValueUsage valueUsage = new ValueUsage(0L, "Title", true);
        //with getter but no setter
        System.out.println(valueUsage.getTitle());
    }
}
