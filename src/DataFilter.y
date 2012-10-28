%{
/*
 * Copyright (c) 2012 Mark Liversedge (liversedge@gmail.com)
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

// This grammar should work with yacc and bison, but has
// only been tested with bison. In addition, since qmake
// uses the -p flag to rename all the yy functions to
// enable multiple grammars in a single executable you
// should make sure you use the very latest bison since it
// has been known to be problematic in the past. It is
// know to work well with bison v2.4.1.
//
// To make the grammar readable I have placed the code
// for each nterm at column 40, this source file is best
// edited / viewed in an editor which is at least 120
// columns wide (e.g. vi in xterm of 120x40)
//
//

#include "DataFilter.h"

extern int DataFilterlex(); // the lexer aka yylex()
extern char *DataFiltertext; // set by the lexer aka yytext

extern void DataFilter_setString(QString);
extern void DataFilter_clearString();

// PARSER STATE VARIABLES
extern QStringList DataFiltererrors;
static void DataFiltererror(const char *error) { DataFiltererrors << QString(error);}
extern int DataFilterparse();

extern Leaf *root; // root node for parsed statement

%}

// Symbol can be meta or metric name
%token <leaf> SYMBOL

// Constants can be a string or a number
%token <leaf> STRING INTEGER FLOAT

// comparative operators
%token <op> EQ NEQ LT LTE GT GTE 
%token <op> MATCHES ENDSWITH BEGINSWITH CONTAINS

// logical operators
%token <op> AND OR

%union {
   Leaf *leaf;
   int op;
}

%type <leaf> value lexpr;
%type <op> lop op;

%start filter;
%%

filter: lexpr                       { root = $1; }
        ;

lexpr : '(' lexpr ')'               { $$ = new Leaf();
                                      $$->type = Leaf::Logical;
                                      $$->lvalue.l = $2;
                                      $$->op = 0; }

      | lexpr lop lexpr             { $$ = new Leaf();
                                      $$->type = Leaf::Logical;
                                      $$->lvalue.l = $1;
                                      $$->op = $2;
                                      $$->rvalue.l = $3; }
                                    
      | value op value              { $$ = new Leaf();
                                      $$->type = Leaf::Operation;
                                      $$->lvalue.l = $1;
                                      $$->op = $2;
                                      $$->rvalue.l = $3; }
      ;


op    : EQ
      | NEQ
      | LT
      | LTE
      | GT
      | GTE
      | MATCHES
      | ENDSWITH
      | BEGINSWITH
      | CONTAINS
      ;

lop   : AND
      | OR
      ;

value : SYMBOL                      { $$ = new Leaf(); $$->type = Leaf::Symbol;
                                      $$->lvalue.n = new QString(DataFiltertext); }
      | STRING                      { $$ = new Leaf(); $$->type = Leaf::String;
                                      $$->lvalue.s = new QString(DataFiltertext); }
      | FLOAT                       { $$ = new Leaf(); $$->type = Leaf::Float;
                                      $$->lvalue.f = QString(DataFiltertext).toFloat(); }
      | INTEGER                     { $$ = new Leaf(); $$->type = Leaf::Integer;
                                      $$->lvalue.i = QString(DataFiltertext).toInt(); }
      ;

%%
