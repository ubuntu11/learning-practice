import java.util.Scanner;
import java.util.Hashtable;
import java.util.Enumeration;

/*
程式目標見C語言版本註解.
*/
class ch_2_5 {
  private Hashtable<String, Boolean> rooms = new Hashtable<>();

  private void init() {
    for (int i=0; i<3; i++) {
      for (int j=0; j<3; j++) {
        rooms.put("" + ((i + 1) * 100 + (j % 3 + 1)), false);
      }
    }
  }

  public static void main(String[] args) {
    Scanner scanner = new Scanner(System.in);
    ch_2_5 t = new ch_2_5();
    t.init();

    int roomNumber = -1;
    System.out.println("input paid room number: (input 0 when finish)");
    while((roomNumber = scanner.nextInt()) != 0) {
      if (t.rooms.get(roomNumber) != null) {
        t.rooms.put("" + roomNumber, true);
      }
    }

    Enumeration e = t.rooms.keys();
    while(e.hasMoreElements()) {
      System.out.println(e.nextElement());
    }
  }
}
