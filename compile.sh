#!/bin/bash

gcc -g src/tbcc.c src/util/strwindow.c src/util/readfile.c src/util/prettyerror.c src/lexer/lex.c src/lexer/lexer.c src/lexer/tokenlist.c src/parser/ast.c src/parser/parse.c src/air/aerate.c src/air/air.c -o tbcc 