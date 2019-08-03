#!/usr/bin/python
# -*- coding: UTF-8 -*-
import time

def fib(n):
	if n <= 2:
		return 1
	else:
		return fib(n - 1) + fib(n - 2)

start_time = time.time()
for x in xrange(1,1000000):
	v = fib(12)
print("[Python]执行时间:%s" % (time.time() - start_time))

# [Python]执行时间:56.9069800377