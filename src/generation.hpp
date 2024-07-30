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
                if (!gen->m_vars.contains(term_ident->ident.value.value())) {
                    std::cerr << "Undeclared identifier: " << term_ident->ident.value.value() << std::endl;
                    exit(EXIT_FAILURE);
                }
                const auto& var = gen->m_vars.at(term_ident->ident.value.value());
                std::stringstream offset;
                offset << "QWORD [rsp + " << (gen->m_stack_size-var.stack_loc-1)*8 << "]";
                gen->push(offset.str());
            }
        };

        TermVisitor visitor{.gen = this};
        std::visit(visitor, term->var);
    }

    void gen_expr(const NodeExpr *expr) {
        struct ExprVisitor {
            Generator *gen;
            void operator()(const NodeTerm *term) const {
                gen->gen_term(term);
            }
            void operator()(const NodeBinExpr *bin_expr) const {
                // assuming var is addition
                gen->gen_expr(bin_expr->var->lhs);
                gen->gen_expr(bin_expr->var->rhs);
                gen->pop("rbx");
                gen->pop("rax");
                gen->m_output << "  add rax, rbx\n";
                gen->push("rax");
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
                if(gen->m_vars.contains(stmt_let->ident.value.value())) {
                    std::cerr << "Identifier already used: " << stmt_let->ident.value.value() << std::endl;
                    exit(EXIT_FAILURE);
                }
                gen->m_vars.insert({stmt_let->ident.value.value(), Var {.stack_loc = gen->m_stack_size}});
                gen->gen_expr(stmt_let->expr);
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

    struct Var {
        size_t stack_loc;
    };

    const NodeProg m_prog;
    std::stringstream m_output;
    size_t m_stack_size;
    std::unordered_map<std::string, Var> m_vars {};
};