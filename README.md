# php-ffi-rust

本项目目标就是采用Rust编写扩展库，提供给PHP7.4的FFI扩展，看看是否能够抢救一下PHP。

## 拓展

[创建一个PHP扩展调用Rust动态库](docs/php-ext.md)

## 测试环境

macOS：10.13.6

PHP：7.4.0beta1

Rust：1.36.0

## 为macOS编译PHP

由于MacOS下没有找到合适的PHP7.4版本，所以只能自己动手编译。

### 下载

从PHP的[Github Release](https://github.com/php/php-src/releases)页面下载PHP7.4.0beta1版本。

### 编译PHP

确保系统安装`autoconf`，如果没有安装，执行`brew install autoconf`进行安装。

确保系统安装`bison`,`re2c`,`libffi`。

```
$ ./buildconf --force
$ ./configure --prefix=/Users/llgoer/php-ffi-rust/dist --with-ffi 
$ make && make install
```

这里注意，如果编译没有找到libffi，可以采用
```
FFI_CFLAGS=/xxx/libffi-3.2.1/include FFI_LIBS=/xxx/libffi/3.2.1/lib ./configure ...
```
指定

### 运行PHP7.4

进入`dist`目录，我们可以看到我们已经编译好的PHP7.4。

```shell
$ ./php -version
PHP 7.4.0beta1 (cli) (built: Aug  2 2019 22:12:05) ( NTS )
Copyright (c) The PHP Group
Zend Engine v3.4.0-dev, Copyright (c) Zend Technologies

$ ./php -m
[PHP Modules]
Core
ctype
date
dom
FFI
fileinfo
filter
hash
iconv
json
libxml
pcre
PDO
pdo_sqlite
Phar
posix
Reflection
session
SimpleXML
SPL
sqlite3
standard
tokenizer
xml
xmlreader
xmlwriter

[Zend Modules]

```
如果存在FFI扩展则表示编译正常。

## 安装Rust环境

这里就不多说了，可以去Rust官方网站看看。

## Rust编写一个动态连接库

执行

```shell
$ cargo new r2p-fib --lib
```

创建一个rust库，修改`Cargo.toml`，配置项目生成动态链接库

```toml
[package]
name = "r2p-fib"
version = "0.1.0"
authors = ["llgoer <llgoer@github.com>"]
edition = "2018"

[dependencies]
name = "r2pfib"
crate-type = ["dylib"]
```

在lib.rs中创建一个导出的fib函数：

```rust
#[no_mangle]
pub extern fn fib(n: i32) -> i32 {
  return match n {
    1 | 2 => 1,
    n => fib(n - 1) + fib(n - 2)
  }
}
```

执行`cargo build`以及`cargo build --release`，生成对应的扩展库。

## 编写PHP代码

编写test-ffi.php
```php
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
for ($i=0; $i < 100000; $i++) { 
	fib(12);
}
echo '[PHP]fib执行时间:' . (microtime(true) - $time_start).PHP_EOL;


// 下面则是测试Rust的扩展，我们采用debug、release两种模式
$libExtension = (PHP_OS_FAMILY == "Darwin" ? 'dylib' : 'so');

// debug模式
$ffiDebug = FFI::cdef(
    "int32_t fib(int32_t n);",
    "r2p-fib/target/debug/libr2pfib.$libExtension");

$time_start = microtime(true);
for ($i=0; $i < 1000000; $i++) { 
	$ffiDebug->fib(12);
}

echo '[Rust]Debug执行时间:' . (microtime(true) - $time_start).PHP_EOL;

// release模式
$ffiRelease = FFI::cdef(
    "int32_t fib(int32_t n);",
    "r2p-fib/target/release/libr2pfib.$libExtension");

$time_start = microtime(true);
for ($i=0; $i < 1000000; $i++) { 
	$ffiRelease->fib(12);
}

echo '[Rust]Release执行时间:' . (microtime(true) - $time_start).PHP_EOL;
?>
```

## 查看测试结果

执行`make test-php`查看结果，看看惊不惊喜，意不意外。

```
[PHP]fib执行时间:3.624843120575
[Rust]Debug执行时间:2.0988941192627
[Rust]Release执行时间:0.56653189659119
```

## 字符串测试

我们编写了一个生成字符串的调用测试，详细可以参考`test-ffi2.php`。

这个例子告诉我们，这里频繁调用字符串解析是比较消耗性能的。

```
[Rust]debug执行时间:7.8468458652496
string(52) "💣na na na na na na na na na na na na Batman! 💣"
[Rust]release执行时间:1.4546401500702
string(52) "💣na na na na na na na na na na na na Batman! 💣"
[PHP]执行时间:0.23888492584229
string(52) "💣na na na na na na na na na na na na Batman! 💣"
```

## 总结

目前PHP同Rust结合有几种方式：1.PHP的FFI调用Rust导出的库；2.编写PHP扩展调用Rust动态库；3.直接采用Rust编写PHP扩展库；

第一种目前并不成熟，而且受限于PHP7.4+，所以不是很推荐，仅限于兴趣研究，另外FFI性能损耗也是比较大，频繁调用处理非常不推荐。

第二种比较实用，如果想为PHP添加一些额外扩展也是非常推荐这种方式，将Rust开发的库按照格式导出再交给PHP调用处理。

第三种基本上需要将PHP的C API导出为Rust方法，然后直接在Rust中编写PHP的扩展，这种方式目前还没尝试。

