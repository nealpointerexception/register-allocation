parser grammar simpleParser;
options { tokenVocab=simpleLexer; }

program
    : stat+ EOF
    ;

stat_list: stat+
    ;

stat: vardecl
    | if
    | while
    ;

vardecl: ID '=' expr
    ;

expr: ID
    | NUM
    | LPAREN expr RPAREN
    | expr PLUS expr
    | expr MINUS expr
    | expr GT expr
    | expr LT expr
    ;

while: WHILE LPAREN expr RPAREN LBRAC stat_list RBRAC
     ;

if: IF LPAREN expr RPAREN LBRAC stat_list RBRAC
  | IF LPAREN expr RPAREN LBRAC stat_list RBRAC ELSE LBRAC stat_list RBRAC
  ;