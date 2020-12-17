package tw.idv.ken.kata.primefactors;
import java.util.LinkedList;
import java.util.List;
import java.util.Scanner;

/**
 * 
 */

/**
 * @author ken
 * 質因數分解
 */
public class Kata1 {
	/**
	 * @param args
	 */
	public static void main(String[] args) {
		Scanner scanner = new Scanner(System.in);
	    System.out.print("Enter an integer: ");
		int testPrime = scanner.nextInt();
		scanner.close();
		
		//do it by myself
		Kata1 pTest = new Kata1();
		List<Integer> factors = new LinkedList<>();
		pTest.doBreakdown(factors, testPrime);
		for (Integer a : factors) {
			System.out.println(a);
		}
	}
	
	public List<Integer> breakdown(int testNumber) {
		List<Integer> factors = new LinkedList<>();
		doBreakdown(factors, testNumber);
		return factors; 
	}
	
	private void doBreakdown(List<Integer> factors, int testNumber) {		
		int nextPrimeFactor = getPrimeFactor(testNumber);
		factors.add(nextPrimeFactor);
		if ((testNumber / nextPrimeFactor) >= nextPrimeFactor) {
			doBreakdown(factors, testNumber / nextPrimeFactor);
		} else {
			return;
		}
	}
	
	private int getPrimeFactor(int testNumber) {
		int denominator = 2;
		while (testNumber % denominator > 0) {
			denominator ++;
		}

		return denominator;
	}

}
