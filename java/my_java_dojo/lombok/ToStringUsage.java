package org.projectlombok;

import lombok.ToString;

@ToString
public class ToStringUsage {
    //static field will be excluded
    private static final int STATIC_VAR = 10;
    //exclude field manually
    @ToString.Exclude private Long id;
    private String name = "Katana Wasaki";
    private int age = 25;

    public static void main(String[] args) {
        ToStringUsage usage = new ToStringUsage();
        System.out.println(usage.toString());
    }
}

