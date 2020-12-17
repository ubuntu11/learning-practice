package org.projectlombok;

import lombok.EqualsAndHashCode;

@EqualsAndHashCode
public class EqualsAndHashCodeUsage {
    private static int STATIC_VAR = -1;
    private transient Long id;
    private String name;
    private int age;
    @EqualsAndHashCode.Exclude private String sex;

    public EqualsAndHashCodeUsage(Long id, String name, int age) {
        this.id = id;
        this.name = name;
        this.age = age;
    }

    public static void main(String[] args) {
        EqualsAndHashCodeUsage katana1 = new EqualsAndHashCodeUsage(0L,"Katana", 25);
        EqualsAndHashCodeUsage katana2 = new EqualsAndHashCodeUsage(1L,"Katana", 25);
        EqualsAndHashCodeUsage katana3 = new EqualsAndHashCodeUsage(2L,"Katana", 35);
        //compare by name & age
        System.out.println(String.format("katana1.equals(katana2) ? %s, katana1.equals(katana3) ? %s",
            katana1.equals(katana2), katana1.equals(katana3)));
        //print out hash code
        System.out.println(String.format("hashCode of katana1,katana2,katana3=(%s,%s,%s)",
            katana1.hashCode(), katana2.hashCode(), katana3.hashCode()));
    }
}
