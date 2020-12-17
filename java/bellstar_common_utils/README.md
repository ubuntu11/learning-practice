# common-utils

把公司各類Java專案中可能會共用到的工具類別集中在這裡，透過maven、ivy、nexus server的組合來實現相依性管理。第一個實作專案會是call center portlet。

既然是共用的工具類別，裡頭的東西應該具備普遍性，如果是特別為了某個專案的某個用途而寫的東西，要想一想：它是否真有必要放在這裡？

# 建置方式

## 設定好你的maven環境

參考wiki page : Maven Settings

## 設定好你的IDE

由於用了Lombok project，來自動生成accessor（getter、setter），並提供了builder pattern，可以快速地生成實例，所以IDE需要做些調整。
下載Lombok project的1.14.8版本（是一個jar檔），然後執行：

`java -jar lombok-1.14.8.jar`

它會出現圖型介面，如果沒有自動找到你的Eclipse安裝，就手動指定Location，挑選到你的Eclipse裡的eclipse.ini，double click它。

## 開發

由於這像是大家共用的函式庫，所以必須Follow我們的[coding convetion](https://github.com/joshua60/Call-Center/wiki/Coding-Convention)，並且應該送出pull request，好讓大家知道發生了怎樣的異動。

## 建置與發佈

mvn clean deploy
