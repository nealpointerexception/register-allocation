lexer grammar simpleLexer;

GT: '>';
LT: '<';
EQ : '=' ;
PLUS: '+';
MINUS: '-';
SEMI : ';' ;
LPAREN : '(' ;
RPAREN : ')' ;
LBRAC : '{' ;
RBRAC : '}' ;
WHILE: 'while';
IF: 'if';
ELSE: 'else';

fragment INT : [0] | [1-9][0-9]* ;
NUM : INT;
ID: [a-zA-Z_][a-zA-Z_0-9]* ;
WS: [ \t\n\r\f]+ -> skip ;