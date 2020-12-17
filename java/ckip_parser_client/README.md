# ckipparserclient
Java Client Side for CKIP Parser

Open source my working with CKIP Parser will all other developers, cheers !

Usage:

## construct client instance:
```
Client client = new Client(IP_ADDRESS_OF_CKIP_PARSER, PORT_NUMBER_OF_CKIP_PARSER, YOUR_ACCOUNT, YOUR_PASSWORD);

ParserResult result = client.parser("台新金控12月3日將召開股東臨時會進行董監改選。");
```
如果要使用樹狀結構來檢視parser的分機結果，則使用：如果要使用樹狀結構來檢視parser的分機結果，則使用：
```
Tree<Term> tree = ParserUtil.taggedTextToTermTree(resultgetSentence());
```
上頭的result是ParserResult這個類別的一個instance
