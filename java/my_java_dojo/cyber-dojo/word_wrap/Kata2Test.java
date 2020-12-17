package tw.idv.ken.kata.wordwrapper;

import static org.junit.Assert.*;

import org.junit.Test;

public class Kata2Test {
	private Kata2 wrapper = new Kata2();
    @Test
    public void noBreak() {
        String expected = "hello";
        String actual = wrapper.wrap("hello", 10);

        expected = "hi man";
        actual = wrapper.wrap("hi man", 10);
        assertEquals(expected, actual);
    }


    @Test
    public void oneBreak() {
        String original = "hello world, great";
        String expected = "hello worl\nd, great";
        String actual = wrapper.wrap(original, 10);
        assertEquals(expected, actual);
    }


    @Test
    public void twoBreak() {
        String original = "hello world, great. and you";
        String expected = "hello worl\nd, great. \nand you";
        String actual = wrapper.wrap(original, 10);
        assertEquals(expected, actual);
    }

    @Test
    public void threeBreak() {
        String original = "hello world, great. and you? i am fine";
        String expected = "hello worl\nd, great. \nand you? i\n am fine";
        String actual = wrapper.wrap(original, 10);
        assertEquals(expected, actual);
    }
}
