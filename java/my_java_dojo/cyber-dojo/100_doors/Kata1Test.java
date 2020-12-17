package tw.idv.ken.kata.one_hundred_doors;

import org.junit.Before;
import org.junit.Test;

import static org.junit.Assert.*;

public class Kata1Test {
    private Kata1 instance;
    @Before
    public void setUp() {
        instance = new Kata1();
    }

    @Test
    public void test_1st_time_visit() {
        instance.visit(1);
        Kata1.Door[] doors = instance.getDoors();
        assertTrue(doors[0].isOpened());
        assertTrue(doors[1].isOpened());
        assertTrue(doors[2].isOpened());
        assertTrue(doors[3].isOpened());
        assertTrue(doors[4].isOpened());
        assertTrue(doors[50].isOpened());
        assertTrue(doors[99].isOpened());
    }

    @Test
    public void test_2nd_time_visit() {
        instance.visit(2);
        Kata1.Door[] doors = instance.getDoors();
        assertTrue(doors[0].isOpened());
        assertFalse(doors[1].isOpened());
        assertTrue(doors[2].isOpened());
        assertFalse(doors[3].isOpened());
        assertTrue(doors[4].isOpened());
        assertTrue(doors[50].isOpened());
        assertFalse(doors[99].isOpened());
    }

    @Test//every 1,2,3
    public void test_3rd_time_visit() {
        instance.visit(3);
        Kata1.Door[] doors = instance.getDoors();
        assertTrue(doors[0].isOpened());
        assertFalse(doors[1].isOpened());
        assertFalse(doors[2].isOpened());
        assertFalse(doors[3].isOpened());
        assertTrue(doors[4].isOpened());
        assertTrue(doors[5].isOpened());
        assertTrue(doors[29].isOpened());
        assertFalse(doors[39].isOpened());
    }

    @Test//every 1,2,3,4
    public void test_4st_time_visit() {
        instance.visit(4);
        Kata1.Door[] doors = instance.getDoors();
        assertTrue(doors[0].isOpened());
        assertFalse(doors[1].isOpened());
        assertFalse(doors[2].isOpened());
        assertTrue(doors[3].isOpened());
        assertTrue(doors[7].isOpened());
        assertFalse(doors[98].isOpened());
        assertTrue(doors[99].isOpened());
    }

    @Test
    public void test_100st_time_visit() {
        instance.visit(100);
        Kata1.Door[] doors = instance.getDoors();
        assertTrue(doors[3].isOpened());
        assertTrue(doors[8].isOpened());
        assertTrue(doors[15].isOpened());
        assertTrue(doors[24].isOpened());
        assertTrue(doors[35].isOpened());
        assertTrue(doors[48].isOpened());
        assertTrue(doors[63].isOpened());
        assertTrue(doors[80].isOpened());
        assertTrue(doors[99].isOpened());
    }
}