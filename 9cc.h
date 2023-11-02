#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct Type Type;

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
void error_tok(Token *tok, char *fmt, ...);
Token *consume(char *op);
char *strndup(char *p, int len);
Token *consume_ident();
Token *peek(char *s);
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
    char *name;
    Type *ty;
    int offset;
    bool is_local;
};
typedef struct VarList VarList;
struct VarList {
    VarList *next;
    Var *var;
};
typedef enum {
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_ASSIGN,
    ND_EXPR_STMT,
    ND_VAR,
    ND_NULL,
    ND_EQ,    //==
    ND_NE,    //!=
    ND_LT,    //<
    ND_LE,    //<=
    ND_ADDR,  // unary &
    ND_DEREF, // unary *
    ND_RETURN,
    ND_IF,
    ND_WHILE,
    ND_FOR,
    ND_BLOCK,
    ND_FUNCALL,
    ND_NUM,
    ND_SIZEOF,
} NodeKind;
typedef struct Node Node;
struct Node {
    NodeKind kind;
    Node *next;
    Type *ty;
    Token *tok;
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
    VarList *params;
    Node *node;
    VarList *locals;
    int stack_size;
};
typedef struct {
    VarList *globals;
    Function *fns;
} Program;

Program *program();
typedef enum { TY_INT, TY_PTR, TY_ARRAY } TypeKind;
struct Type {
    TypeKind kind;
    Type *base;
    int array_size;
};
Type *int_type();
Type *pointer_to(Type *base);
Type *array_of(Type *base, int size);
int size_of(Type *ty);
void add_type(Program *prog);
void codegen(Program *prog);