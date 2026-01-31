#include "repl.hpp"
#include "../compiler/lexer.hpp"
#include "../compiler/parser.hpp"
#include "../compiler/generator/generator.hpp"
#include "../compiler/generator/emit.hpp"
#include "../runtime/vm.hpp"
#include "../compiler/ast.hpp"
#include <chrono>

int run_repl() {
    std::string expr;
    lmx::Lexer l(expr);
    lmx::Generator gener;
    lmx::runtime::VirtualCore core;
    core.set_program(&gener.ops);

    while (true) {

        std::cout << std::flush << ">>>";
        if (!std::getline(std::cin, expr)) break;
        else if (expr == ":lastret") std::cout << core.look_register(0) << std::endl;
        else if (expr == ":exit") break;
        else if (expr == ":op") gener.print_ops();
        else {
            std::vector<lmx::Token> tks = l.tokenize(expr);
            lmx::Parser parser(tks);
            auto node = parser.parse();
            if (!node || parser.error()) continue;
            auto op = gener.gen(node);
            if (lmx::node_has_error) continue;
            gener.ops.emplace_back(lmx::runtime::Opcode::HALT);

            const auto start = std::chrono::high_resolution_clock::now();
            core.run();
            const auto end = std::chrono::high_resolution_clock::now();

            const auto result = op != -1 ? core.look_register(op) : core.look_register(0);

            std::cout << result << std::endl;
            std::cout << "time " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start) << std::endl;

            if (op != -1) gener.regs.free(op);
            if (gener.ops.back().op == lmx::runtime::Opcode::HALT) gener.ops.pop_back();
        }
    }
    return 0;
}