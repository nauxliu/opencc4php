dnl $Id$
dnl config.m4 for extension opencc

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

PHP_ARG_WITH(opencc, for opencc support,
dnl Make sure that the comment is aligned:
[  --with-opencc=[DIR]             Include opencc support])

dnl Otherwise use enable:

dnl PHP_ARG_ENABLE(opencc, whether to enable opencc support,
dnl Make sure that the comment is aligned:
dnl [  --enable-opencc           Enable opencc support])

if test "$PHP_OPENCC" != "no"; then
  dnl # --with-opencc -> check with-path
   SEARCH_PATH="/usr/local /usr"     # you might want to change this
   SEARCH_FOR="/include/opencc/opencc.h"  # you most likely want to change this
   if test -r $PHP_OPENCC/$SEARCH_FOR; then # path given as parameter
     AC_MSG_CHECKING([for opencc files in given path])
     OPENCC_DIR=$PHP_OPENCC
     AC_MSG_RESULT(found in $OPENCC_DIR)
   else # search default path list
     AC_MSG_CHECKING([for opencc files in default path])
     for i in $SEARCH_PATH ; do
       if test -r $i/$SEARCH_FOR; then
         OPENCC_DIR=$i
         AC_MSG_RESULT(found in $i)
       fi
     done
   fi
  
  if test -z "$OPENCC_DIR"; then
     AC_MSG_RESULT([not found])
     AC_MSG_ERROR([Please reinstall the opencc distribution])
  fi

  dnl # --with-opencc -> add include path
  PHP_ADD_INCLUDE($OPENCC_DIR/include/opencc)

  dnl # --with-opencc -> check for lib and symbol presence
  LIBNAME=opencc # you may want to change this
  LIBSYMBOL=libopencc.so # you most likely want to change this 
    
  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
    PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $OPENCC_DIR/$PHP_LIBDIR, OPENCC_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_OPENCCLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong opencc lib version or lib not found])
  dnl ],[
  dnl   -L$OPENCC_DIR/$PHP_LIBDIR -lm
  dnl ])
  
  PHP_SUBST(OPENCC_SHARED_LIBADD)

  PHP_NEW_EXTENSION(opencc, opencc.c, $ext_shared)
fi