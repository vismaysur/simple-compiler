#pragma once

#include <assert.h>

#include "parser.hpp"

class Generator {
public:
    inline Generator(NodeProg prog)
        : m_prog(std::move(prog))
    {
    }

    void gen_term (const NodeTerm *term) {
        struct TermVisitor {
            Generator *gen;
            void operator() (const NodeTermIntLit *term_int_lit) const {
                gen->m_output << "  mov rax, " << term_int_lit->int_lit.value.value() << "\n";
                gen->push("rax");
            }
            void operator() (const NodeTermIdent *term_ident) const {
                auto it = std::find_if(gen->m_vars.begin(), gen->m_vars.end(),
                    [&](const Var &var) { return var.name == term_ident->ident.value.value();});
                if (it == gen->m_vars.end()) {
                    std::cerr << "Undeclared identifier: " << term_ident->ident.value.value() << std::endl;
                    exit(EXIT_FAILURE);
                }
                std::stringstream offset;
                offset << "QWORD [rsp + " << (gen->m_stack_size-(*it).stack_loc-1)*8 << "]";
                gen->push(offset.str());
            }
            void operator() (const NodeTermParen *term_paren) const {
                gen->gen_expr(term_paren->expr);
            }
        };

        TermVisitor visitor{.gen = this};
        std::visit(visitor, term->var);
    }

    void gen_bin_expr(const NodeBinExpr *bin_expr) {
        struct BinExprVisitor {
            Generator* gen;
            void operator()(const NodeBinExprAdd *bin_expr_add) const {
                gen->gen_expr(bin_expr_add->lhs);
                gen->gen_expr(bin_expr_add->rhs);
                gen->pop("rbx");
                gen->pop("rax");
                gen->m_output << "  add rax, rbx\n";
                gen->push("rax");
            }
            void operator()(const NodeBinExprSub *bin_expr_sub) const {
                gen->gen_expr(bin_expr_sub->lhs);
                gen->gen_expr(bin_expr_sub->rhs);
                gen->pop("rbx");
                gen->pop("rax");
                gen->m_output << "  sub rax, rbx\n";
                gen->push("rax");
            }
            void operator()(const NodeBinExprMulti *bin_expr_multi) const {
                gen->gen_expr(bin_expr_multi->lhs);
                gen->gen_expr(bin_expr_multi->rhs);
                gen->pop("rbx");
                gen->pop("rax");
                gen->m_output << "  mul rbx\n";
                gen->push("rax");
            }
            void operator()(const NodeBinExprDiv *bin_expr_div) const {
                gen->gen_expr(bin_expr_div->lhs);
                gen->gen_expr(bin_expr_div->rhs);
                gen->pop("rbx");
                gen->pop("rax");
                gen->m_output << "  div rbx\n";
                gen->push("rax");
            }
        };

        BinExprVisitor visitor {.gen = this};
        std::visit(visitor, bin_expr->var);
    }

    void gen_expr(const NodeExpr *expr) {
        struct ExprVisitor {
            Generator *gen;
            void operator()(const NodeTerm *term) const {
                gen->gen_term(term);
            }
            void operator()(const NodeBinExpr *bin_expr) const {
                gen->gen_bin_expr(bin_expr);
            }
        };

        ExprVisitor visitor{.gen = this};
        std::visit(visitor, expr->var);
    }

    void gen_stmt(const NodeStmt *stmt) {
        struct StmtVisitor {
            Generator *gen;
            void operator()(const NodeStmtExit *stmt_exit) const
            {
                gen->gen_expr(stmt_exit->expr);
                gen->m_output << "  mov rax, 0x2000001\n";
                gen->pop("rdi");
                gen->m_output << "  syscall\n";
            }

            void operator()(const NodeStmtLet *stmt_let) const
            {
                auto it = std::find_if(gen->m_vars.begin(), gen->m_vars.end(),
                    [&](const Var &var) { return var.name == stmt_let->ident.value.value();});

                if (it != gen->m_vars.end()) {
                    std::cerr << "Identifier already used: " << stmt_let->ident.value.value() << std::endl;
                    exit(EXIT_FAILURE);
                }

                gen->m_vars.push_back(Var {.name = stmt_let->ident.value.value(), .stack_loc = gen->m_stack_size});
                gen->gen_expr(stmt_let->expr);
            }

            void operator()(const NodeStmtScope *scope) const {
                gen->begin_scope();
                for (const NodeStmt* stmt: scope->statements) {
                    gen->gen_stmt(stmt);
                }
                gen->end_scope();
            }
        };

        StmtVisitor visitor{.gen = this};
        std::visit(visitor, stmt->var);
    }

    [[nodiscard]] std::string gen_prog() {
        m_output << "global _main\nsection .text\n\n_main:\n";

        for (const NodeStmt *stmt: m_prog.statements) {
            gen_stmt(stmt);
        }

        m_output << "  mov rax, 0x2000001\n";
        m_output << "  mov rdi, 0\n";
        m_output << "  syscall";

        return m_output.str();
    }

private:

    void push(const std::string& reg) {
        m_output << "  push " << reg << "\n";
        m_stack_size++;
    }

    void pop(const std::string& reg) {
        m_output << "  pop " << reg << "\n";
        m_stack_size--;
    }

    void begin_scope() {
        m_scopes.push_back(m_vars.size());
    }

    void end_scope() {
        int cnt = 0;
        while (m_vars.size() > m_scopes.back()) {
            m_vars.pop_back();
            cnt++;
        }
        m_output << "  add rsp, " << cnt * 8 << "\n";
        m_stack_size -= cnt;
        m_scopes.pop_back();
    }

    struct Var {
        std::string name;
        size_t stack_loc;
    };

    const NodeProg m_prog;
    std::stringstream m_output;
    size_t m_stack_size;
    std::vector<Var> m_vars;
    std::vector<size_t> m_scopes;
};