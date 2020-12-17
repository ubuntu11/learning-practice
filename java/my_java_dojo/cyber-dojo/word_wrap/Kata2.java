package tw.idv.ken.kata.wordwrapper;

public class Kata2 {
    public String wrap(String content, int lineWidth) {
        StringBuilder sb = new StringBuilder();
        int lineBreaks = 1;
        for(int i=0;i<content.length(); i++){
            sb.append(content.charAt(i));
            if(i == (lineWidth * lineBreaks - 1)){
                sb.append("\n");
                lineBreaks ++;
            }
        }
        return sb.toString();
    }
}
