/**
 * 
 */
package tw.idv.ken.kata.wordwrapper;

/**
 * @author ken
 * 
 */
public class Kata1 {
    public String addNewLine(String content, int width) {
        int newLineCount = 0;
        String result = "";
        while (content.length() > width * newLineCount) {
            if ((content.length() - width * newLineCount) < width) {
                result = result + content.substring( 
                    width * newLineCount);
            } else {
                result = result + content.substring( 
                    width * newLineCount, width * (newLineCount + 1)) + "\n";
            }
            newLineCount ++;
        }

        return result;
    }
}
