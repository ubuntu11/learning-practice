import java.util.Random;
import java.util.HashSet;

public class Hiker {
    private static Random r = new Random();

    public static int[] array(int lower, int upper) {
        int[] iArray = new int[upper - lower];
        for (int i=0; i<iArray.length; i++) {
            iArray[i] = i + lower;
        }
        
        return iArray;
    }
    
    public static int[] shuffleArray(int[] oArray) {
        int n = -1;
        int tmp = -1;
        int[] nArray = new int[oArray.length];
        System.arraycopy(oArray,0,nArray,0,oArray.length);
        HashSet<Integer> s = new HashSet<>();
        for (int i=0; i< nArray.length; i++) {
          n = getNextInt(s, nArray.length);
          tmp = nArray[i];
          nArray[i] = nArray[n];
          nArray[n] = tmp;
        }
        
        return nArray;
    }
    
    private static int getNextInt(HashSet<Integer> s, int upperLimit) {
        int n = r.nextInt(upperLimit);
        while(s.contains(new Integer(n))){
            n = r.nextInt(upperLimit);
        }
        
        s.add(new Integer(n));
        return n;
    }
}

