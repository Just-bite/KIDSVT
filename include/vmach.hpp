#pragma once
#include "vram.hpp"
#include <stack>
#include <string>
#include <fstream>
#include <functional>
#include <map>
#include <sstream>
#include <optional>
#include <vector>

class Vmach {
public:
    using Word = Vram::Word;

    Vmach(std::string const &program_str, Vram &ram);

    void step();
    void halt() { _halted = true; }
    bool is_halted() const { return _halted; } 
    void dump_stack() const;
    void run();
private:
    Vram &_ram;
    std::stack<Word> stack;
    std::stack<Word> _hidden_stack;

    struct LoopFrame { size_t pos; Word counter; };
    std::stack<LoopFrame> _loop_stack;

    size_t i = 0;
    bool _halted = false;
    std::istringstream program;
    std::map<std::string, std::function<void()>> _ops;

    std::string next_op();
    std::optional<Word> sane_stoull(std::string const &str);
    Word sane_pop(std::stack<Word> &stack);
    void goto_matching_op(const std::string& target_op, int direction);
    void op_const(Word constant);

    void op_loop();
    void op_endloop();
    void op_asc();
    void op_desc();
    void op_then();
    void op_endthen();
    void op_assert();
    void op_read();
    void op_write();
    void op_swaponstack();
    void op_last();
    void op_cur();
    void op_equal();
    void op_not();
    void op_xor();
    void op_and();
    void op_or();
    void op_add();
    void op_neg();
    void op_read_i();
    void op_write_i();
    void op_lshift();
    
    void register_ops();
};
