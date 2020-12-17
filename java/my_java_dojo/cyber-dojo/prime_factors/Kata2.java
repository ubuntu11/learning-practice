package tw.idv.ken.kata.primefactors;
import java.util.Arrays;
import java.util.LinkedList;
import java.util.List;


public class Kata2 {
    /**
     * 第一個版本
     * @param i
     * @return
     */
	public List<Integer> breakdown(int i) {
		if (i == 3) return Arrays.asList(3);
		if (i == 4) return Arrays.asList(2, 2);
		if (i == 5) return Arrays.asList(5);
		if (i == 6) return Arrays.asList(2, 3);
		if (i == 7) return Arrays.asList(7);
		//若要讓9過，加下一行
		if (i == 9) return Arrays.asList(3, 3);
		return Arrays.asList(2);
		//寫到這裡，規納出規則：找出第一個質因數，加入List中，接著做判斷，要不要繼續找下一個
		//質因數，then repeat這個過程
		/*
		 * TODO:
		 * 1.先寫一個找質因數的方法: findFirstPrimeFactor-2,3 pass, 
		 *   4 fail（expected:<[2, 2]> but was:<[2]>），進行第二步
		 * 2.寫一個做判斷要不要繼續的方法:hasNext 既然 4/2 = 原本的被除數2，所以要繼續
		 * 3.到了6又fail,expected:<[2, 3]> but was:<[2]>，因為原本的hasNext只寫：
		 * dividend / divisor == divisor，把==改成>=，6&7就pass
		 * 4.到了8又fail: expected:<[2, 2, 2]> but was:<[2, 2]>，嗯，原本那個
		if(hasNext(i, firstPrimeFactor)) {
			int secondPrimeFactor = findFirstPrimeFactor(i / firstPrimeFactor);
			return Arrays.asList(firstPrimeFactor, secondPrimeFactor);
		}顯然只能滿足只有兩個質因數的情況
		 * 5.針對4做修改，寫了doBreakdown(List<Integer> factors, int dividend)這個method
		 *   then all tests pass
		 */
	}
	
	/**
	 * 第二個版本,breakdown的重構.
	 * @param i
	 * @return
	 */
	public List<Integer> breakdownRefactoring(int i) {
		//到上頭TODO 4的版本
        /*
		int firstPrimeFactor = findFirstPrimeFactor(i);
		if(hasNext(i, firstPrimeFactor)) {
			int secondPrimeFactor = findFirstPrimeFactor(i / firstPrimeFactor);
			return Arrays.asList(firstPrimeFactor, secondPrimeFactor);
		}
		return Arrays.asList(firstPrimeFactor);
		*/	
		List<Integer> factors = new LinkedList<>();
		doBreakdown(factors, i);
		return factors;	
	}
	
	private void doBreakdown(List<Integer> factors, int dividend) {
		int nextPrimeFactor = findFirstPrimeFactor(dividend);
		factors.add(nextPrimeFactor);
		if (hasNext(dividend, nextPrimeFactor)) {
			doBreakdown(factors, dividend / nextPrimeFactor);
		}
	}

	private boolean hasNext(int dividend, int divisor) {
		return dividend / divisor >= divisor;
	}
	
    private int findFirstPrimeFactor(int dividend) {
		int denominator = 2;
		while (dividend % denominator > 0) {
			denominator ++;
		}

		return denominator;
    }
}
