<?php
$br = (php_sapi_name() == "cli")? "":"<br>";

if(!extension_loaded('opencc')) {
	dl('opencc.' . PHP_SHLIB_SUFFIX);
}
$module = 'opencc';
$functions = get_extension_funcs($module);
echo "Functions available in the test extension:$br\n";
foreach($functions as $func) {
    echo $func."$br\n";
}
echo "$br\n";

$od = opencc_open('s2twp.json');
echo opencc_convert("我的鼠标哪儿去了？", $od);
echo opencc_error()."\n";
var_dump(opencc_close($od));
?>
