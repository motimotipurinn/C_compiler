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
Token *consume_ident();
void expect(char *op);
int expect_number();
bool at_eof();
extern char *user_input;
extern Token *token;
Token *new_token(TokenKind kind, Token *cur, char *str, int len);
bool startwith(char *p, char *q);
Token *tokenize();
typedef enum {
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_ASSIGN,
    ND_EXPR_STMT,
    ND_LVAR,
    ND_EQ, //==
    ND_NE, //!=
    ND_LT, //<
    ND_LE, //<=
    ND_RETURN,
    ND_NUM,
} NodeKind;
typedef struct Node Node;
struct Node {
    NodeKind kind;
    Node *next;
    Node *lhs;
    Node *rhs;
    char name;
    int val;
};
Node *program();
void codegen(Node *node);