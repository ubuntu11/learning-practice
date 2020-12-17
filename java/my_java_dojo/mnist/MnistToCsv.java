package tw.com.familyplus.mnist;

import java.io.*;
import java.net.URL;
import java.nio.channels.Channels;
import java.nio.channels.ReadableByteChannel;
import java.util.Arrays;
import java.util.stream.Collectors;
import java.util.zip.GZIPInputStream;

public class MnistToCsv {
    /**
     * Label File要跳過開項幾個bytes.
     */
    private int bytesToSkipOfLabelFile = 8;
    /**
     * Image File要跳過開項幾個bytes.
     */
    private int bytesToSkipOfImageFile = 16;
    /**
     * Image水平方向像素大小.
     */
    private int imageHorizontalPixels = 28;
    /**
     * Image垂直方向像素大小.
     */
    private int imageVerticalPixels = imageHorizontalPixels;
    /**
     * 讀取Image data時用的緩沖byte陣列.
     */
    private byte[] imageBuffer = new byte[imageHorizontalPixels * imageVerticalPixels];

    /**
     * 區分訓練集與測試集的enum class.
     */
    private enum DataSet {
        TRAIN("http://yann.lecun.com/exdb/mnist/train-labels-idx1-ubyte.gz",
                "http://yann.lecun.com/exdb/mnist/train-images-idx3-ubyte.gz", "mnist-training-set.csv", 60000),
        TEST("http://yann.lecun.com/exdb/mnist/t10k-labels-idx1-ubyte.gz",
                "http://yann.lecun.com/exdb/mnist/t10k-images-idx3-ubyte.gz", "mnist-test-set.csv", 10000);

        private String labelFileUrl, labelFilename, imageFileUrl, imageFilename, csvFilename;
        private int totalImages;

        private DataSet(String labelFileUrl, String imageFileUrl, String csvFilename, int totalImages) {
            this.labelFileUrl = labelFileUrl;
            this.imageFileUrl = imageFileUrl;
            this.labelFilename = labelFileUrl.substring(
                    labelFileUrl.lastIndexOf("/") + 1).replace(".gz", "");;
            this.imageFilename = imageFileUrl.substring(
                    imageFileUrl.lastIndexOf("/") + 1).replace(".gz", "");
            this.csvFilename = csvFilename;
            this.totalImages = totalImages;
        }

        public String getLabelFileUrl(){
            return this.labelFileUrl;
        }

        public String getImageFileUrl() {
            return this.imageFileUrl;
        }

        public String getImageFilename() {
            return this.imageFilename;
        }

        public String getLabelFilename() {
            return this.labelFilename;
        }

        public String getCsvFilename() {
            return this.csvFilename;
        }

        public int getTotalImages() {
            return this.totalImages;
        }
    }

    /**
     * 從網路上取得某個資料集的相關檔案.
     * @param dataSet 訓練集或測試集
     * @throws IOException if network or file I/O runs into problem
     */
    private void getDataSetFile(DataSet dataSet) throws IOException {
        if (!checkFileExistence(dataSet.getLabelFilename())) {
            downloadAndExtractFile(dataSet.getLabelFileUrl(), dataSet.getLabelFilename(), 1);
        }

        if (!checkFileExistence(dataSet.getImageFilename())) {
            downloadAndExtractFile(dataSet.getImageFileUrl(), dataSet.getImageFilename(), 512);
        }
    }

    /**
     * 檢查某個檔案是否存在.
     * @param filename 檔名
     * @return
     */
    private boolean checkFileExistence(String filename) {
        File f = new File(filename);
        return f.exists() && f.isFile();
    }

