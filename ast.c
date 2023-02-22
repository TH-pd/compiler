#include <stdio.h>
#include "ast.h"

#define DATA_SEG 0x10004000

struct varlist Varlist = {NULL, 0, 0, 0, NULL};
int L = 0;

void prtitNode(Node* tree) {
    switch (tree->type)
    {
    case NUM_AST:
        printf("%d", tree->ivalue);
        break;
    case IDENT_AST:
        printf("%s", tree->variable);
        break;    
    default:
        break;
    }
}

void printTree(Node* tree) {
    if (tree->child == NULL) {
        prtitNode(tree);
    } else {
        prtitNode(tree);
        printf("(");
        printTree(tree->child);
        printf(") ");
    }
    if (tree->brother != NULL) {
        printf(",");
        printTree(tree->brother);
    }
    return;
}

struct node* build_node0(NType t) {
    struct node* p;
    p = (struct node*)malloc(sizeof(struct node));
    p->type = t;
    p->child = NULL;
    p->brother = NULL;
    return p;
}

struct node* build_node1(NType t, Node* p1) {
    struct node* p;
    p = (struct node*)malloc(sizeof(struct node));
    p->type = t;
    p->child = p1;
    p->brother = NULL;
    return p;
}

struct node* build_node2(NType t, Node* p1, Node* p2) {
    struct node* p;
    p = (struct node*)malloc(sizeof(struct node));
    p->type = t;
    p->child = p1;
    if (p1 != NULL) {
        p1->brother = p2;
    }
    p->brother = NULL;
    return p;
}

struct node* build_node3(NType t, Node* p1, Node* p2, Node* p3) {
    struct node* p;
    p = (struct node*)malloc(sizeof(struct node));
    p->type = t;
    p->child = p1;
    if (p1 != NULL) {
        p1->brother = p2;
        if (p2 != NULL) {
            p2->brother = p3;
        }
    }
    p->brother = NULL;
    return p;
}

struct node* build_ident_node(char* c) {
    struct node* p;
    p = (struct node*)malloc(sizeof(struct node));
    p->type = IDENT_AST;
    p->variable = (char*)malloc(sizeof(c));
    strcpy(p->variable, c);
    p->ivalue  = 1;
    p->ivalue2  = 1;
    p->child = NULL;
    p->brother = NULL;
    return p;
}

struct node* build_num_node(int n) {
    struct node* p;
    p = (struct node*)malloc(sizeof(struct node));
    p->type    = NUM_AST;
    p->ivalue  = n;
    p->child   = NULL;
    p->brother = NULL;
    return p;
}

struct node* build_array_node(char* c, int n) {
    struct node* p;
    p = (struct node*)malloc(sizeof(struct node));
    p->type    = ArryEl_AST;
    p->variable = (char*)malloc(sizeof(c));
    strcpy(p->variable, c);
    p->ivalue  = n;
    p->child   = NULL;
    p->brother = NULL;
    return p;
}

struct node* build_array_node2(char* c, char *n) {
    struct node* p;
    p = (struct node*)malloc(sizeof(struct node));
    p->type    = ArryEl2_AST;
    p->variable = (char*)malloc(sizeof(c));
    strcpy(p->variable, c);
    p->elem = (char*)malloc(sizeof(n));
    strcpy(p->elem, n);
    p->ivalue  = 0;
    p->child   = NULL;
    p->brother = NULL;
    return p;
}

struct node* build_array_node2d(NType t, char* c, int n1, int n2, char* s1, char* s2) {
    struct node* p;
    p = (struct node*)malloc(sizeof(struct node));
    p->type    = t;
    p->variable = (char*)malloc(sizeof(c));
    strcpy(p->variable, c);
    if ((t == ArryE2_AST) || (t == ArryE2ni_AST)) {
        p->ivalue  = n1;
    } else {
        p->elem = (char*)malloc(sizeof(s1));
        strcpy(p->elem, s1);
    } 
    if ((t == ArryE2_AST) || (t == ArryE2in_AST)) {
        p->ivalue2  = n2;
    } else {
        p->elem2 = (char*)malloc(sizeof(s2));
        strcpy(p->elem2, s2);
    }
    p->child   = NULL;
    p->brother = NULL;
    return p;
}

