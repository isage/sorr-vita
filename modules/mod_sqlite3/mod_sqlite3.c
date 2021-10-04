/*
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This file is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 *  Initial release by ColDev. Improvements by Osk.
 */

/* --------------------------------------------------------------------------- */

#include <stdio.h>
#include <sqlite3.h>
#include "bgddl.h"
#include "xstrings.h"
#ifndef __MONOLITHIC__
#include "mod_sqlite3_symbols.h"
#endif

typedef struct
{
  int cols,
      rows;
  int CurrentRow;
  char **pazResult;//internal use
} SqlResult ;

/* ----------------------------------------------------------------- */
/*                  Function definitions                           */

static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{/*
  int i;
  for(i=0; i<argc; i++){
    printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
  }
  printf("\n");
*/
  return 0;
}


CONDITIONALLY_STATIC int modsqlite3_enableCache(INSTANCE * my, int * params)
{
  return sqlite3_enable_shared_cache(params[0]);
}


CONDITIONALLY_STATIC int modsqlite3_openDb (INSTANCE * my, int * params)
{
    sqlite3 *db;
    const char * text = (const char *)string_get (params[0]) ;

    int rc= sqlite3_open_v2(text,&db,params[1],0);
    if( rc==SQLITE_OK ) rc= (int)db;
    else {
	   rc=0;
	   sqlite3_close(db);
	   }

    string_discard (params[0]) ;
    return rc;
}


CONDITIONALLY_STATIC int modsqlite3_closeDb (INSTANCE * my, int * params)
{
    if (params[0]) sqlite3_close((sqlite3 *)params[0]);
    return 1;
}


CONDITIONALLY_STATIC int modsqlite3_execDb (INSTANCE * my, int * params)
{
  char *zErrMsg = 0;
  const char * text = (const char *)string_get (params[1]) ;

  int  rc = sqlite3_exec((sqlite3 *)params[0],text, callback, 0, &zErrMsg);

  if( rc!=SQLITE_OK ){
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }
  string_discard (params[1]) ;
  return rc;
}


CONDITIONALLY_STATIC int modsqlite3_openTable (INSTANCE * my, int * params)
{
  char *zErrMsg = 0;
  const char * text = (const char *)string_get (params[1]) ;
  SqlResult *resultado= (SqlResult *)params[2];

  int  rc = sqlite3_get_table((sqlite3 *)params[0],text,&resultado->pazResult, &resultado->rows ,&resultado->cols,&zErrMsg);

  if( rc!=SQLITE_OK ){
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }
  resultado->CurrentRow=1;
  string_discard (params[1]) ;
  return rc;
}


CONDITIONALLY_STATIC int modsqlite3_closeTable (INSTANCE * my, int * params)
{
  SqlResult *resultado= (SqlResult *)params[0];

  sqlite3_free_table(resultado->pazResult);
  return 1;
}


CONDITIONALLY_STATIC int modsqlite3_getFieldName (INSTANCE * my, int * params)
{
  SqlResult *resultado= (SqlResult *)params[0];
  int res=0;

  res= string_new("");
  string_concat (res,resultado->pazResult[params[1]]);
  string_use(res) ;
  return res;
}


CONDITIONALLY_STATIC int modsqlite3_getFieldValue (INSTANCE * my, int * params)
{
  SqlResult *resultado= (SqlResult *)params[0];
  int res=0;
  int i=(resultado->cols * resultado->CurrentRow)+ params[1];

  res= string_new("");
  if (resultado->pazResult[i])
       string_concat (res,resultado->pazResult[i]);

  string_use(res) ;
  return res;
}


CONDITIONALLY_STATIC int modsqlite3_lastId (INSTANCE * my, int * params)
{
  return sqlite3_last_insert_rowid((sqlite3*)params[0]);
}


CONDITIONALLY_STATIC int modsqlite3_totalChanges(INSTANCE * my, int * params)
{
  return sqlite3_total_changes((sqlite3*)params[0]);
}


CONDITIONALLY_STATIC int modsqlite3_errMsg (INSTANCE * my, int * params)
{
  int res=0;
  const char *cad= sqlite3_errmsg((sqlite3*)params[0]);

  res= string_new("");
  string_concat (res,cad);
  string_use(res) ;
  return res;
}

/* --------------------------------------------------------------------------- */


/*Pendientes: sqlite_get_table, sql_prepare,step,reset y finalize, sqlite_bind_tipodato, sqlite_column_count ,sqlite_column_tipodato...
Consultar: http://www.sqlite.org/c3ref/funclist.html
Consultar: http://www.sqlite.org/c3ref/objlist.html
Consultar: http://www.sqlite.org/c3ref/constlist.html
Consultar: http://www.sqlite.org/cintro.html*/

/* --------------------------------------------------------------------------- */
