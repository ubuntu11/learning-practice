package org.projectlombok;

import lombok.AccessLevel;
import lombok.Getter;
import lombok.Setter;

public class GetterSetterOnField {
    @Getter
    @Setter
    private String title;
    //can specify access level
    @Getter(AccessLevel.PROTECTED)
    private boolean titleIsSet = false;

    public static void main(String[] args) {
        GetterSetterOnField usage = new GetterSetterOnField();
        usage.setTitle("Getter/Setter generation");
        System.out.println(usage.getTitle());

        System.out.println(usage.isTitleIsSet());
        //no setter for changing titleIsSet
        //usage.setTitleIsSet(true);
    }
}
