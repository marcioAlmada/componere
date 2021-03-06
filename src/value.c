/*
  +----------------------------------------------------------------------+
  | componere                                                            |
  +----------------------------------------------------------------------+
  | Copyright (c) Joe Watkins 2018                                       |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: krakjoe <krakjoe@php.net>                                    |
  +----------------------------------------------------------------------+
 */
#ifndef HAVE_COMPONERE_VALUE
#define HAVE_COMPONERE_VALUE

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include <php.h>
#include <zend_closures.h>
#include <zend_exceptions.h>
#include <ext/spl/spl_exceptions.h>

#include "src/value.h"

zend_class_entry *php_componere_value_ce;
zend_object_handlers php_componere_value_handlers;

static inline zend_object* php_componere_value_create(zend_class_entry *ce) {
	php_componere_value_t *o = 
		(php_componere_value_t*) 
			ecalloc(1, sizeof(php_componere_value_t));

	zend_object_std_init(&o->std, ce);

	object_properties_init(&o->std, ce);

	o->std.handlers = &php_componere_value_handlers;

	return &o->std;
}

static inline void php_componere_value_destroy(zend_object *zo) {
	php_componere_value_t *o = php_componere_value_from(zo);

	if (Z_REFCOUNTED(o->value)) {
		zval_ptr_dtor(&o->value);
	}

	zend_object_std_dtor(&o->std);
}

ZEND_BEGIN_ARG_INFO_EX(php_componere_value_construct, 0, 0, 1)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

PHP_METHOD(Value, __construct)
{
	php_componere_value_t *o = php_componere_value_fetch(getThis());
	zval *value = NULL;

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS(), "z", &value) != SUCCESS) {
		zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0, "value expected");
		return;
	}

	switch (Z_TYPE_P(value)) {
		case IS_STRING:
		case IS_LONG:
		case IS_DOUBLE:
		case IS_TRUE:
		case IS_FALSE:
		case IS_NULL:
			ZVAL_COPY(&o->value, value);
		break;
		
		default:
			zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0,
				"values of type %s cannot be declared with default values",
				Z_TYPE_P(value) == IS_OBJECT ?
					ZSTR_VAL(Z_OBJCE_P(value)->name) :
					zend_get_type_by_const(Z_TYPE_P(value)));
	}

	o->access = ZEND_ACC_PUBLIC;
}

ZEND_BEGIN_ARG_INFO(php_componere_value_protected, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Value, setProtected)
{
	php_componere_value_t *o = php_componere_value_fetch(getThis());

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS(), "") != SUCCESS) {
		zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0, "no arguments expected");
		return;
	}

	if ((o->access & ZEND_ACC_PPP_MASK) > ZEND_ACC_PUBLIC) {
		zend_throw_exception_ex(spl_ce_RuntimeException, 0,
			"access level already set");
		return;
	}

	o->access = ZEND_ACC_PROTECTED;

	RETURN_ZVAL(getThis(), 1, 0);
}

ZEND_BEGIN_ARG_INFO(php_componere_value_private, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Value, setPrivate)
{
	php_componere_value_t *o = php_componere_value_fetch(getThis());

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS(), "") != SUCCESS) {
		zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0, "no arguments expected");
		return;
	}

	if ((o->access & ZEND_ACC_PPP_MASK) > ZEND_ACC_PUBLIC) {
		zend_throw_exception_ex(spl_ce_RuntimeException, 0,
			"access level already set");
		return;
	}

	o->access = ZEND_ACC_PRIVATE;

	RETURN_ZVAL(getThis(), 1, 0);
}

static zend_function_entry php_componere_value_methods[] = {
	PHP_ME(Value, __construct, php_componere_value_construct, ZEND_ACC_PUBLIC)
	PHP_ME(Value, setProtected, php_componere_value_protected, ZEND_ACC_PUBLIC)
	PHP_ME(Value, setPrivate, php_componere_value_private, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

PHP_MINIT_FUNCTION(Componere_Value) {
	zend_class_entry ce;

	INIT_NS_CLASS_ENTRY(ce, "Componere", "Value", php_componere_value_methods);

	php_componere_value_ce = zend_register_internal_class(&ce);
	php_componere_value_ce->create_object = php_componere_value_create;

	memcpy(&php_componere_value_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));

	php_componere_value_handlers.offset = XtOffsetOf(php_componere_value_t, std);
	php_componere_value_handlers.free_obj = php_componere_value_destroy;

	return SUCCESS;
}

#endif
