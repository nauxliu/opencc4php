/*
  +----------------------------------------------------------------------+
  | PHP Version 7.1 (Backward-Compatiable)                             |
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
ZEND_DECLARE_MODULE_GLOBALS(opencc)


/* True global resources - no need for thread safety here */
int le_opencc;

typedef struct _php_opencc {
	char * config;
	opencc_t od;
} php_opencc;

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
	opencc_t od = NULL;
	php_opencc *opencc;

	// opencc_t od = OPENCC_G(global_opencc_handler);
	// if(od != (opencc_t) -1) {
		// fprintf(stderr, "reuse opencc handler [%p]\n", od);
		// #if PHP_MAJOR_VERSION < 7
			// RETURN_RESOURCE((long) od);
		// #else
			// RETURN_RES(zend_register_resource(od, le_opencc));
		// #endif
	// }
	int key_len;
	char *key;

	#if PHP_MAJOR_VERSION < 7

		zend_rsrc_list_entry *le, new_le;
		char *config = NULL;
		int config_len;
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &config, &config_len) == FAILURE) {
			return;
		}

		key_len = spprintf(&key, 0, "opecc4php_%s", config);

		if (zend_hash_find(&EG(persistent_list), key, key_len + 1,(void*) &le) == SUCCESS) {
			opencc = le->ptr;
		} else {
			od = opencc_open(config);

			opencc = pemalloc(sizeof(php_opencc), 1);
			opencc->od = od;
			opencc->config = key;

			new_le.ptr = opencc;
			new_le.type = le_opencc;

			zend_hash_add(&EG(persistent_list), key, key_len + 1, &new_le, sizeof(zend_rsrc_list_entry), NULL);
		}
		efree(key);
	#else
		zend_string *config;
		zend_resource *le, new_le;
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "S", &config) == FAILURE) {
			return;
		}

		key_len = spprintf(&key, 0, "opecc4php_%s", ZSTR_VAL(config));

		le = zend_hash_str_find_ptr(&EG(persistent_list), key, key_len);
		if (le != NULL) {
			opencc = (php_opencc*) le->ptr;

		} else {
			od = opencc_open(ZSTR_VAL(config));
			opencc = pemalloc(sizeof(php_opencc), 1);
			opencc->od = od;
			opencc->config = key;

			new_le.type = le_opencc;
			new_le.ptr = opencc;

			zend_hash_str_update_mem(&EG(persistent_list), key, key_len, &new_le, sizeof(zend_resource));
		}
 	#endif

	if(opencc == (php_opencc*) -1 ) {
		RETURN_FALSE;
	}

	// OPENCC_G(global_opencc_handler) = od;
    /* fprintf(stderr, "create a new opencc handler and store into global_opencc_handler[%p]\n", OPENCC_G(global_opencc_handler)); */
	#if PHP_MAJOR_VERSION < 7
		ZEND_REGISTER_RESOURCE(return_value, opencc, le_opencc);
	#else
		RETURN_RES(zend_register_resource(opencc, le_opencc));
	#endif
}
/* }}} */

/* {{{ proto bool opencc_close(resource ob)
    */
