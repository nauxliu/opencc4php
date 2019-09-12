--TEST--
Test opencc4php
--FILE--
<?php 
$od = opencc_open('s2twp.json');
echo opencc_convert("我的鼠标哪儿去了？", $od) . "\n";
echo opencc_convert(iconv("UTF-8", "GB2312", "错误的编码"), $od);
echo substr(opencc_error(), 0, 12);
?>
--EXPECT--
我的滑鼠哪兒去了？
Invalid UTF8