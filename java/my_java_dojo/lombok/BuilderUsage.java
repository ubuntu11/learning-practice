package org.projectlombok;

import lombok.Builder;
import lombok.Singular;

import java.util.List;

@Builder
public class BuilderUsage {
    private Long id;
    private String title;
    private boolean titleIsSet;
    @Singular private List<String> neighbors;

    public static void main(String[] args) {
        BuilderUsage.builder().id(0L).title("Title").titleIsSet(true).neighbor("Jonathan")
                .neighbor("Chiayi").build();
    }
}
