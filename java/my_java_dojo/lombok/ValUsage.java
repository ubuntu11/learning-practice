package org.projectlombok;

import lombok.val;
import java.util.ArrayList;

public class ValUsage {
    String name = "Ken HU";
    //val is not for fields
    //val address = "KH City";

    private void sillyLoop() {
        String[] names = {"ken", "jonathan"};
        //val works on foreach loop
        for (val name : names) {
            System.out.println(name);
        }
    }

    public static void main(String[] args) {
        //val works on local variable
        val names = new ArrayList<String>();
        names.add("Ken Hu");
        val myName = names.get(0);
        System.out.println("My name is " + myName);

        val names2 = new ArrayList<String>();
        //val can't be reassigned
        //names = names2;
    }
}