PHP_FUNCTION(opencc_close)
{
	//fprintf(stderr, "opencc_close() is not necessary anymore, resource will auto release on php Module shutdown\n");
	// return;

	int argc = ZEND_NUM_ARGS();
	int ob_id = -1;
	zval *zod = NULL;
	php_opencc *opencc;
	char *config;

	int key_len;
	char *key;

	if (zend_parse_parameters(argc TSRMLS_CC, "r", &zod) == FAILURE) {
		return;
	}

	#if PHP_MAJOR_VERSION < 7
	ZEND_FETCH_RESOURCE(opencc, php_opencc*, &zod, -1, "OpenCC", le_opencc);
	#else
	if ((opencc = (opencc_t)zend_fetch_resource(Z_RES_P(zod), "OpenCC", le_opencc)) == NULL) {
		RETURN_FALSE;
	}
	#endif


	#if PHP_MAJOR_VERSION < 7
	zend_list_delete(Z_RESVAL_P(zod));

	config = opencc->config;
	key_len = strlen(config);

	if (zend_hash_exists(&EG(persistent_list), key, key_len + 1)) {
		zend_hash_del(&EG(persistent_list), key, key_len + 1);
	}
	efree(key);
	#else
	zend_list_close(Z_RES_P(zod));

	config = opencc->config;
	zend_string *z_key = zend_string_init(config, strlen(config) , 0);
	if (zend_hash_exists(&EG(persistent_list), z_key)) {
		zend_hash_del(&EG(persistent_list), z_key);
	}
	#endif
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string opencc_error()
    */
PHP_FUNCTION(opencc_error)
{
	int len;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	const char *msg;
	msg = opencc_error();
	if(NULL == msg) {
		msg="";
	}
	len = strlen(msg);

	#if PHP_MAJOR_VERSION < 7
	RETURN_STRINGL(msg, len, 0);
	#else
	zend_string *ret = zend_string_alloc(len, 0);
	strncpy(ret->val, msg, len);
	RETURN_STR(ret);
	#endif
}
/* }}} */

/* {{{ proto string opencc_convert(string str, resource ob)
    */
PHP_FUNCTION(opencc_convert)
{
//	int ob_id = -1;
	zval *zod;
	opencc_t od;
	php_opencc *opencc;
	char *outstr;

	#if PHP_MAJOR_VERSION < 7
	char *str = NULL;
	int str_len;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sr", &str, &str_len, &zod) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(opencc, php_opencc*, &zod, -1, "OpenCC", le_opencc);
	od = opencc->od;
	outstr = opencc_convert_utf8(od, str, -1);

	int len = strlen(outstr);

	char * rs = emalloc(sizeof(char) * (len + 1));
	strncpy(rs, outstr, len + 1);
	opencc_convert_utf8_free(outstr);

	RETURN_STRINGL(rs, len, 0);
	#else

	zend_string *str;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Sr", &str, &zod) == FAILURE) {
		return;
	}

	if ((opencc = (php_opencc*)zend_fetch_resource(Z_RES_P(zod), "OpenCC", le_opencc)) == NULL) {
		RETURN_FALSE;
	}

	od = opencc->od;
	outstr = opencc_convert_utf8(od, str->val, -1);
	int len = strlen(outstr);

	zend_string *ret = zend_string_alloc(len, 0);
	strncpy(ret->val, outstr, len + 1);
	opencc_convert_utf8_free(outstr);

	RETURN_STR(ret);
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

static void
php_opencc_init_globals(zend_opencc_globals *opencc_globals)
{
}

#if PHP_MAJOR_VERSION < 7
static void php_opencc_persist_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	php_opencc *opencc = (php_opencc*)rsrc->ptr;
	if (opencc) {
		opencc_close(opencc->od);
	}
	pefree(opencc, 1);
}

#else
static void php_opencc_persist_dtor(zend_resource *rsrc TSRMLS_DC)
{
	php_opencc *opencc = (php_opencc*)rsrc->ptr;
	if (opencc) {
		opencc_close(opencc->od);
	}
	pefree(opencc, 1);
}
#endif

PHP_MINIT_FUNCTION(opencc)
{
	/* If you have INI entries, uncomment these lines
	REGISTER_INI_ENTRIES();
	*/

	// ZEND_INIT_MODULE_GLOBALS(opencc, php_opencc_init_globals, NULL);
	le_opencc = zend_register_list_destructors_ex(NULL, php_opencc_persist_dtor, "OpenCC", module_number);
	// OPENCC_G(global_opencc_handler) = (opencc_t) -1 ;
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
	// opencc_t od = OPENCC_G(global_opencc_handler);
	// if(od != (opencc_t) -1) {
		// int res = opencc_close(od);
		// if(res == 0) {
			// fprintf(stderr, "module shutdown closing opencc resource[%p] success!\n", od);
		// } else {
			// fprintf(stderr, "module shutdown closing opencc resource[%p] fail!\n", od);
		// }
	// }
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
	PHP_FE(opencc_open,		NULL)
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
	PHP_RINIT(opencc),
	PHP_RSHUTDOWN(opencc),
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
