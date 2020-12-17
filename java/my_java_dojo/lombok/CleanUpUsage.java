package org.projectlombok;

import lombok.Cleanup;
import java.util.Arrays;
import java.util.HashSet;
import java.util.Set;

public class CleanUpUsage {
    private Set<String> names = new HashSet<>(Arrays.asList("ken","jonathan"));

    public void cleanUp() {
        System.out.println("I am cleaned up");
    }

    public String pop() {
        if (names.isEmpty()) {
            return null;
        } else {
            String v = names.stream().findFirst().get();
            names.remove(v);
            return v;
        }
    }

    public static void main(String[] args) {
        @Cleanup("cleanUp") CleanUpUsage c = new CleanUpUsage();
        String v = null;
        while((v = c.pop()) != null) {
            System.out.println(v);
        }
    }
}
