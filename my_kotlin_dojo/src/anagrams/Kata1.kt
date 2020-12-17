package anagrams

import org.jetbrains.annotations.Mutable

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
class Kata1 {
    /**
     * 主程式.
     * @param input 原始輸入字串，例如：biro
     */
    fun anagrams(input : String) : Array<String> {
        val words : MutableList<String> = mutableListOf()
        for (i in 0..input.length - 1) {
            computeLetter(input, words)
        }
        return words.toTypedArray()
    }

    /**
     * 對某個位置填入所有可能的字元.
     * @param input 原始輸入字串
     * @param words 承裝填入字元的所有字串
     */
    private fun computeLetter(input: String, words: MutableList<String>) {
        val tmpList : MutableList<String> = mutableListOf()
        if (words.isNotEmpty()) {
            for(w in words) {
                for (c in input) {
                    if(!w.contains(c)) {
                        tmpList.add("$w$c")
                    }
                }
            }
        } else {
            for (c in input) {
                tmpList.add(c.toString())
            }
        }

        words.clear()
        words.addAll(0, tmpList)
    }
}