void def_var(Node* p) {
    struct varlist* v, *p1;
    if (Varlist.name == NULL) {
        Varlist.name = p->variable;
        Varlist.offset = 0;
        if (p->type != ArryE2_AST) {
            Varlist.volume = p->ivalue*4;
        } else {
            Varlist.volume = p->ivalue*p->ivalue2*4;
        }
        Varlist.w1 = p->ivalue;
        Varlist.next = NULL;
        return;
    }
    p1 = &Varlist;
    v = (varlist*) malloc(sizeof(varlist));
    v->name = (char*)malloc(sizeof(p->variable));
    strcpy(v->name, p->variable);
    v->offset = Varlist.volume;
    v->w1     = p->ivalue;
    if (p->type != ArryE2_AST) {
        v->volume = p->ivalue*4;
    } else {
        v->volume = p->ivalue*p->ivalue2*4;
    }
    v->next = NULL;
    while(1) {
        if (p1->next == NULL) {
            break;
        }
        v->offset += p1->next->volume;
        p1 = p1->next;
    }
    p1->next = v;
    return;
}

void print_operator(Node *p) {
    switch (p->type)
    {
    case PLUS_AST:
        fprintf(stdout, "  add $t3, $t1, $t2\n");
        break;
    case MINUS_AST:
        fprintf(stdout, "  sub $t3, $t1, $t2\n");
        break;
    case MUL_AST:
        fprintf(stdout, "  mult $t1, $t2\n");
        fprintf(stdout, "  mflo $t3\n  nop\n");
        break;
    case DIV_AST:
        fprintf(stdout, "  div $t1, $t2\n");
        fprintf(stdout, "  mflo $t3\n  nop\n");
        break;
    case SUR_AST:
        fprintf(stdout, "  div $t1, $t2\n  mfhi $t3\n");
        break;
    default:
        break;
    }
    return;
}

