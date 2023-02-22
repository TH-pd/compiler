%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ast.h"
extern int yylex();
extern int yyerror();
extern char *yytext;
struct node* top;
%}
%union{
 struct node* np;
 int ival;
 char* sp;
}
%token DEFINE ARRAY WHILE IF ELSE SEMIC L_BRACKET R_BRACKET L_PARAN R_PARAN L_BRACE R_BRACE ASSIGN ADD SUB MUL DIV EQ LT GT SURPLUS GTE LTE
%token <sp> IDENT 
%token <ival> NUMBER
%type <np> var cond_stmt loop_stmt expression term factor condition program declarations decl_statement statements statement assignment_stmt mul_op add_op cond_op

%%
program : declarations statements {top = build_node2(PROG_AST, $1, $2);}
        | statements              {top = build_node1(PROG_AST, $1);}
;
declarations : decl_statement declarations  {$$ = build_node2(DECLA_AST, $1, $2);}
             | decl_statement               {$$ = build_node1(DECLA_AST, $1);}
;
decl_statement : DEFINE IDENT SEMIC         {$$ = build_node1(DECL_AST, build_ident_node($2));}
               | ARRAY IDENT L_BRACKET NUMBER R_BRACKET SEMIC {$$ = build_node1(DECL_AST, build_array_node($2, $4));}
               | ARRAY IDENT L_BRACKET NUMBER R_BRACKET L_BRACKET NUMBER R_BRACKET SEMIC {$$ = build_node1(DECL_AST, build_array_node2d(ArryE2_AST, $2, $4, $7, "", ""));}
;
statements : statement statements {$$ = build_node2(STATEMENTS_AST, $1, $2);}
           | statement            {$$ = build_node1(STATEMENTS_AST, $1);}
;
statement : assignment_stmt {$$ = build_node1(STATEMENT_AST, $1);}
          | loop_stmt       {$$ = build_node1(STATEMENT_AST, $1);}
          | cond_stmt       {$$ = build_node1(STATEMENT_AST, $1);}
;
assignment_stmt : IDENT ASSIGN expression SEMIC                             {$$ = build_node2(ASSIGN_AST, build_ident_node($1), $3);}
                | IDENT L_BRACKET NUMBER R_BRACKET ASSIGN expression SEMIC  {$$ = build_node2(ASSIGN_AST, build_array_node($1, $3), $6);}
                | IDENT L_BRACKET IDENT R_BRACKET ASSIGN expression SEMIC  {$$ = build_node2(ASSIGN_AST, build_array_node2($1, $3), $6);}
                | IDENT L_BRACKET NUMBER R_BRACKET L_BRACKET NUMBER R_BRACKET ASSIGN expression SEMIC  {$$ = build_node2(ASSIGN_AST, build_array_node2d(ArryE2_AST, $1, $3, $6, "", ""), $9);}
                | IDENT L_BRACKET NUMBER R_BRACKET L_BRACKET IDENT R_BRACKET ASSIGN expression SEMIC  {$$ = build_node2(ASSIGN_AST, build_array_node2d(ArryE2ni_AST, $1, $3, 0, "", $6), $9);}
                | IDENT L_BRACKET IDENT R_BRACKET L_BRACKET NUMBER R_BRACKET ASSIGN expression SEMIC  {$$ = build_node2(ASSIGN_AST, build_array_node2d(ArryE2in_AST, $1, 0, $6, $3, ""), $9);}
                | IDENT L_BRACKET IDENT R_BRACKET L_BRACKET IDENT R_BRACKET ASSIGN expression SEMIC  {$$ = build_node2(ASSIGN_AST, build_array_node2d(ArryE2ii_AST, $1, 0, 0, $3, $6), $9);}
;
expression : expression add_op term {$$ = build_node3(EXP_AST, $1, $2, $3);}
           | term                   {$$ = build_node1(EXP_AST, $1);}
;
term : term mul_op factor  {$$ = build_node3(TERM_AST, $1, $2, $3);}
     | factor              {$$ = build_node1(TERM_AST, $1);}
;
factor : var
       | L_PARAN expression R_PARAN {$$ = $2;}
;
add_op : ADD {$$ = build_node0(PLUS_AST);}
       | SUB {$$ = build_node0(MINUS_AST);}
;
mul_op : MUL     {$$ = build_node0(MUL_AST);}
       | DIV     {$$ = build_node0(DIV_AST);}
       | SURPLUS {$$ = build_node0(SUR_AST);}
;
var : IDENT {$$ = build_ident_node($1);}
    | NUMBER {$$ = build_num_node($1);}
    | IDENT L_BRACKET NUMBER R_BRACKET {$$ = build_array_node($1, $3);}
    | IDENT L_BRACKET IDENT R_BRACKET  {$$ = build_array_node2($1, $3);}
    | IDENT L_BRACKET IDENT R_BRACKET L_BRACKET IDENT R_BRACKET {$$ = build_array_node2d(ArryE2ii_AST, $1, 0, 0, $3, $6);}
    | IDENT L_BRACKET IDENT R_BRACKET L_BRACKET NUMBER R_BRACKET {$$ = build_array_node2d(ArryE2in_AST, $1, 0, $6, $3, "");}
    | IDENT L_BRACKET NUMBER R_BRACKET L_BRACKET IDENT R_BRACKET {$$ = build_array_node2d(ArryE2ni_AST, $1, $3, 0, "", $6);}
    | IDENT L_BRACKET NUMBER R_BRACKET L_BRACKET NUMBER R_BRACKET {$$ = build_array_node2d(ArryE2_AST, $1, $3, $6, "", "");}
;
loop_stmt : WHILE L_PARAN condition R_PARAN L_BRACE statements R_BRACE {$$ = build_node2(WHILE_AST, $3, $6);}
;
cond_stmt : IF L_PARAN condition R_PARAN L_BRACE statements R_BRACE                                 {$$ = build_node2(IF_AST, $3, $6);}
          | IF L_PARAN condition R_PARAN L_BRACE statements R_BRACE ELSE L_BRACE statements R_BRACE {$$ = build_node3(IF_AST, $3, $6, $10);}
;
condition : expression cond_op expression {$$ = build_node3(COND_AST, $1, $2, $3);}
;
cond_op : EQ {$$ = build_node0(EQ_AST);}
        | LTE {$$ = build_node0(LTE_AST);}
        | GTE {$$ = build_node0(GTE_AST);}
        | LT {$$ = build_node0(LT_AST);}
        | GT {$$ = build_node0(GT_AST);}
;
%%
int main(void)
{
       int result;
       result = yyparse();
       if (result == 0){
              result = codegen(top);
       } else {
              fprintf(stderr, "Error\n");
       }
       return result;
}