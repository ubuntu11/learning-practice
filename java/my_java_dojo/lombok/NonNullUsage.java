package org.projectlombok;

import lombok.NonNull;

public class NonNullUsage {
    private String title;

    public NonNullUsage(@NonNull String title) {
        this.title = title;
    }

    private void setTitle(@NonNull String title) {
        this.title = title;
    }

    public static void main(String[] args) {
        //expect NPE
        try {
            NonNullUsage usage = new NonNullUsage(null);
        } catch(NullPointerException npe) {
            System.out.println("a NPE is caught");
        }

        NonNullUsage usage = new NonNullUsage("NonNull Check");
        //expect NPE
        try {
            usage.setTitle(null);
        } catch(NullPointerException npe) {
            System.out.println("a NPE is caught");
        }
    }
}
