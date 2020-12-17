## 使用Selenium來產生大量訓練資料的實驗性專案

* 在pom file裡已經定義好main class及預設參數
* 執行：mvn exec:java
* 用exec:java -Dexec.args="http://www.udn.com udn"來覆蓋預設參數 
* 也可以加上第三個參數（整數）來限制開啟的鏈結數目上限
