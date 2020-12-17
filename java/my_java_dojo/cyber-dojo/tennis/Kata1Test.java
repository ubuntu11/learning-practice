package tw.idv.ken.kata.tennis;

import static org.junit.Assert.*;

import org.junit.Test;

public class Kata1Test {
	Kata1 tennis = new Kata1();
	/*
	 * reference: http://tennisteens.info/umpire/calpoint.htm
	 * 
	 * Decision Tree
	 * Scores
	 * |_equal
	 *     |_0:0 Love All
	 *     |_1:1 Fifteen All
	 *     |_2:2 Thirty All
	 *     |_3:3 Deuce
	 *     |_4:4 Decue
	 *     |_... Deuce
	 * |_not equal
	 *     |_A leads
	 *        |_1:0 Fifteen Love
	 *        |_2:0 Thirty Love
	 *        |_3:0 Forty Love
	 *        |_4:0 A Win
	 *        |_2:1 Thirty Fifteen
	 *        |_3:1 Forty Love
	 *        |_4:1 A Win
	 *        |_3:2 Forty Thirty
	 *        |_4:2 A Win
	 *        |_4:3 A Advantage
	 *        |_5:3 A Win
	 *        |_5:4 A Advantage
	 *     |_B leads
	 *        |_0:1 Love Fifteen
	 *        |_0:2 Love Thirty
	 *        |_0:3 Love Forty
	 *        |_0:4 B Win
	 *        |_1:2 Fifteen Thirty
	 *        |_1:3 Fifteen Forty
	 *        |_1:4 B Win
	 */

	@Test
	public void equals() {
		assertEquals("Love All", tennis.scores(0, 0));
		assertEquals("Fifteen All", tennis.scores(1, 1));
		assertEquals("Thirty All", tennis.scores(2, 2));
		assertEquals("Deuce", tennis.scores(3, 3));
		assertEquals("Deuce", tennis.scores(4, 4));
		assertEquals("Deuce", tennis.scores(5, 5));
	}

	@Test
	public void aLeads() {
		assertEquals("Fifteen Love", tennis.scores(1, 0));
		assertEquals("Thirty Love", tennis.scores(2, 0));
		assertEquals("Forty Love", tennis.scores(3, 0));
		assertEquals("A Win", tennis.scores(4, 0));
		assertEquals("Thirty Fifteen", tennis.scores(2, 1));
		assertEquals("Forty Fifteen", tennis.scores(3, 1));		
		assertEquals("A Win", tennis.scores(4, 1));
		assertEquals("Forty Thirty", tennis.scores(3, 2));
		assertEquals("A Win", tennis.scores(4, 2));
		assertEquals("A Advantage", tennis.scores(4, 3));
		assertEquals("A Win", tennis.scores(5, 3));
		assertEquals("A Advantage", tennis.scores(5, 4));
	}

	@Test
	public void bLeads() {
		assertEquals("Love Fifteen", tennis.scores(0, 1));
		assertEquals("Love Thirty", tennis.scores(0, 2));
		assertEquals("Love Forty", tennis.scores(0, 3));
		assertEquals("B Win", tennis.scores(0, 4));
		assertEquals("Fifteen Thirty", tennis.scores(1, 2));
		assertEquals("Fifteen Forty", tennis.scores(1, 3));
		assertEquals("B Win", tennis.scores(1, 4));
	}
}
