package tw.idv.ken.kata.anagrams;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import java.util.Arrays;

import static org.junit.Assert.*;

public class Kata1Test {
    private Kata1 instance = new Kata1();

    @Before
    public void setUp() {
    }

    @After
    public void tearDown() {
    }

    @Test
    public void test_2_letters() {
        String[] result = instance.anagrams("bi");
        assertEquals(2, result.length);
        assertArrayEquals(new String[]{"bi", "ib"}, result);
    }

    @Test
    public void test_3_letters() {
        String[] result = instance.anagrams("bir");
        assertEquals(6, result.length);
        assertArrayEquals(new String[]{"bir", "bri", "ibr", "irb", "rbi", "rib"}, result);
    }

    @Test
    public void test_4_letters() {
        String[] result = instance.anagrams("biro");
        assertEquals(24, result.length);
        assertArrayEquals(new String[]{"biro", "bior", "brio", "broi", "boir", "bori"}, Arrays.copyOfRange(result,0 ,6));
    }
}