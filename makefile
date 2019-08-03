
# 测试fib函数
test-php:
	./dist/bin/php test-ffi.php

# 测试python下fib
test-python:
	python test-fib.py

# 测试字符串调用
test-php2:
	./dist/bin/php test-ffi2.php