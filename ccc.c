#include<ctype.h>
#include<stdarg.h>
#include<stdbool.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

//kind of token
typedef enum{
    TK_RESERVED,//sign
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
};

//finding token
Token *token;

//column num of input
char *user_input;

//inform error position.//function is order to call error. 
void error_at(char *loc, char *fmt, ...){
    va_list ap;
    va_start(ap, fmt);

    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, "");// pos spaces
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// next token judge
bool consume(char op){
    if(token->kind != TK_RESERVED || token->str[0] != op){
        return false;
    }
    token = token->next;
    return true;
}

// next token judge, error
void expect(char op){
    if(token->kind != TK_RESERVED || token->str[0] != op){
        error_at(token->str, "it is not '%c'", op);
    }
    token = token->next;
}

// next token is NUM,
int expect_num(){
    if(token->kind != TK_NUM){
        error_at(token->str, "it is not NUM");
    }
    int val = token->val;
    token = token->next;
    return val;
}

//formula is end
bool at_eof(){
    return token->kind == TK_EOF;
}

//create new token at cur next
Token *new_token(TokenKind kind, Token *cur, char *str){
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    cur->next = tok;
    return tok;
}

//tokenize string.and retrun head.next(head is empty)
Token *tokenize(){
    char *p = user_input;
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while (*p){
        //skip space
        if(isspace(*p)){
            p++;
            continue;
        }

        if(strchr("+-*/()", *p)){
            cur = new_token(TK_RESERVED, cur, p++);
            continue;
        }

        if(isdigit(*p)){
            cur = new_token(TK_NUM, cur, p);
            cur->val = strtol(p, &p, 10);
            continue;
        }
        error_at(p, "can't tokenize");
    }
    new_token(TK_EOF, cur, p);//add eof at the end of tokenize
    return head.next;
}

//kind of node on tree
typedef enum{
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_NUM,
}NodeKind;

typedef struct Node Node;

// tree Node
struct Node{
    NodeKind kind;
    Node *lhs;
    Node *rhs;
    int val;
};

Node *new_node(NodeKind kind, Node *lhs, Node *rhs){
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *new_node_num(int val){
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_NUM;
    node->val = val;
    return node;
}

Node *expr();
Node *mul();
Node *unary();
Node *primary();

// expr
Node *expr(){
    Node *node = mul();

    for(;;){
        if(consume('+')){
            node = new_node(ND_ADD, node, mul());
        }else if(consume('-')){
            node = new_node(ND_SUB, node, mul());
        }else{
            return node;
        }
    }
}

//mul
Node *mul(){
    Node *node = unary();

    for(;;){
        if(consume('*')){
            node = new_node(ND_MUL, node, unary());
        }else if(consume('/')){
            node = new_node(ND_DIV, node, unary());
        }else{
            return node;
        }
    }
}

Node *unary(){
    if(consume('+')){
        return primary();
    }
    if(consume('-')){
        return new_node(ND_SUB, new_node_num(0), primary());
    }
    return primary();
}

//primary function
Node *primary(){
    // 
    if(consume('(')){
        Node *node = expr();
        expect(')');
        return node;
    }

    //
    return new_node_num(expect_num());
}

//saibunki wotukatta stack machine to redista machine
void gen(Node *node){
    if(node->kind == ND_NUM){
        printf("    push %d\n", node->val);
        return;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("    pop rdi\n");
    printf("    pop rax\n");

    switch(node->kind){
    case ND_ADD:
        printf("    add rax, rdi\n");
        break;
    case ND_SUB:
        printf("    sub rax, rdi\n");
        break;
    case ND_MUL:
        printf("    imul rax, rdi\n");
        break;
    case ND_DIV:
        printf("    cqo\n");
        printf("    idiv rdi\n");
        break;
    }

    printf("    push rax\n");
}

int main(int argc, char **argv){
    if(argc != 2){
        fprintf(stderr, "引数の個数が正しくありません\n");
        return 1;
    }

    // tokenize
    user_input = argv[1];
    token = tokenize();
    Node *node = expr();

    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");

    // code generate
    gen(node);

    // top stack
    printf("    pop rax\n");
    printf("    ret\n");
    return 0;

    // printf("    mov rax, %d\n", expect_num());

    // while(!at_eof()){
    //     if(consume('+')){
    //         printf("    add rax, %d\n", expect_num());
    //         continue;
    //     }

    //     expect('-');
    //     printf("    sub rax, %d\n", expect_num());
    //     continue;

    //     return 1;
    // }

    // printf("    ret\n");
    // return 0;
}