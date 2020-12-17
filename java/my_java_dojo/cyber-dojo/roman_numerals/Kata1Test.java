/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package tw.idv.ken.kata.romannumerals;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import static org.junit.Assert.*;

/**
 *
 * @author ken
 */
public class Kata1Test {
    static Kata1 instance;
    
    public Kata1Test() {
    }
    
    @BeforeClass
    public static void setUpClass() {
        instance = new Kata1();
    }
    
    @AfterClass
    public static void tearDownClass() {
    }
    
    @Before
    public void setUp() {
    }
    
    @After
    public void tearDown() {
    }

    @Test
    public void testSingleDigit() {
        assertEquals("I", instance.convert(1));
        assertEquals("II", instance.convert(2));
        assertEquals("III", instance.convert(3));
        assertEquals("IV", instance.convert(4));
        assertEquals("V", instance.convert(5));
        assertEquals("VI", instance.convert(6));
        assertEquals("VII", instance.convert(7));
        assertEquals("VIII", instance.convert(8));
        assertEquals("IX", instance.convert(9));
    }
    
    @Test
    public void testDoubleDigits() {
        assertEquals("X", instance.convert(10));
        assertEquals("XI", instance.convert(11));
        assertEquals("XII", instance.convert(12));
        assertEquals("XIII", instance.convert(13));
        assertEquals("XIV", instance.convert(14));
        assertEquals("XV", instance.convert(15));
        assertEquals("XVI", instance.convert(16));
        assertEquals("XIX", instance.convert(19));
        assertEquals("XX", instance.convert(20));
        assertEquals("XXX", instance.convert(30));
        assertEquals("XL", instance.convert(40));
        assertEquals("L", instance.convert(50));
        assertEquals("LX", instance.convert(60));
        assertEquals("LXV", instance.convert(65));
        assertEquals("LXXXIX", instance.convert(89));
        assertEquals("XC", instance.convert(90));
        assertEquals("XCIII", instance.convert(93));
        assertEquals("XCIV", instance.convert(94));
        assertEquals("XCIX", instance.convert(99));
    }
    
    @Test
    public void testThreeDigits() {
        assertEquals("C", instance.convert(100));
        assertEquals("CI", instance.convert(101));
        assertEquals("CIX", instance.convert(109));
        assertEquals("CI", instance.convert(101));
        assertEquals("CCCXCIX", instance.convert(399));
        assertEquals("CD", instance.convert(400));
        assertEquals("CDLV", instance.convert(455));
        assertEquals("DCCCXCIX", instance.convert(899));
        assertEquals("CM", instance.convert(900));
        assertEquals("CMXCIX", instance.convert(999));
    }
    
    @Test
    public void testFourDigits() {
        assertEquals("M", instance.convert(1000));
        assertEquals("MCCCXCVIII", instance.convert(1398));
        assertEquals("MMMCCCXXXIII", instance.convert(3333));
        assertEquals("MMMCCCLXXV", instance.convert(3375));
        assertEquals("MMMCMXCIX", instance.convert(3999));
    }
}
