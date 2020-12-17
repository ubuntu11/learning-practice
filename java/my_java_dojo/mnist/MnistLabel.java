package tw.com.familyplus.mnist;

import java.io.DataInputStream;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;

/**
 * A small java program to read labels form mnist training set.
 * @author Ken Hu
 * created on 2018-05-25
 */
public class MnistLabel {
    public static void main(String[] args) {
        int i = 0;
        byte[] buffer = new byte[4];
        try (FileInputStream fis = new FileInputStream("/Users/huzhongwei/IdeaProjects/MyJavaDojo/target/classes/train-labels-idx1-ubyte");
             DataInputStream dis = new DataInputStream(fis);
             FileOutputStream fos = new FileOutputStream("/Users/huzhongwei/IdeaProjects/MyJavaDojo/me")) {
            int magicNumber = dis.readInt();
            System.out.println(magicNumber);
            int numberOfItems = dis.readInt();
            System.out.println(numberOfItems);
            while(i < numberOfItems) {
                fos.write((dis.readUnsignedByte()+System.lineSeparator()).getBytes());
                i++;
            }
            dis.close();
            fis.close();
            fos.close();
        } catch(IOException e) {
            e.printStackTrace();
        }

    }

    private static int bytes2Int(byte[] buffer) {
        return (buffer[0]<<24)&0xff000000|
        (buffer[1]<<16)&0x00ff0000|
                (buffer[2]<< 8)&0x0000ff00|
                (buffer[3]<< 0)&0x000000ff;
    }
}
