#include "9cc.h"
char *user_input;
Token *token;
void error(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}
void verror_at(char *loc, char *fmt, va_list ap) {
    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, " ");
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}
void error_at(char *loc, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    verror_at(loc, fmt, ap);
}
void error_tok(Token *tok, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    if (tok) {
        verror_at(tok->str, fmt, ap);
    }
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}
char *strndup(char *p, int len) {
    char *buf = malloc(len + 1);
    strncpy(buf, p, len);
    buf[len] = '\0';
    return buf;
}
// Returns true if the current token matches a given string.
Token *peek(char *s) {
    if (token->kind != TK_RESERVED || strlen(s) != token->len || memcmp(token->str, s, token->len))
        return NULL;
    return token;
}
// Consumes the current token if it matches a given string.
Token *consume(char *op) {
    if (token->kind != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len))
        return NULL;
    Token *t = token;
    token = token->next;
    return t;
}
Token *consume_ident() {
    if (token->kind != TK_IDENT) {
        return NULL;
    }
    Token *t = token;
    token = token->next;
    return t;
}
// Ensure that the current token is a given string
void expect(char *s) {
    if (!peek(s))
        error_tok(token, "expected \"%s\"", s);
    token = token->next;
}
int expect_number() {
    if (token->kind != TK_NUM)
        error_tok(token, "expected a number");
    int val = token->val;
    token = token->next;
    return val;
}
char *expect_ident() {
    if (token->kind != TK_IDENT) {
        error_tok(token, "expected an identifier");
    }
    char *s = strndup(token->str, token->len);
    token = token->next;
    return s;
}
bool at_eof() {
    return token->kind == TK_EOF;
}
Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    tok->len = len;
    cur->next = tok;
    return tok;
}
bool startswith(char *p, char *q) {
    return memcmp(p, q, strlen(q)) == 0;
}
bool is_alpha(char c) {
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z' || c == '_');
}
int is_alnum(char c) {
    return ('0' <= c && c <= '9') || is_alpha(c);
}
char *starts_with_reserved(char *p) {
    static char *kw[] = {"return", "if", "else", "while", "for", "int", "sizeof"};
    for (int i = 0; i < sizeof(kw) / sizeof(*kw); i++) {
        int len = strlen(kw[i]);
        if (startswith(p, kw[i]) && !is_alnum(p[len])) {
            return kw[i];
        }
    }
    static char *ops[] = {"==", "!=", "<=", ">="};
    for (int i = 0; i < sizeof(ops) / sizeof(*ops); i++) {
        if (startswith(p, ops[i])) {
            return ops[i];
        }
    }
    return NULL;
}

Token *tokenize() {
    char *p = user_input;
    Token head;
    head.next = NULL;
    Token *cur = &head;
    while (*p) {
        if (isspace(*p)) {
            p++;
            continue;
        }
        char *kw = starts_with_reserved(p);
        if (kw) {
            int len = strlen(kw);
            cur = new_token(TK_RESERVED, cur, p, len);
            p += len;
            continue;
        }
        if (strchr("+-*/()<>;={},&[]", *p)) {
            cur = new_token(TK_RESERVED, cur, p++, 1);
            continue;
        }
        if (is_alpha(*p)) {
            char *q = p++;
            while (is_alnum(*p))
                p++;
            cur = new_token(TK_IDENT, cur, q, p - q);
            // cur->len = 1;
            continue;
        }
        if (isdigit(*p)) {
            char *q = p;
            cur = new_token(TK_NUM, cur, p, 0);
            cur->val = strtol(p, &p, 10);
            cur->len = p - q;
            continue;
        }
        error_at(p, "expected a number");
    }
    new_token(TK_EOF, cur, p, 0);
    return head.next;
}
