#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//
// tokenize.c
//

typedef enum{
    TK_RESERVED,//sign
    TK_IDENT,   //sikibetusi
    TK_NUM,     //token of number
    TK_EOF,     //token which is end of input
} TokenKind;

typedef struct Token Token;

//form of token
struct Token{
    TokenKind kind;//kind(find enum)
    Token *next;   //next token
    int val;       //this is value,if kind is TK_NUM
    char *str;     //string of token
    int len;       //len(correspond <=,>=,==,!=)
};

void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
// void error_tok(Token *tok, char *fmt, ...);
// bool equal(Token tok, char *op);
// Token *skip(Token *tok, char *op);
Token *tokenize(char *p);
// Token *tokenize();

//
// parse.c
//

//kind of node on tree
typedef enum{
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_NUM,
    ND_EQ,
    ND_NE,
    ND_LT,
    ND_LE,
}NodeKind;

typedef struct Node Node;

// tree Node
struct Node{
    NodeKind kind;
    Node *lhs;
    Node *rhs;
    int val;
};

Node *parse(Token *tok);

//
// codegen.c
//

void codegen(Node *node);