void print_exp(Node* p, int *po) {
    struct varlist* v;
    struct varlist *p1 = &Varlist;
    int flg = 0, arrayflg = 0;
    v = &Varlist;
    switch (p->type) {
        case EXP_AST:
            if (p->child->brother != NULL) {
                *po += 4;
                print_exp(p->child, po);
                *po += 4;
                print_exp(p->child->brother->brother, po);
                *po-=8;
                fprintf(stdout, "  lw  $t1, %d($sp)\n", (*po)+4);
                fprintf(stdout, "  lw  $t2, %d($sp)\n  nop\n", (*po)+8);
                print_operator(p->child->brother);
                fprintf(stdout, "  sw  $t3, %d($sp)\n  nop # (delay slot)\n", (*po));
            } else {
                print_exp(p->child, po);
            }
            break;
        case TERM_AST:
            if (p->child->brother != NULL) {
                *po += 4;
                print_exp(p->child, po);
                *po += 4;
                print_exp(p->child->brother->brother, po);
                *po-=8;
                fprintf(stdout, "  lw  $t1, %d($sp)\n", (*po)+4);
                fprintf(stdout, "  lw  $t2, %d($sp)\n  nop\n", (*po)+8);
                print_operator(p->child->brother);
                fprintf(stdout, "  sw  $t3, %d($sp)\n  nop # (delay slot)\n", (*po));
            } else {
                print_exp(p->child, po);
            }
            break;
        case IDENT_AST:
            while(1) {
                if (strcmp(v->name, p->variable) == 0) {
                    fprintf(stdout, "  lw  $t1, %d($t0)  #load %s\n  nop\n", v->offset, v->name);
                    fprintf(stdout, "  sw  $t1, %d($sp)\n  nop # (delay slot)\n", *po);
                    break;
                }
                if (v->next == NULL) {
                    flg = 1;
                    fprintf(stderr, "error has ocurred:%s is not defined.\n", p->variable);
                    break;
                }
                v = v->next;
            }
            if (flg) {
                return;
            }
            break;
        case NUM_AST:
            fprintf(stdout, "  addi $t1, $zero, %d\n", p->ivalue);
            fprintf(stdout, "  sw  $t1, %d($sp)\n  nop\n", *po);
            break;
        case ArryEl_AST:
        case ArryEl2_AST:
            if (p->type == ArryEl2_AST) {
                while(1) {
                    if (strcmp(p1->name, p->elem) == 0) {
                        fprintf(stdout, "  lw  $v1, %d($t0)  # load %s\n  nop\n\n", p1->offset, p1->name);
                        break;
                    }
                    if (p1->next == NULL) {
                        fprintf(stderr, "error in store_ident:%s", p->variable);
                    }
                    p1 = p1->next;
                }
                p1 = &Varlist;
            } else {
                fprintf(stdout, "  addi $v1, $zero, %d\n", p->ivalue);
            }
            fprintf(stdout, "  sll $v1, $v1, 2\n");
            fprintf(stdout, "  add $v1, $v1, $t0\n");
            while(1) {
                if (strcmp(p1->name, p->variable) == 0) {
                    fprintf(stdout, "  lw  $t1, %d($v1)  # load %s\n  nop\n\n", p1->offset, p1->name);
                    fprintf(stdout, "  sw  $t1, %d($sp)\n  nop # (delay slot)\n", *po);
                    break;
                }
                if (p1->next == NULL) {
                    fprintf(stderr, "error in store_ident:%s", p->variable);
                }
                p1 = p1->next;
            }
            break;
        case ArryE2_AST:
            fprintf(stdout, "  addi $t1, $zero, %d\n", p->ivalue);
            fprintf(stdout, "  addi $t2, $zero, %d\n", p->ivalue2);
            arrayflg = 1;
            break;
        case ArryE2ii_AST:
            while(1) {
                if (strcmp(p1->name, p->elem) == 0) {
                    fprintf(stdout, "  lw  $t1, %d($t0)  # load %s\n  nop\n\n", p1->offset, p1->name);
                    break;
                }
                if (p1->next == NULL) {
                    fprintf(stderr, "error in store_ident:%s", p->variable);
                }
                p1 = p1->next;
            }
            p1 = &Varlist;
            while(1) {
                if (strcmp(p1->name, p->elem2) == 0) {
                    fprintf(stdout, "  lw  $t2, %d($t0)  # load %s\n  nop\n\n", p1->offset, p1->name);
                    break;
                }
                if (p1->next == NULL) {
                    fprintf(stderr, "error in store_ident:%s", p->variable);
                }
                p1 = p1->next;
            }
            arrayflg = 1;
            break;
        case ArryE2ni_AST:
            while(1) {
                if (strcmp(p1->name, p->elem2) == 0) {
                    fprintf(stdout, "  lw  $t1, %d($t0)  # load %s\n  nop\n\n", p1->offset, p1->name);
                    break;
                }
                if (p1->next == NULL) {
                    fprintf(stderr, "error in store_ident:%s", p->variable);
                }
                p1 = p1->next;
            }
            fprintf(stdout, "  addi $t2, $zero, %d\n", p->ivalue);
            arrayflg = 1;
            break;
        case ArryE2in_AST:
            while(1) {
                if (strcmp(p1->name, p->elem) == 0) {
                    fprintf(stdout, "  lw  $t1, %d($t0)  # load %s\n  nop\n\n", p1->offset, p1->name);
                    break;
                }
                if (p1->next == NULL) {
                    fprintf(stderr, "error in store_ident:%s", p->variable);
                }
                p1 = p1->next;
            }
            fprintf(stdout, "  addi $t2, $zero, %d\n", p->ivalue2);
            arrayflg = 1;
            break;
        
        default:
            fprintf(stderr, "error:%d", p->type);
        break;
    }
    if (arrayflg) {
        p1 = &Varlist;
        while(1) {
            if (strcmp(p1->name, p->variable) == 0) {
                fprintf(stdout, "  addi $t3, $zero, %d\n  nop\n", p1->w1);
                fprintf(stdout, "  mult $t1, $t3\n  nop\n");
                fprintf(stdout, "  mflo $v1\n  nop\n");
                fprintf(stdout, "  add $v1, $v1, $t2\n  nop\n");
                fprintf(stdout, "  sll $v1, $v1, 2\n");
                fprintf(stdout, "  add $v1, $v1, $t0\n  nop\n");
                fprintf(stdout, "  lw  $t1, %d($v1)  # load %s\n  nop\n\n", p1->offset, p1->name);
                fprintf(stdout, "  sw  $t1, %d($sp)\n  nop # (delay slot)\n", *po);
                break;
            }
            if (p1->next == NULL) {
                fprintf(stderr, "error in store_ident:%s", p->variable);
            }
            p1 = p1->next;
        }
    }
    return;
}

