/**
 * 
 */
package tw.idv.ken.kata.wordwrapper;

import static org.junit.Assert.*;

import org.junit.Test;

/**
 * @author ken
 *
 */
public class Kata1Test {

	Kata1 processor = new Kata1();

    @Test
    public void oneLine() {
        String original = "this is a book";
        String expected = original;
        String actual = processor.addNewLine(original, 30);
        assertEquals(expected, actual);
    }

    @Test
    public void twoLines() {
        String original = "this is a book";
        String expected = "this is a " + "\n" +
            "book";
        String actual = processor.addNewLine(original, 10);
        assertEquals(expected, actual);
    }

    @Test
    public void threeLines() {
        String original = "this is a book,a wonderful book";
        String expected = "this is a " + "\n" +
            "book,a won" + "\n" +
            "derful boo" + "\n" +
            "k";
        String actual = processor.addNewLine(original, 10);
        assertEquals(expected, actual);
    }

    @Test
    public void multipleOfWidth() {
        String original = "0123456789";
        String expected = "0123456789\n";
        String actual = processor.addNewLine(original, 10);
        assertEquals(expected, actual);
        original = "01234567890123456789";
        expected = "0123456789\n0123456789\n";
        actual = processor.addNewLine(original, 10);
        assertEquals(expected, actual);
    }

}
