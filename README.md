# Arduino-misakiUTF16
Arduino用 美咲フォントライブラリ 教育漢字・内部フラッシュメモリ乗せ版

##概要
Arduino用の美咲フォントドライバライブラリです。

フォントを教育漢字1,006字(小学校で習う漢字）＋ひらがな・カタカナ・記号・半角等の1,710字に絞って  
Arduino Uno(Atmega328)のフラッシュメモリ上に格納しました。  

##仕様
* 文字コード  UTF16/UTF-8  
* フォントサイズ  8x8ドッド（美咲フォント)  
* 利用可能フォント数  1,710字（Arduinoのフラッシュメモリ上に格納）  
  * 漢字 教育漢字 1,006字(小学校で習う漢字）  
  * 非漢字 全角 546字(全角英数字、ひらがな、かたかな、記号)  
  * 半角フォント  158字(半角記号、半角英数、半角カタカナ）  
  
##利用可能API
* UTF8文字列をUTF16文字列に変換する  
  byte Utf8ToUtf16(uint16_t* pUTF16, char *pUTF8)  

* UTF8文字をUTF16に変換  
  byte charUFT8toUTF16(char *pUTF8, uint16_t *pUTF16)

* UTF16半角コードをUTF16全角コードに変換  
  uint16_t hkana2kana(uint16_t ucode);  

* 半角カナを全角に変換  
  uint16_t hkana2kana(uint16_t ucode)

* UTF16に対応するフォントデータ取得  
  boolean getFontDataByUTF16(byte* fontdata, uint16_t utf16) 

* UTF8文字列に対応する先頭文字のフォントデータ取得  
  char* getFontData(byte* fontdata,char *pUTF8)

*  フォントデータテーブル先頭アドレス取得  
  const uint8_t* getFontTableAddress()

##サンプルスケッチ
banner  
![banner](img/sample.png)

misaki_tvout  
![misaki_tvout](img/misaki_tvout.jpg)

aitendo_ht16k33  
![aitendo_ht16k33](img/aitendo_ht16k33.jpg)
