/* rust extension for PHP */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "php_rust.h"

// 这里直接编写一个C的fib调用
int fibinC(int n) 
{ 
    if (n <= 1) 
        return n; 
    return fibinC(n - 1) + fibinC(n - 2); 
} 


/* For compatibility with older PHP versions */
#ifndef ZEND_PARSE_PARAMETERS_NONE
#define ZEND_PARSE_PARAMETERS_NONE() \
	ZEND_PARSE_PARAMETERS_START(0, 0) \
	ZEND_PARSE_PARAMETERS_END()
#endif

/* {{{ void rust_test1()
 */
PHP_FUNCTION(rust_test1)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_printf("The extension %s is loaded and working!\r\n", "rust");
}
/* }}} */

/* {{{ string rust_test2( [ string $var ] )
 */
PHP_FUNCTION(rust_test2)
{
	char *var = "World";
	size_t var_len = sizeof("World") - 1;
	zend_string *retval;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_STRING(var, var_len)
	ZEND_PARSE_PARAMETERS_END();

	retval = strpprintf(0, "Hello %s", var);

	RETURN_STR(retval);
}
/* }}}*/

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

/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(rust)
{
#if defined(ZTS) && defined(COMPILE_DL_RUST)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(rust)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "rust support", "enabled");
	php_info_print_table_end();
}
/* }}} */

/* {{{ arginfo
 */
ZEND_BEGIN_ARG_INFO(arginfo_rust_test1, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_rust_test2, 0)
	ZEND_ARG_INFO(0, str)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_rust_fib, 0)
	ZEND_ARG_INFO(0, number)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_c_fib, 0)
	ZEND_ARG_INFO(0, number)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ rust_functions[]
 */
static const zend_function_entry rust_functions[] = {
	PHP_FE(rust_test1,		arginfo_rust_test1)
	PHP_FE(rust_test2,		arginfo_rust_test2)
	PHP_FE(rust_fib,		arginfo_rust_fib)
	PHP_FE(c_fib,		arginfo_c_fib)
	PHP_FE_END
};
/* }}} */

/* {{{ rust_module_entry
 */
zend_module_entry rust_module_entry = {
	STANDARD_MODULE_HEADER,
	"rust",					/* Extension name */
	rust_functions,			/* zend_function_entry */
	NULL,							/* PHP_MINIT - Module initialization */
	NULL,							/* PHP_MSHUTDOWN - Module shutdown */
	PHP_RINIT(rust),			/* PHP_RINIT - Request initialization */
	NULL,							/* PHP_RSHUTDOWN - Request shutdown */
	PHP_MINFO(rust),			/* PHP_MINFO - Module info */
	PHP_RUST_VERSION,		/* Version */
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_RUST
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(rust)
#endif
