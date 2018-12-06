@echo off 

if not exist sqlite3.c goto notfound 
if not exist sqlite3.h goto notfound
rem if not exist sqlite3ext.h goto notfound
if not exist shell.c goto notfound

echo dll
echo.
rem cl  /DSQLITE_ENABLE_FTS5 /DSQLITE_ENABLE_JSON1 sqlite3.c /O2 /link /dll /out:sqlite3.dll 
rem cl sqlite3.c /LD /Fesqlite3  /DSQLITE_ENABLE_FTS5 /DSQLITE_INTROSPECTION_PRAGMAS
cl sqlite3.c /LD /Fesqlite3  /DSQLITE_INTROSPECTION_PRAGMAS

echo.
@echo exe 
echo.
rem cl  /DSQLITE_ENABLE_FTS5 /DSQLITE_ENABLE_JSON1 shell.c sqlite3.c /Fesqlite3 /O1 
rem cl  shell.c sqlite3.c /Fesqlite3 /DSQLITE_ENABLE_FTS5 /DSQLITE_INTROSPECTION_PRAGMAS
cl  shell.c sqlite3.c /Fesqlite3 /DSQLITE_INTROSPECTION_PRAGMAS
del *.obj
goto done 

:notfound
echo one of sqlite source files not found 
 
:done

