/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2015 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: NauxLiu                                                      |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_opencc.h"

/* If you declare any globals in php_opencc.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(opencc)
*/

/* True global resources - no need for thread safety here */
int le_opencc;

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("opencc.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_opencc_globals, opencc_globals)
    STD_PHP_INI_ENTRY("opencc.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_opencc_globals, opencc_globals)
PHP_INI_END()
*/
/* }}} */

/* {{{ proto resource opencc_open(string config)
    */
PHP_FUNCTION(opencc_open)
{
	char *config = NULL;
	int argc = ZEND_NUM_ARGS();
	int config_len;
	opencc_t od;

	if (zend_parse_parameters(argc TSRMLS_CC, "s", &config, &config_len) == FAILURE) 
		return;

	od = opencc_open(config);
	if(od == (opencc_t) -1 )
	{
		RETURN_FALSE;
	}
	#ifdef ZEND_ENGINE_3
	RETURN_RES(zend_register_resource((long)od, le_opencc));
	#else
	RETURN_RESOURCE((long)od);
	#endif
}
/* }}} */

/* {{{ proto bool opencc_close(resource ob)
    */
PHP_FUNCTION(opencc_close)
{
	int argc = ZEND_NUM_ARGS();
	int ob_id = -1;
	zval *zod = NULL;
	opencc_t od;

	if (zend_parse_parameters(argc TSRMLS_CC, "r", &zod) == FAILURE)
		return;

	#ifdef ZEND_ENGINE_3
	if ((od = (opencc_t)zend_fetch_resource(Z_RES_P(zod), "OpenCC", le_opencc)) == NULL)
		RETURN_FALSE;
	#else
	od = (opencc_t)zod->value.lval;
	#endif

	int res = opencc_close(od);

	if(res == 0)
	{
		#ifdef ZEND_ENGINE_3
		Z_TYPE_INFO_P(zod) = IS_NULL;//wtf
		#else
		Z_TYPE_P(zod) = IS_NULL;
		#endif
		RETURN_TRUE;
	}
	else
	{
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto string opencc_error()
    */
PHP_FUNCTION(opencc_error)
{
	char * msg = NULL;
	int len;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	msg = opencc_error();
	len = strlen(msg);
	#ifdef ZEND_ENGINE_3
	zend_string *ret = zend_string_alloc(len, 0);
	strncpy(ret->val, msg, len);
	RETURN_STR(ret);
	#else
	RETURN_STRINGL(msg, len, 0);
	#endif
}
/* }}} */

/* {{{ proto string opencc_convert(string str, resource ob)
    */
PHP_FUNCTION(opencc_convert)
{
	char *str = NULL;
	int argc = ZEND_NUM_ARGS();
	int str_len;
//	int ob_id = -1;
	zval *zod = NULL;
	opencc_t od;

	if (zend_parse_parameters(argc TSRMLS_CC, "sr", &str, &str_len, &zod) == FAILURE) 
		return;

	#ifdef ZEND_ENGINE_3
	if ((od = (opencc_t)zend_fetch_resource(Z_RES_P(zod), "OpenCC", le_opencc)) == NULL)
		RETURN_FALSE;
	#else
	od = (opencc_t)zod->value.lval;
	#endif

	char * outstr = opencc_convert_utf8(od, str, -1);

	int len = strlen(outstr);
	#ifdef ZEND_ENGINE_3
	zend_string *ret = zend_string_alloc(len, 0);
	strncpy(ret->val, outstr, len + 1);
	#else
	char * rs = emalloc(sizeof(char) * (len + 1));
	strncpy(rs, outstr, len + 1);
	#endif
	opencc_convert_utf8_free(outstr);
	#ifdef ZEND_ENGINE_3
	RETURN_STR(ret);
	#else
	RETURN_STRINGL(rs, len, 0);
	#endif
}
/* }}} */


/* {{{ php_opencc_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_opencc_init_globals(zend_opencc_globals *opencc_globals)
{
	opencc_globals->global_value = 0;
	opencc_globals->global_string = NULL;
}
*/
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(opencc)
{
	/* If you have INI entries, uncomment these lines 
	REGISTER_INI_ENTRIES();
	*/
	#ifdef ZEND_ENGINE_3
	le_opencc = zend_register_list_destructors_ex(NULL, NULL, "opencc_od", module_number);
	#endif
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(opencc)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(opencc)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(opencc)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(opencc)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "opencc support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */

/* {{{ opencc_functions[]
 *
 * Every user visible function must have an entry in opencc_functions[].
 */
const zend_function_entry opencc_functions[] = {
	PHP_FE(opencc_open,	NULL)
	PHP_FE(opencc_close,	NULL)
	PHP_FE(opencc_error,	NULL)
	PHP_FE(opencc_convert,	NULL)
	PHP_FE_END	/* Must be the last line in opencc_functions[] */
};
/* }}} */

/* {{{ opencc_module_entry
 */
zend_module_entry opencc_module_entry = {
	STANDARD_MODULE_HEADER,
	"opencc",
	opencc_functions,
	PHP_MINIT(opencc),
	PHP_MSHUTDOWN(opencc),
	PHP_RINIT(opencc),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(opencc),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(opencc),
	PHP_OPENCC_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_OPENCC
ZEND_GET_MODULE(opencc)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
