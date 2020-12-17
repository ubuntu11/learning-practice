// A simple example to get you started
// JUnit assertion - the default Java assertion library
// https://junit.org/junit5/

import org.junit.jupiter.api.Test;
import static org.junit.jupiter.api.Assertions.*;

public class HikerTest {

    @Test
    void generate_integer_array() {
        int[] iArray = Hiker.array(1,7);
        assertEquals(6, iArray.length);
        assertEquals(1, iArray[0]);
        assertArrayEquals(new int[]{1,2,3,4,5,6}, iArray);
        
        iArray = Hiker.array(3,10);
        assertEquals(7, iArray.length);
        assertEquals(3, iArray[0]);
        assertArrayEquals(new int[]{3,4,5,6,7,8,9}, iArray);
    }
    
    @Test
    void shuffle_array() {
        int[] oArray = Hiker.array(1,7);
        int[] nArray = Hiker.shuffleArray(oArray);
        assertEquals(6, nArray.length);        
    }
}

