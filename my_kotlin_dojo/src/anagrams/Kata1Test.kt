package anagrams

import org.junit.After
import org.junit.Before

import org.junit.Assert.*
import org.junit.Test

class Kata1Test {
    var instance : Kata1 = Kata1()
    @Before
    fun setUp() {
    }

    @After
    fun tearDown() {
    }

    @Test
    fun test_2_letters() {
        val result = instance.anagrams("ab")
        assertEquals(2, result.size)
        assertArrayEquals(arrayOf("ab", "ba"), result)
    }

    @Test
    fun test_3_letters() {
        val result = instance.anagrams("abc")
        assertEquals(6, result.size)
        assertArrayEquals(arrayOf("abc", "acb", "bac", "bca", "cab", "cba"), result)
    }

    @Test
    fun test_4_letters() {
        val result = instance.anagrams("abcd")
        assertEquals(24, result.size)
        assertArrayEquals(arrayOf("abcd", "abdc", "acbd", "acdb", "adbc", "adcb"), result.sliceArray(0..5))
    }

    @Test
    fun test_5_letters() {
        val result = instance.anagrams("abcde")
        assertEquals(120, result.size)
    }
}