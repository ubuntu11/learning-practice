package tw.idv.ken.kata.anagrams;

import java.util.ArrayList;
import java.util.List;

/**
 * Anagrams.
 * Write a program to generate all potential
 * anagrams of an input string.
 *
 * For example, the potential anagrams of "biro" are
 *
 * biro bior brio broi boir bori
 * ibro ibor irbo irob iobr iorb
 * rbio rboi ribo riob roib robi
 * obir obri oibr oirb orbi orib
 */
public class Kata1 {
    /**
     * 主程式.
     * @param input 原始輸入字串
     * @return 產生的結果，一個字串陣列
     */
    public String[] anagrams(String input) {
        List<String> words = new ArrayList<>();
        for(int i=0;i <input.length(); i++) {
            generateWordByLetter(input, words);
        }
        return words.toArray(new String[0]);
    }

    /**
     * 由左而右，逐格填入可行字元，產生不同的字串.
     * @param input 原始輸入字串
     * @param words 產生的字串陣列,運算中會變更它的內容
     */
    private void generateWordByLetter(String input, List<String> words) {
        List<String> tmpWords = new ArrayList<>();
        if(words.isEmpty()) {
            for(int i=0; i<input.length(); i++) {
                tmpWords.add(String.valueOf(input.charAt(i)));
            }
        } else {
            words.forEach(w -> {
                for (int i = 0; i < input.length(); i++) {
                    String letter = String.valueOf(input.charAt(i));
                    if(! w.contains(letter))
                        tmpWords.add(w + letter);
                }
            });
        }
        words.clear();
        words.addAll(tmpWords);
    }
}
