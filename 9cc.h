#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    TK_RESERVED,
    TK_IDENT,
    TK_NUM,
    TK_EOF,
} TokenKind;
typedef struct Token Token;
struct Token {
    TokenKind kind;
    Token *next;
    int val;
    char *str;
    int len;
};
void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
bool consume(char *op);
char *strndup(char *p, int len);
Token *consume_ident();
void expect(char *op);
int expect_number();
char *expect_ident();
bool at_eof();
extern char *user_input;
extern Token *token;
Token *new_token(TokenKind kind, Token *cur, char *str, int len);
bool startwith(char *p, char *q);
Token *tokenize();
typedef struct Var Var;
struct Var {
    Var *next;
    char *name;
    int offset;
};
typedef enum {
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_ASSIGN,
    ND_EXPR_STMT,
    ND_VAR,
    ND_EQ, //==
    ND_NE, //!=
    ND_LT, //<
    ND_LE, //<=
    ND_RETURN,
    ND_IF,
    ND_WHILE,
    ND_FOR,
    ND_BLOCK,
    ND_FUNCALL,
    ND_NUM,
} NodeKind;
typedef struct Node Node;
struct Node {
    NodeKind kind;
    Node *next;
    Node *lhs;
    Node *rhs;
    Node *cond;
    Node *then;
    Node *els;
    Node *init;
    Node *inc;
    Node *body;
    char *funcname;
    Node *args;
    Var *var;
    int val;
};

typedef struct Function Function;
struct Function {
    Function *next;
    char *name;
    Node *node;
    Var *locals;
    int stack_size;
};
Function *program();
void codegen(Function *prog);