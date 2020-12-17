/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package tw.idv.ken.kata.fizzbuzz;

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
    static Kata1 instance = new Kata1();
    
    public Kata1Test() {
    }
    
    @BeforeClass
    public static void setUpClass() {
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

    /**
     * Test of says method, of class Kata1.
     */
    @Test
    public void testFizzBuzz() {
        String actual = instance.says(1);
        assertEquals("1", actual);
        actual = instance.says(2);
        assertEquals("2", actual);
        actual = instance.says(3);
        assertEquals("Fizz", actual);
        actual = instance.says(5);
        assertEquals("Buzz", actual);
        actual = instance.says(15);
        assertEquals("FizzBuzz", actual);
        actual = instance.says(25);
        assertEquals("Buzz", actual);
        actual = instance.says(37);
        assertEquals("37", actual);
        actual = instance.says(75);
        assertEquals("FizzBuzz", actual);
    }
    
}
