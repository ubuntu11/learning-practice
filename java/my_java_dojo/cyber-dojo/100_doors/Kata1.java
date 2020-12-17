package tw.idv.ken.kata.one_hundred_doors;

/**
 * 100 doors in a row are all initially closed. You make
 100 passes by the doors. The first time through, you
 visit every door and toggle the door (if the door is
 closed, you open it; if it is open, you close it).
 The second time you only visit every 2nd door (door
 #2, #4, #6, ...). The third time, every 3rd door
 (door #3, #6, #9, ...), etc, until you only visit
 the 100th door.

 Question: What state are the doors in after the last
 pass? Which are open, which are closed?

 [Source http://rosettacode.org]
 */
public class Kata1 {
    private static final int NUMNER_OF_DOORS = 100;
    private Door[] doors;

    public void visit(int times) {
        for(int i=1; i<=times; i++) {
            for(int j=(i-1);j<NUMNER_OF_DOORS;j=j+i){
                doors[j].toogle();
            }
        }
    }

    public Door[] getDoors() {
        return doors;
    }

    public class Door {
        private boolean isOpenedflag = false;
        public boolean isOpened() {
            return isOpenedflag;
        }

        public void toogle() {
            isOpenedflag = !isOpenedflag;
        }
    }

    public Kata1() {
        init();
    }

    private void init() {
        doors = new Door[NUMNER_OF_DOORS];
        for(int i=0; i<NUMNER_OF_DOORS; i++) {
            doors[i] = new Door();
        }
    }
}
