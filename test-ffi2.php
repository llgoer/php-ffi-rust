<?php
// 这个例子用于测试在Rust中生成字符串给PHP FFI调用
// 并且用完以后释放掉内存

// 下面则是测试Rust的扩展，我们采用debug、release两种模式
$libExtension = (PHP_OS_FAMILY == "Darwin" ? 'dylib' : 'so');

// debug模式
$ffiDebug = FFI::cdef(
    "char * text_generate(int8_t n);
     void text_free(char *s);",
    "r2p-fib/target/debug/libr2pfib.$libExtension");

// 扩展一个libc中的printf
$ffi = FFI::cdef(
    "int printf(const char *format, ...);", // this is a regular C declaration
    "libc.dylib");

// 将返回的char*转换为php的字符串
$time_start = microtime(true);
for ($i=0; $i < 1000000; $i++) { 
	$rstr = $ffiDebug->text_generate(12); // 这里生成文本
	$result = FFI::string($rstr); // 转成php字符串
	$ffiDebug->text_free($rstr); // 释放掉内存
}
// debug在8300ns一次
echo '[Rust]debug执行时间:' . (microtime(true) - $time_start).PHP_EOL;
var_dump($result);

$ffiRelease = FFI::cdef(
    "char * text_generate(int8_t n);
     void text_free(char *s);",
    "r2p-fib/target/release/libr2pfib.$libExtension");

// 将返回的char*转换为php的字符串
$time_start = microtime(true);
for ($i=0; $i < 1000000; $i++) { 
	$rstr = $ffiRelease->text_generate(12); // 这里生成文本
	$result = FFI::string($rstr); // 转成php字符串
	$ffiRelease->text_free($rstr); // 释放掉内存
}
// release基本上在1600ns一次
// 测试在rust中每次操作在700ns一次，那就是这里的FFI转值消耗了将近1000ns
echo '[Rust]release执行时间:' . (microtime(true) - $time_start).PHP_EOL;
var_dump($result);

// 这里能够正常打印
// $ffi->printf($ffiDebug->text_generate(12));

function text_generate($num) {
	$result = "💣";
	$result .= str_repeat("na ",$num);
	$result .= "Batman! 💣";
	return $result;
}

$time_start = microtime(true);
for ($i=0; $i < 1000000; $i++) { 
	$result = "";
	$result = text_generate(12);
}
// 这里计算出PHP这样一个生成字符串的操作，实际是需要270ns左右
echo '[PHP]执行时间:' . (microtime(true) - $time_start).PHP_EOL;
var_dump($result);
?>