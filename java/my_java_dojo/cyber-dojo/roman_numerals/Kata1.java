/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package tw.idv.ken.kata.romannumerals;

import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * 羅馬數字的第一次練習.參考:https://zh.wikipedia.org/wiki/%E7%BD%97%E9%A9%AC%E6%95%B0%E5%AD%97
 * @author ken
 */
public class Kata1 {
    private int[] keyNumbers = {1000, 500, 100, 50, 10, 5, 1};
    private int[] keyNumberCounts = {0, 0, 0, 0, 0, 0, 0};
    private String[] keyNumberSymbols = {"M", "D", "C", "L", "X", "V", "I"};
	private String[] errPatterns = new String[] { "IIII", "VIV", "XXXX", "LXL", "CCCC", "DCD" };
	private String[] replacePatterns = new String[] { "IV", "IX", "XL", "XC", "CD", "CM" };
    /**
     * Convert an hindu-arabic number to roman number.
     * @param i an integer as hindu-arabic number.
     * @return converted roman number
     */
    String convert(int i) {
        setKeyNumberCounts(i);
        String romanNumber = this.toRomanNumber(i);
        return romanNumber;
    }
    
    private void setKeyNumberCounts(int i) {
        int seed = i;
        for(int c=0; c<this.keyNumberCounts.length; c++) {
            this.keyNumberCounts[c] = seed / this.keyNumbers[c];
            seed = seed - this.keyNumberCounts[c] * this.keyNumbers[c];
        }
    }
    
    private String toRomanNumber(int i) {
    	String romanNumber;
        StringBuilder sb = new StringBuilder();
        for(int c=0; c<this.keyNumberCounts.length; c++) {
            for (int d=0; d<this.keyNumberCounts[c]; d++) {
                sb.append(this.keyNumberSymbols[c]);
            }
        }
        romanNumber = sb.toString();
        return handleOccurranceFour(romanNumber);
    }

    private String handleOccurranceFour(String romanNumber) {
        int i = 0;
        for (String errPattern : errPatterns) {
            Pattern pattern = Pattern.compile(errPattern);
            Matcher matcher = pattern.matcher(romanNumber);
            romanNumber = matcher.replaceAll(this.replacePatterns[i]);
            i++;
        }
        return romanNumber;
    }
}
