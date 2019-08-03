/* rust extension for PHP */

#ifndef PHP_RUST_H
# define PHP_RUST_H

extern zend_module_entry rust_module_entry;
# define phpext_rust_ptr &rust_module_entry

# define PHP_RUST_VERSION "0.1.0"

# if defined(ZTS) && defined(COMPILE_DL_RUST)
ZEND_TSRMLS_CACHE_EXTERN()
# endif

#endif	/* PHP_RUST_H */

// 来自rust的fib函数
int32_t fib(int32_t n);