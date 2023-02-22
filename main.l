%{
#include "main.tab.h"
#include <string.h>
#include <stdlib.h>
/* lex for main.y */
%}
%%
"define" {;return DEFINE;}
"array" {return ARRAY;}
"while" {return WHILE;}
"if" {return IF;}
"else" {return ELSE;}
";" {return SEMIC;}
"[" {return L_BRACKET;}
"]" {return R_BRACKET;}
"(" {return L_PARAN;}
")" {return R_PARAN;}
"{" {return L_BRACE;}
"}" {return R_BRACE;}
"=" {return ASSIGN;}
"+" {return ADD;}
"-" {return SUB;}
"*" {return MUL;}
"/" {return DIV;}
"%" {return SURPLUS;}
"==" {return EQ;}
"<=" {return LTE;}
">=" {return GTE;}
"<" {return LT;}
">" {return GT;}
[a-zA-Z][a-zA-Z0-9]* {yylval.sp = strdup(yytext); return IDENT;}
[0-9]+ {yylval.ival = atoi(yytext); return NUMBER;}
%%