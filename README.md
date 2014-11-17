#介绍
opencc4php 是OpenCC的PHP扩展，能很智能的完成简繁体转换

转换效果：
`你干什么不干我事` => `你幹什麼不干我事`

地区词汇转换效果：
`我鼠标哪儿去了` => `我滑鼠哪兒去了`

#安装
你需要线安装OpenCC 1.0.1 版本以上,安装OpenCC：
```bash
git clone https://github.com/BYVoid/OpenCC.git
cd OpenCC
make
sudo make install
```
安装opencc4php:
```bash
git clone git@github.com:NauxLiu/opencc4php.git
cd opencc4php
phpize
./configure
make && sudo make install
```

#使用

###简单例子：
```php
$od = opencc_open("s2twp.json"); //传入配置文件名
$text = opencc_convert("我鼠标哪儿去了。", $od);
echo $text;
opencc_close($od);
```
输出:
`我滑鼠哪兒去了`

###函数列表：

`opencc_open(string ConfigName)`  ConfigName:配置文件名，成功返回资源对象，失败返回false  
`opencc_close(resource ob)`  关闭资源对象,成功返回true，失败返回false. 成功后od会置为NULL  
`opencc_error()` 返回最后一条错误信息，有错误信息返回String,无错误返回false  
`opencc_convert(string str, resource od)` str：要转换的字符串（必须UTF-8），od：opencc资源对象  


###可用配置
* `s2t.json` 简体到繁体 
* `t2s.json` 繁体到简体 
* `s2tw.json` 简体到台湾正体 
* `tw2s.json` 台湾正体到简体 
* `s2hk.json` 简体到香港繁体（香港小学学习字词表标准） 
* `hk2s.json` 香港繁体（香港小学学习字词表标准）到简体 
* `s2twp.json` 简体到繁体（台湾正体标准）并转换为台湾常用词汇 
* `tw2sp.json` 繁体（台湾正体标准）到简体并转换为中国大陆常用词汇