void make_word() {
    if (Varlist.name == NULL) {
        return;
    }
    struct varlist *p1 = &Varlist;
    fprintf(stdout, "    .data 0x%x\nRESULT:\n", DATA_SEG);
    while(p1->next != NULL) {
        if (p1->volume == 4) {
            fprintf(stdout, "  .word 0 # %s\n", p1->name);
        } else {
            fprintf(stdout, "  .space %d # %s\n", p1->volume, p1->name);
        }
        p1 = p1->next;
    }
    if (p1->volume == 4) {
        fprintf(stdout, "  .word 0 # %s\n", p1->name);
    } else {
        fprintf(stdout, "  .space %d # %s\n", p1->volume, p1->name);
    }
    return;
}

void store_ident(Node *p) {
    struct varlist *p1 = &Varlist;
    while(1) {
        if (strcmp(p1->name, p->variable) == 0) {
            fprintf(stdout, "  sw  $v0, %d($t0)  # store %s\n  nop\n\n", p1->offset, p1->name);
            break;
        }
        if (p1->next == NULL) {
            fprintf(stderr, "error in store_ident:%s", p->variable);
        }
        p1 = p1->next;
    }
    return;
}

void print_condition(Node *p) {
    int i = 0;
    print_exp(p->child, &i);
    fprintf(stdout, "  lw  $t6, %d($sp)\n", i);
    print_exp(p->child->brother->brother, &i);
    i = 0;
    fprintf(stdout, "  lw  $t7, %d($sp)\n  nop\n", i);
    switch (p->child->brother->type)
    {
    case EQ_AST:
        fprintf(stdout, "  beq $t6, $t7, $L%d\n  nop\n", L);
        fprintf(stdout, "  add $v0, $zero, $zero\n");
        fprintf(stdout, "  j $L%d\n  nop\n", L+1);
        fprintf(stdout, "$L%d:\n", L);
        fprintf(stdout, "  addi $v0, $zero, 1\n");
        fprintf(stdout, "$L%d:\n", L+1);
        L += 2;
        break;
    case LTE_AST:
        fprintf(stdout, "  slt $v0, $t6, $t7\n");
        fprintf(stdout, "  beq $t6, $t7, $L%d\n  nop\n", L);
        fprintf(stdout, "  add $v1, $zero, $zero\n");
        fprintf(stdout, "  j $L%d\n  nop\n", L+1);
        fprintf(stdout, "$L%d:\n", L);
        fprintf(stdout, "  addi $v1, $zero, 1\n");
        fprintf(stdout, "$L%d:\n", L+1);
        fprintf(stdout, "  or $v0, $v0, $v1\n");
        L += 2;
        break;
    case GTE_AST:
        fprintf(stdout, "  sgt $v0, $t6, $t7\n");
        fprintf(stdout, "  beq $t6, $t7, $L%d\n  nop\n", L);
        fprintf(stdout, "  add $v1, $zero, $zero\n");
        fprintf(stdout, "  j $L%d\n  nop\n", L+1);
        fprintf(stdout, "$L%d:\n", L);
        fprintf(stdout, "  addi $v1, $zero, 1\n");
        fprintf(stdout, "$L%d:\n", L+1);
        fprintf(stdout, "  or $v0, $v0, $v1\n");
        L += 2;
        break;
    case LT_AST:
        fprintf(stdout, "  slt $v0, $t6, $t7\n");
        break;
    case GT_AST:
        fprintf(stdout, "  sgt $v0, $t6, $t7\n");
        break;
    default:
        break;
    }
    return;
}

