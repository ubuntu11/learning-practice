package tw.com.familyplus.mnist;

import javax.imageio.ImageIO;
import java.awt.image.BufferedImage;
import java.io.*;

import static java.awt.image.BufferedImage.TYPE_BYTE_GRAY;

/**
 * A small java program to read images form mnist training set.
 * @author Ken Hu
 * created on 2018-05-27
 */
public class MnistImage {
    public static void main(String[] args) {

        byte[] buffer = new byte[28 * 28];
        try (FileInputStream fis = new FileInputStream("/Users/huzhongwei/Downloads/train-images-idx3-ubyte")) {
            InputStream in = new ByteArrayInputStream(buffer);
            BufferedImage bImageFromConvert = new BufferedImage(28, 28, TYPE_BYTE_GRAY);

            fis.skip(16);
            fis.skip(28 * 28 * 10000);
            //generate 64 images
            for (int count = 0; count < 64; count++) {
                fis.read(buffer);
                for (int i = 0; i < 28; i++) {
                    for (int j = 0; j < 28; j++) {
                        bImageFromConvert.setRGB(i, j, buffer[j * 28 + i]);
                    }
                }

                ImageIO.write(bImageFromConvert, "jpg", new File(
                    String.format("/Users/huzhongwei/Downloads/mnist-traing-image%s.jpg", count)));
            }
        } catch (IOException e) {

        }

    }

    static int bytes2Int(byte[] buffer) {
        return (buffer[0]<<24)&0xff000000|
        (buffer[1]<<16)&0x00ff0000|
                (buffer[2]<< 8)&0x0000ff00|
                (buffer[3]<< 0)&0x000000ff;
    }
}
