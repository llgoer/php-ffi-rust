<?php
// 一个PHP的fib函数
function fib($n) {
	if ($n < 0) {
		return NULL;
	} elseif ($n === 0) {
		return 0;
	} elseif ($n === 1 || $n === 2) {
		return 1;
	} else {
		return fib($n-1) + fib($n-2);
	}	 
}

// 这里测试PHP下的Fib函数
$time_start = microtime(true);
for ($i=0; $i < 1000000; $i++) { 
	$v = fib(12);
}
echo '[PHP]fib执行时间:' . (microtime(true) - $time_start).PHP_EOL;


// 下面则是测试Rust的扩展，我们采用debug、release两种模式
$libExtension = (PHP_OS_FAMILY == "Darwin" ? 'dylib' : 'so');

// debug模式
$ffiDebug = FFI::cdef(
    "int32_t fib(int32_t n);",
    "r2p-fib/target/debug/libr2pfib.$libExtension");

$time_start = microtime(true);
for ($i=0; $i < 10000000; $i++) { 
	$v = $ffiDebug->fib(12);
}

echo '[Rust]Debug执行时间:' . (microtime(true) - $time_start).PHP_EOL;

// release模式
$ffiRelease = FFI::cdef(
    "int32_t fib(int32_t n);",
    "r2p-fib/target/release/libr2pfib.$libExtension");

$time_start = microtime(true);
for ($i=0; $i < 10000000; $i++) { 
	$v = $ffiRelease->fib(12);
}

echo '[Rust]Release执行时间:' . (microtime(true) - $time_start).PHP_EOL;
?>