void print_while(Node *p) {
    int label = L;
    L += 2;
    fprintf(stdout, "$L%d:\n", label);
    print_condition(p);
    fprintf(stdout, "  beq $v0, $zero, $L%d\n  nop\n", label+1);
    codegen(p->brother);
    fprintf(stdout, "  j $L%d\n  nop\n", label);
    fprintf(stdout, "$L%d:\n", label+1);
    return;
}

void print_if(Node* p) {
    int label = L;
    fprintf(stdout, "#if\n");
    if (p->brother->brother == NULL) {
        L += 1;
        print_condition(p);
        fprintf(stdout, "  beq $v0, $zero, $L%d\n  nop\n", label);
        codegen(p->brother);
        fprintf(stdout, "$L%d:\n", label);
    } else {
        L += 2;
        print_condition(p);
        fprintf(stdout, "  beq $v0, $zero, $L%d\n  nop\n", label);
        codegen(p->brother);
        fprintf(stdout, "  j $L%d\n  nop\n", label+1);
        fprintf(stdout, "$L%d:\n", label);
        fprintf(stdout, "#else\n");
        codegen(p->brother->brother);
        fprintf(stdout, "$L%d:\n", label+1);
    }
    return;
}

void store_array(Node *p) {
    struct varlist *p1 = &Varlist;
    int i;
    if (p->type == ArryEl2_AST) {
        while(1) {
            if (strcmp(p1->name, p->elem) == 0) {
                fprintf(stdout, "  lw  $v1, %d($t0)  # load %s\n  nop\n\n", p1->offset, p1->name);
                break;
            }
            if (p1->next == NULL) {
                fprintf(stderr, "error in store_ident:%s", p->variable);
            }
            p1 = p1->next;
        }
    } else if (p->type == ArryE2_AST){
        fprintf(stdout, "  addi $t1, $zero, %d\n", p->ivalue);
        fprintf(stdout, "  addi $t2, $zero, %d\n", p->ivalue2);
    } else if (p->type == ArryE2in_AST){
        while(1) {
            if (strcmp(p1->name, p->elem) == 0) {
                fprintf(stdout, "  lw  $t1, %d($t0)  # load %s\n  nop\n\n", p1->offset, p1->name);
                break;
            }
            if (p1->next == NULL) {
                fprintf(stderr, "error in store_ident:%s", p->variable);
            }
            p1 = p1->next;
        }
        fprintf(stdout, "  addi $t2, $zero, %d\n", p->ivalue2);
    } else if (p->type == ArryE2ni_AST){
        while(1) {
            if (strcmp(p1->name, p->elem2) == 0) {
                fprintf(stdout, "  lw  $t1, %d($t0)  # load %s\n  nop\n\n", p1->offset, p1->name);
                break;
            }
            if (p1->next == NULL) {
                fprintf(stderr, "error in store_ident:%s", p->variable);
            }
            p1 = p1->next;
        }
        fprintf(stdout, "  addi $t2, $zero, %d\n", p->ivalue);
    } else if (p->type == ArryE2ii_AST){
        while(1) {
            if (strcmp(p1->name, p->elem) == 0) {
                fprintf(stdout, "  lw  $t1, %d($t0)  # load %s\n  nop\n\n", p1->offset, p1->name);
                break;
            }
            if (p1->next == NULL) {
                fprintf(stderr, "error in store_ident:%s", p->variable);
            }
            p1 = p1->next;
        }
        p1 = &Varlist;
        while(1) {
            if (strcmp(p1->name, p->elem2) == 0) {
                fprintf(stdout, "  lw  $t2, %d($t0)  # load %s\n  nop\n\n", p1->offset, p1->name);
                break;
            }
            if (p1->next == NULL) {
                fprintf(stderr, "error in store_ident:%s", p->variable);
            }
            p1 = p1->next;
        }
    } else {
        fprintf(stdout, "  addi $v1, $zero, %d\n", p->ivalue);
    }

    p1 = &Varlist;
    fprintf(stdout, "  sll $v1, $v1, 2\n");
    fprintf(stdout, "  add $v1, $v1, $t0\n");
    while(1) {
        if (strcmp(p1->name, p->variable) == 0) {
            switch (p->type) {
            case ArryE2_AST:
            case ArryE2ii_AST:
            case ArryE2in_AST:
            case ArryE2ni_AST:
                fprintf(stdout, "  addi $t3, $zero, %d  # store array\n  nop\n", p1->w1);
                fprintf(stdout, "  mult $t1, $t3\n  nop\n");
                fprintf(stdout, "  mflo $v1\n  nop\n");
                fprintf(stdout, "  add $v1, $v1, $t2\n  nop\n");
                fprintf(stdout, "  sll $v1, $v1, 2\n");
                fprintf(stdout, "  add $v1, $v1, $t0\n  nop\n");
            default:
                fprintf(stdout, "  sw  $v0, %d($v1)  # store %s\n  nop\n\n", p1->offset, p1->name);
                break;
            }
            break;
        }
        if (p1->next == NULL) {
            fprintf(stderr, "error in store_ident:%s", p->variable);
        }
        p1 = p1->next;
    }
    return;
}

