# README #

JSF專案範例.藉此提供[專案開發方式改進意見](https://docs.google.com/document/d/1dd1zEDo26brGIUzGJ9tRa1Gz-gxEWdE91nUpPZJnlSg/edit#heading=h.gxethljtesbm)的示範性實作，專案基本架構是根據myfaces-archetype-helloworld20:1.0.4這個archetype所建立。

### 專案目的 ###

* 作為日後JSF專案（ex：JSF Portlet）的範例，裡頭使用了JSF、JPA、Hibernate、Mysql、Junit、dbUnit、Mockito、Maven、Checkstyle等多項工具.
* 版本：
 * 0.1：完成基本的示範內容，包含coding convention、透過JPA來使用Hibernate、單元測試
 * 0.2：把專案內容改為一個簡單的訪客留言簿、加上Bootstrap、新增留言、立即更新留言清單

### 如何設置開發環境? ###

* git clone本專案
* 標準maven專案，毋須特別設定
* 環境依存：Java 7、Maven 3
* 資料庫設置：使用Mysql資料庫，如有需要，請參考persistence.xml進行修改
* 如何執行測試：簡單，執行 `mvn test` 即可
* 如何部署：在IDE裡只要run on server，若要手動部署到任何一種Servlet container上，執行 `mvn package`，再以產生的war檔來進行部署

### Contribution guidelines ###

* Writing tests
* Code review
* Other guidelines

### Who do I talk to? ###

* Repo owner or admin
* Other community or team contact
