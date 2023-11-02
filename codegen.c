#include "9cc.h"

int labelseq = 0;
char *funcname;
void gen(Node *node);
char *argreg[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
void gen_addr(Node *node) {
    switch (node->kind) {
    case ND_VAR:
        Var *var = node->var;
        if (var->is_local) {
            printf("  lea rax, [rbp-%d]\n", var->offset);
            printf("  push rax\n");
        } else {
            printf("  push offset %s\n", var->name);
        }
        return;
    case ND_DEREF:
        gen(node->lhs);
        return;
    }
    error_tok(node->tok, "not an lvalue");
}
void gen_lval(Node *node) {
    if (node->ty->kind == TY_ARRAY) {
        error_tok(node->tok, "not an lvalue");
    }
    gen_addr(node);
}
void load() {
    printf("  pop rax\n");
    printf("  mov rax, [rax]\n");
    printf("  push rax\n");
}
void store() {
    printf("  pop rdi\n");
    printf("  pop rax\n");
    printf("  mov [rax], rdi\n");
    printf("  push rdi\n");
}
void gen(Node *node) {
    switch (node->kind) {
    case ND_NUM:
        printf("  push %d\n", node->val);
        return;
    case ND_EXPR_STMT:
        gen(node->lhs);
        printf("  add rsp, 8\n");
        return;
    case ND_NULL:
        return;
    case ND_VAR:
        gen_addr(node);
        if (node->ty->kind != TY_ARRAY)
            load();
        return;
    case ND_ASSIGN:
        gen_lval(node->lhs);
        gen(node->rhs);
        store();
        return;
    case ND_ADDR:
        gen_addr(node->lhs);
        return;
    case ND_DEREF:
        gen(node->lhs);
        if (node->ty->kind != TY_ARRAY)
            load();
        return;
    case ND_IF: {
        int seq = labelseq++;
        if (node->els) {
            gen(node->cond);
            printf("  pop rax\n");
            printf("  cmp rax, 0\n");
            printf("  je  .Lelse%d\n", seq);
            gen(node->then);
            printf("  jmp .Lend%d\n", seq);
            printf(".Lelse%d:\n", seq);
            gen(node->els);
            printf(".Lend%d:\n", seq);
        } else {
            gen(node->cond);
            printf("  pop rax\n");
            printf("  cmp rax, 0\n");
            printf("  je  .Lend%d\n", seq);
            gen(node->then);
            printf(".Lend%d:\n", seq);
        }
        return;
    }
    case ND_WHILE: {
        int seq = labelseq++;
        printf(".Lbegin%d:\n", seq);
        gen(node->cond);
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        printf("  je  .Lend%d\n", seq);
        gen(node->then);
        printf("  jmp .Lbegin%d\n", seq);
        printf(".Lend%d:\n", seq);
        return;
    }
    case ND_FOR: {
        int seq = labelseq++;
        if (node->init) {
            gen(node->init);
        }
        printf(".Lbegin%d:\n", seq);
        if (node->cond) {
            gen(node->cond);
            printf("  pop rax\n");
            printf("  cmp rax, 0\n");
            printf("  je  .Lend%d\n", seq);
        }
        gen(node->then);
        if (node->inc) {
            gen(node->inc);
        }
        printf("  jmp .Lbegin%d\n", seq);
        printf(".Lend%d:\n", seq);
        return;
    }
    case ND_BLOCK:
        for (Node *n = node->body; n; n = n->next) {
            gen(n);
        }
        return;
    case ND_FUNCALL: {
        int nargs = 0;
        for (Node *arg = node->args; arg; arg = arg->next) {
            gen(arg);
            nargs++;
        }
        for (int i = nargs - 1; i >= 0; i--) {
            printf("  pop %s\n", argreg[i]);
        }
        int seq = labelseq++;
        printf("  mov rax, rsp\n");
        printf("  and rax, 15\n");
        printf("  jnz .Lcall%d\n", seq);
        printf("  mov rax, 0\n");
        printf("  call %s\n", node->funcname);
        printf("  jmp .Lend%d\n", seq);
        printf(".Lcall%d:\n", seq);
        printf("  sub rsp, 8\n");
        printf("  mov rax, 0\n");
        printf("  call %s\n", node->funcname);
        printf("  add rsp, 8\n");
        printf(".Lend%d:\n", seq);
        printf("  push rax\n");
        return;
    }
    case ND_RETURN:
        gen(node->lhs);
        printf("  pop rax\n");
        printf("  jmp .Lreturn.%s\n", funcname);
        return;
    }
    gen(node->lhs);
    gen(node->rhs);
    printf("  pop rdi\n");
    printf("  pop rax\n");
    switch (node->kind) {
    case ND_ADD:
        if (node->ty->base)
            printf("  imul rdi, %d\n", size_of(node->ty->base));
        printf("  add rax, rdi\n");
        break;
    case ND_SUB:
        if (node->ty->base)
            printf("  imul rdi, %d\n", size_of(node->ty->base));
        printf("  sub rax, rdi\n");
        break;
    case ND_MUL:
        printf("  imul rax, rdi\n");
        break;
    case ND_DIV:
        printf("  cqo\n");
        printf("  idiv rdi\n");
        break;
    case ND_EQ:
        printf("  cmp rax, rdi\n");
        printf("  sete al\n");
        printf("  movzb rax, al\n");
        break;
    case ND_NE:
        printf("  cmp rax, rdi\n");
        printf("  setne al\n");
        printf("  movzb rax, al\n");
        break;
    case ND_LT:
        printf("  cmp rax, rdi\n");
        printf("  setl al\n");
        printf("  movzb rax, al\n");
        break;
    case ND_LE:
        printf("  cmp rax, rdi\n");
        printf("  setle al\n");
        printf("  movzb rax, al\n");
        break;
    }
    printf("  push rax\n");
}
void emit_data(Program *prog) {
    printf(".data\n");
    for (VarList *vl = prog->globals; vl; vl = vl->next) {
        Var *var = vl->var;
        printf("%s:\n", var->name);
        printf("  .zero %d\n", size_of(var->ty));
    }
}
void emit_text(Program *prog) {
    printf(".text\n");
    for (Function *fn = prog->fns; fn; fn = fn->next) {
        printf(".global %s\n", fn->name);
        printf("%s:\n", fn->name);
        funcname = fn->name;
        // prologue
        printf("  push rbp\n");
        printf("  mov rbp, rsp\n");
        printf("  sub rsp, %d\n", fn->stack_size);
        int i = 0;
        for (VarList *vl = fn->params; vl; vl = vl->next) {
            Var *var = vl->var;
            printf("  mov [rbp-%d], %s\n", var->offset, argreg[i++]);
        }
        for (Node *node = fn->node; node; node = node->next) {
            gen(node);
        }
        // epilogue
        printf(".Lreturn.%s:\n", funcname);
        printf("  mov rsp, rbp\n");
        printf("  pop rbp\n");
        printf("  ret\n");
    }
}

void codegen(Program *prog) {
    printf(".intel_syntax noprefix\n");
    emit_data(prog);
    emit_text(prog);
}