int codegen(Node* n) {
    int i = 0;
    switch (n->type) {
        case PROG_AST:
            fprintf(stdout, "  .text\nmain:\n  la $t0, RESULT\n");
            fprintf(stdout, "  add $s0, $ra, $zero\n");
            codegen(n->child);
            if (n->child->brother != NULL) {
               codegen(n->child->brother);
            }
            fprintf(stdout, "$EXIT:\n");
            fprintf(stdout, "  add $ra, $s0, $zero\n");
            //fprintf(stdout, "  jr $ra\n  nop\n");
            fprintf(stdout, "  li $v0, 10\n  syscall\n  nop\n");
            make_word();
            return 0;
            break;

        case DECLA_AST:
        case STATEMENTS_AST:
        case STATEMENT_AST:
            codegen(n->child);
            if (n->child->brother != NULL) {
               codegen(n->child->brother);
            }
            break;

        case DECL_AST:
            def_var(n->child);
            break;

        case ASSIGN_AST:
            print_exp(n->child->brother, &i);
            fprintf(stdout, "  lw  $v0, %d($sp)\n  nop\n", i);
            if (n->child->type == IDENT_AST) {
                store_ident(n->child);
            } else {
                store_array(n->child);
            }
            break;

        case WHILE_AST:
            print_while(n->child);
            break;
        case IF_AST:
            print_if(n->child);
            break;
        case EXP_AST:
            fprintf(stdout, "exp\n");
            print_exp(n, &i);
            fprintf(stdout, "  lw  $v0, %d($sp)\nEND\n", i);
            break;
        case COND_AST:
            fprintf(stdout, "condition\n");
            break;
        default:
            if (n->child != NULL) {
                codegen(n->child);
            }
            break;
    }
    
    return 0;
}
