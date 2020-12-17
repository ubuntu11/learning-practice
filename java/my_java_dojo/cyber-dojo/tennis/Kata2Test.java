/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package tw.idv.ken.kata.tennis;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import static org.junit.Assert.*;

/**
 * Tennis score testing. Try to use 
 * @author ken
 */
public class Kata2Test {
    Kata2 score = null;
    
    public Kata2Test() {
    }
    
    @BeforeClass
    public static void setUpClass() {
    }
    
    @AfterClass
    public static void tearDownClass() {
    }
    
    @Before
    public void setUp() {
        score = new Kata2();
    }
    
    @After
    public void tearDown() {
    }

    @Test
    public void testServerGot0Point() {
        assertEquals("Love All", score.announce(0, 0));
        assertEquals("Love Fifteen", score.announce(0, 1));
        assertEquals("Love Thirty", score.announce(0, 2));
        assertEquals("Love Forty", score.announce(0, 3));
        assertEquals("Game Over", score.announce(0, 4));
    }
    
    @Test
    public void testServerGot1Point() {
        assertEquals("Fifteen Love", score.announce(1, 0));
        assertEquals("Fifteen All", score.announce(1, 1));
        assertEquals("Fifteen Thirty", score.announce(1, 2));
        assertEquals("Fifteen Forty", score.announce(1, 3));
        assertEquals("Game Over", score.announce(1, 4));
    }
    
    @Test
    public void testServerGot2Point() {
        assertEquals("Thirty Love", score.announce(2, 0));
        assertEquals("Thirty Fifteen", score.announce(2, 1));
        assertEquals("Thirty All", score.announce(2, 2));
        assertEquals("Thirty Forty", score.announce(2, 3));
        assertEquals("Game Over", score.announce(2, 4));
    }
    
    @Test
    public void testServerGot3Point() {
        assertEquals("Forty Love", score.announce(3, 0));
        assertEquals("Forty Fifteen", score.announce(3, 1));
        assertEquals("Forty Thirty", score.announce(3, 2));
        assertEquals("Deuce", score.announce(3, 3));
        assertEquals("Advantage Out", score.announce(3, 4));
        assertEquals("Game Over", score.announce(3, 5));
    }
    
    @Test
    public void testServerGot4Point() {
        assertEquals("Game Over", score.announce(4, 0));
        assertEquals("Game Over", score.announce(4, 1));
        assertEquals("Game Over", score.announce(4, 2));
        assertEquals("Advantage In", score.announce(4, 3));
        assertEquals("Deuce", score.announce(4, 4));
        assertEquals("Advantage Out", score.announce(4, 5));
        assertEquals("Game Over", score.announce(4, 6));
    }
    
    @Test
    public void testServerGot5Point() {
        assertEquals("Advantage In", score.announce(5, 4));
        assertEquals("Deuce", score.announce(5, 5));
        assertEquals("Advantage Out", score.announce(5, 6));
        assertEquals("Game Over", score.announce(5, 7));
    }
}