    /**
     * 從網路下載檔案並解壓縮.
     * @param fileUrl 檔案URL
     * @param outputFilename 解壓縮後的新檔名
     * @param bufferSize 讀取檔案時的緩沖byte陣列大小
     * @throws IOException if network or file I/O runs into problem
     */
    private void downloadAndExtractFile(String fileUrl, String outputFilename, int bufferSize) throws IOException {
        URL dlUrl = new URL(fileUrl);
        String outputFile = fileUrl.substring(fileUrl.lastIndexOf("/") + 1);
        byte[] buffer = new byte[bufferSize];

        ReadableByteChannel rbc = Channels.newChannel(dlUrl.openStream());
        FileOutputStream fos = new FileOutputStream(outputFile);
        fos.getChannel().transferFrom(rbc, 0, Long.MAX_VALUE);
        fos.close();

        InputStream in = new GZIPInputStream(new FileInputStream(outputFile));
        fos = new FileOutputStream(outputFilename);
        BufferedOutputStream bos = new BufferedOutputStream(fos);
        int len = 0;
        while ((len = in.read(buffer)) > 0) {
            bos.write(buffer, 0, len);
        }
        bos.close();
        fos.close();
    }

    /**
     * 取得用以讀取Label file的InputStream.
     * @param dataSet 訓練集或測試集
     * @return an FileInputStream instance
     * @throws IOException if file I/O runs into problem
     */
    private FileInputStream getLabelFileInputStream(DataSet dataSet) throws IOException{
        FileInputStream fis =  new FileInputStream(dataSet.getLabelFilename());
        fis.skip(bytesToSkipOfLabelFile);
        return fis;
    }

    /**
     * 取得用以讀取Image file的InputStream.
     * @param dataSet 訓練集或測試集
     * @return an FileInputStream instance
     * @throws IOException if file I/O runs into problem
     */
    private FileInputStream getImageFileInputStream(DataSet dataSet) throws IOException{
        FileInputStream fis =  new FileInputStream(dataSet.getImageFilename());
        fis.skip(bytesToSkipOfImageFile);
        return fis;
    }

    /**
     * 讀取某個DataSet的Label和Image檔案，並把內容轉存為CSV檔.
     * @param dataSet 訓練集或測試集
     * @throws IOException if file I/O runs into problem
     */
    private void readAndWriteToFile(DataSet dataSet) throws IOException {
        FileInputStream labelIS = getLabelFileInputStream(dataSet);
        FileInputStream imageIS = getImageFileInputStream(dataSet);
        FileOutputStream fos = new FileOutputStream(dataSet.getCsvFilename());
        BufferedOutputStream bos = new BufferedOutputStream(fos);
        StringBuffer sb = new StringBuffer();
        sb.append("label");
        sb.append(",");
        for (int i = 0; i < imageHorizontalPixels * imageVerticalPixels; i++) {
            sb.append("pixel");
            sb.append(i);
            sb.append(",");
        }
        sb.append(System.lineSeparator());
        bos.write(sb.toString().getBytes());
        for (int i = 0; i < dataSet.getTotalImages(); i++) {
            imageIS.read(imageBuffer);
            bos.write((labelIS.read() + "," + intArray2String(byteArray2intArray(imageBuffer)) + System.lineSeparator()).getBytes());
        }

        labelIS.close();
        imageIS.close();
        bos.close();
        fos.close();
    }

    /**
     * 把byte陣列轉成int陣列
     * @param byteArray original byte array
     * @return an int array
     */
    private int[] byteArray2intArray(byte[] byteArray) {
        int[] result = new int[byteArray.length];
        for(int i=0; i<result.length; i++) {
            result[i] = byteArray[i] & 0xFF;
        }
        return result;
    }

    /**
     * 把一個int陣列轉成一個字串,便於列印或存檔.
     * @param intArray original int array
     * @return a String
     */
    private String intArray2String(int[] intArray) {
        return Arrays.stream(intArray).mapToObj(String::valueOf).collect(Collectors.joining(",")).toString();
    }

    public static void main(String[] args) {
        MnistToCsv mnist = new MnistToCsv();

        try {
            //取得訓練集資料
            mnist.getDataSetFile(DataSet.TRAIN);
            mnist.readAndWriteToFile(DataSet.TRAIN);
            //取得測試集資料
            mnist.getDataSetFile(DataSet.TEST);
            mnist.readAndWriteToFile(DataSet.TEST);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
