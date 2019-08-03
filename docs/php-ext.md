# 创建一个PHP扩展调用Rust动态库

这里我们尝试编写一个PHP扩展，调用Rust编写的动态库。

## 生成PHP扩展目录结构

我们先使用PHP的`ext_skel`来生成一个PHP扩展源码结构。

执行
```
$ ./dist/bin/php ./php-src/ext/ext_skel.php --ext rust
Copying config scripts... done
Copying sources... done
Copying tests... done

Success. The extension is now ready to be compiled. To do so, use the
following steps:

cd /path/to/php-src/rust
phpize
./configure
make

Don't forget to run tests once the compilation is done:
make test

Thank you for using PHP!
```

我们发现在php-src的ext目录下已经生成了一个叫`rust`的目录

## 编译安装扩展

进入扩展目录，然后执行

```
./phpize
./configure
make install
```

编译完成后，修改php.ini配置，添加

```
extension=rust
```

然后再执行，查看是否成功安装刚才编译的模块：

```
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
rust
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
我们发现多了`rust`，表示模块能够被正确安装及加载。

## Rust开发一个PHP扩展

首先我们从我们的`r2p-fib`中倒入我们用Rust编写的fib函数。

在`php_rust.h`中定义

```
// 来自rust的fib函数
int32_t fib(int32_t n);
```

然后在`rust.c`中实现我们扩展的处理
```
// 这里处理参数
ZEND_BEGIN_ARG_INFO(arginfo_rust_fib, 0)
	ZEND_ARG_INFO(0, number)
ZEND_END_ARG_INFO()

// 这里具体的实现
/* {{{ int rust_fib( [ int $var ] )
 */
PHP_FUNCTION(rust_fib)
{
	zend_long number = 0;
	zend_long result = 0;
	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(number)
	ZEND_PARSE_PARAMETERS_END();

	if (number == 0) {
		RETURN_LONG(result);
	} else {
		result = fib(number);
		RETURN_LONG(result);
	}
}
/* }}}*/

```
这里我们只是贴出核心的代码部分。

为了更好的测试，我们同时编写了一个C的版本

```
// 这里直接编写一个C的fib调用
int fibinC(int n) 
{ 
    if (n <= 1) 
        return n; 
    return fibinC(n - 1) + fibinC(n - 2); 
} 
/* {{{ int c_fib( [ int $var ] )
 */
PHP_FUNCTION(c_fib)
{
	zend_long number = 0;
	zend_long result = 0;
	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(number)
	ZEND_PARSE_PARAMETERS_END();

	if (number == 0) {
		RETURN_LONG(result);
	} else {
		result = fibinC(number);
		RETURN_LONG(result);
	}
}
/* }}}*/
ZEND_BEGIN_ARG_INFO(arginfo_c_fib, 0)
	ZEND_ARG_INFO(0, number)
ZEND_END_ARG_INFO()
```

## 重新编译运行

这里编译需要增加对我们之前动态链接库的引用。

然后我们执行命令

```
$ ./php -r "echo rust_fib(12).PHP_EOL;"
144
```
好了，144就是我们计算成功的结果。

## 性能测试

接下来我们扩充下我们之前的`test-ffi.php`，在其中加入：

```
// 这里我们采用Rust编写的一个PHP扩展
$time_start = microtime(true);
for ($i=0; $i < 10000000; $i++) { 
	$v = rust_fib(12);
}

echo '[Rust]Ext执行时间:' . (microtime(true) - $time_start).PHP_EOL;
```

执行`make test-php`，运行结果如下：

```
[PHP]fib执行时间:35.900850057602
[Rust]Debug执行时间:22.036439180374
[Rust]Release执行时间:6.8401432037354
[Rust]Ext执行时间:5.19260597229
[C]Ext执行时间:13.217513799667
```
看来用Rust开发PHP的扩展性能上面是完全可以的。

完整的PHP扩展代码可以查看`ext/rust`目录。