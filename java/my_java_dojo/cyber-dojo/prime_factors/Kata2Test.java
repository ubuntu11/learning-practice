package tw.idv.ken.kata.primefactors;
import static org.junit.Assert.assertEquals;

import java.util.Arrays;

import org.junit.Test;


public class Kata2Test {
	Kata2 testInst = new Kata2();

	@Test
	public void test2() {
		assertEquals(Arrays.asList(2), testInst.breakdownRefactoring(2));
	}

	@Test
	public void test3() {
		assertEquals(Arrays.asList(3), testInst.breakdownRefactoring(3));
	}

	@Test
	public void test4() {
		assertEquals(Arrays.asList(2, 2), testInst.breakdownRefactoring(4));
	}

	@Test
	public void test5() {
		assertEquals(Arrays.asList(5), testInst.breakdownRefactoring(5));
	}

	@Test
	public void test6() {
		assertEquals(Arrays.asList(2,3), testInst.breakdownRefactoring(6));
	}

	@Test
	public void test7() {
		assertEquals(Arrays.asList(7), testInst.breakdownRefactoring(7));
	}

	@Test
	public void test8() {
		assertEquals(Arrays.asList(2, 2, 2), testInst.breakdownRefactoring(8));
	}

	@Test
	public void test9() {
		assertEquals(Arrays.asList(3, 3), testInst.breakdownRefactoring(9));
	}

	@Test
	public void test10() {
		assertEquals(Arrays.asList(2, 5), testInst.breakdownRefactoring(10));
	}
	
	@Test
	public void test24() {
		assertEquals(Arrays.asList(2, 2, 2, 3), testInst.breakdownRefactoring(24));
	}		
	
	@Test
	public void test120() {
		assertEquals(Arrays.asList(2, 2, 2, 3, 5), testInst.breakdownRefactoring(120));
	}
	
}
