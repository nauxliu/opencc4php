#介绍

[![Build Status](https://travis-ci.org/NauxLiu/opencc4php.svg?branch=master)](https://travis-ci.org/NauxLiu/opencc4php)

opencc4php 是[OpenCC](https://github.com/BYVoid/OpenCC)的PHP扩展，能很智能的完成简繁体转换。 

支持 PHP 版本： 5.3 - 7.0

###转换效果：

`你干什么不干我事` => `你幹什麼不干我事`

地区词汇转换效果：
`我鼠标哪儿去了` => `我滑鼠哪兒去了`

#安装

#### Linux | OS X

你需要先安装`1.0.1` 版本以上的OpenCC，安装OpenCC：
```
git clone https://github.com/BYVoid/OpenCC.git --depth 1
cd OpenCC
make
sudo make install
```
安装opencc4php：
```
git clone git@github.com:NauxLiu/opencc4php.git --depth 1
cd opencc4php
phpize
./configure
make && sudo make install
```
>如果你的OpenCC安装目录不在`/usr`或`/usr/local`，可在`./configure`时添加`--with-opencc=[DIR]`指定你的OpenCC目录


#### Windows
[Windows 安装说明](https://github.com/NauxLiu/opencc4php/pull/16)

#使用

###实例：
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
`opencc_convert(string str, resource od)` str：要转换的字符串(UTF-8)，od：opencc资源对象  


###可用配置
* `s2t.json` 简体到繁体 
* `t2s.json` 繁体到简体 
* `s2tw.json` 简体到台湾正体 
* `tw2s.json` 台湾正体到简体 
* `s2hk.json` 简体到香港繁体（香港小学学习字词表标准） 
* `hk2s.json` 香港繁体（香港小学学习字词表标准）到简体 
* `s2twp.json` 简体到繁体（台湾正体标准）并转换为台湾常用词汇 
* `tw2sp.json` 繁体（台湾正体标准）到简体并转换为中国大陆常用词汇

# 贡献列表
[@刘相轩](https://github.com/NauxLiu)  
[@Kyle Tse](https://github.com/shtse8)  
[@Alliumcepa Triplef](https://github.com/fffonion)  
