package org.projectlombok;

import lombok.val;
import lombok.var;
import java.util.ArrayList;

public class VarUsage {
    public static void main(String[] args) {
        var names = new ArrayList<String>();
        names.add("Ken Hu");
        val myName = names.get(0);
        System.out.println("My name is " + myName);

        val names2 = new ArrayList<String>();
        //names can be reassigned, because it is not finalized
        names = names2;
    }
}
