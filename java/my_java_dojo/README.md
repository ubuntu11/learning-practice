# MyJavaDojo

這個repository原本是用來參照[cyber-dojo](http://cyber-dojo.org)上的題目，
來自我練習Java coding的熟練度，後來逐漸擴充它的用途。

用以下的目錄來區分不同的用途

* cyber-dojo：該網站的多個coding練習題目，用java、python做練習
- run test, for example:
  - Java
    ```
    javac -cp ./:../../java_libs/junit-jupiter-api-5.5.2.jar Hiker.java HikerTest.java
    java -jar ../../java_libs/junit-platform-console-standalone-1.5.2.jar -cp ./ -c HikerTest
    ```
  - python
    ```
    python test_hiker.py
    ```

* mnist：在練習mnist手寫字跡辨識時，用來取得資料集，並把它轉成csv檔
* lombok：練習lombok的使用
