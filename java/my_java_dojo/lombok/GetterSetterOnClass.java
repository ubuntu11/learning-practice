package org.projectlombok;

import lombok.AccessLevel;
import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class GetterSetterOnClass {
    private String title;

    //disable class level setting
    @Setter(AccessLevel.NONE)
    private boolean titleIsSet = false;

    public static void main(String[] args) {
        GetterSetterOnClass getterSetterOnClass = new GetterSetterOnClass();
        getterSetterOnClass.setTitle("Title");
        System.out.println(getterSetterOnClass.getTitle());

        //setter is disabled
        //getterSetterOnClass.setTitleIsSet(true);
        System.out.println(getterSetterOnClass.isTitleIsSet());
    }
}
