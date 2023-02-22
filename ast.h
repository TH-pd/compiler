#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
IDENT_AST, // 変数のノード型
NUM_AST, // 整数のノード型
ASSIGN_AST, // 代入文の=のノード型
PLUS_AST, // 加算演算子のノード型
MINUS_AST, // 減算演算子のノード型
MUL_AST,   // 乗算
DIV_AST,   // 除算
WHILE_AST, // while のノード型
IF_AST, // if のノード型
PROG_AST, //program
DECLA_AST, //declarations
DECL_AST,  //decl_statement
STATEMENTS_AST, //statements
STATEMENT_AST, //statement
EXP_AST,   //算術式
TERM_AST,   //項
ArryEl_AST, //配列
EQ_AST,     //==
LT_AST,     //<
GT_AST,     //>
COND_AST,   //条件式
SUR_AST,    //剰余
LTE_AST,    //<=
GTE_AST,    //>=
ArryEl2_AST,
ArryE2_AST, //2次元
ArryE2ii_AST, //2次元
ArryE2in_AST, //2次元
ArryE2ni_AST, //2次元
} NType;

typedef struct node{
NType type;
int ivalue; // 整数の場合の値を入れる
int ivalue2; // 整数の場合の値を入れる
char* variable; // 変数の場合変数名の文字列を入れる
char* elem;     //変数を添え字とする配列の添え字
char* elem2;     //変数を添え字とする配列の添え字
struct node *child;
struct node *brother;
} Node;

typedef struct varlist {
    char* name;
    int offset;
    int volume;
    int w1;
    struct varlist *next;
} varlist ;

void prtitNode(Node* tree);
void printTree(Node* tree);
struct node* build_node0(NType t);
struct node* build_node1(NType t, Node* p1);
struct node* build_node2(NType t, Node* p1, Node* p2);
struct node* build_node3(NType t, Node* p1, Node* p2, Node* p3);
struct node* build_ident_node(char* c);
struct node* build_num_node(int n);
struct node* build_array_node(char* c, int n);
struct node* build_array_node2(char* c, char* n);
struct node* build_array_node2d(NType t, char* c, int n1, int n2, char* s1, char* s2);
void def_var(Node* p);
void make_word();
void store_ident(Node* p);
int codegen(Node* n);
void print_while(Node *p);
void print_condition(Node *p);
void print_if(Node* p);