#include "vmach.hpp"
#include <iostream>
#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <sstream>

Vmach::Vmach(std::string const &program_str, Vram &ram) : _ram(ram), program(program_str) {
    register_ops();
}

void Vmach::register_ops() {
    _ops["loop"] = [this](){ op_loop(); };
    _ops["endloop"] = [this](){ op_endloop(); };
    _ops["asc"] = [this](){ op_asc(); };
    _ops["desc"] = [this](){ op_desc(); };
    _ops["then"] = [this](){ op_then(); };
    _ops["endthen"] = [this](){ op_endthen(); };
    _ops["assert!"] = [this](){ op_assert(); };
    _ops["read"] = [this](){ op_read(); };
    _ops["write"] = [this](){ op_write(); };
    _ops["swaponstack"] = [this](){ op_swaponstack(); };
    _ops["last"] = [this](){ op_last(); };
    _ops["cur"] = [this](){ op_cur(); };
    _ops["equal?"] = [this](){ op_equal(); };
    _ops["not"] = [this](){ op_not(); };
    _ops["xor"] = [this](){ op_xor(); };
    _ops["and"] = [this](){ op_and(); };
    _ops["or"] = [this](){ op_or(); };
    _ops["add"] = [this](){ op_add(); };
    _ops["neg"] = [this](){ op_neg(); };
    _ops["readi"] = [this](){ op_read_i(); };
    _ops["writei"] = [this](){ op_write_i(); };
    _ops["lshift"] = [this](){ op_lshift(); };
}

void Vmach::step() {
    if (_halted) return;

    std::string op = next_op();
    if (op.empty()) {
        _halted = true;
        return;
    }

    // Обработка констант и шорткатов
    if (op == "0x0000" || op == "0") {
        op_const(0x0000);
    } else if (op == "0xFFFF" || op == "-1") {
        op_const(0xFFFF);
    } else {
        auto c = sane_stoull(op);
        if (c) op_const(c.value());
        else if (_ops.count(op)) _ops[op]();
        else if (!op.empty()) std::cout << "Unknown op: " << op << std::endl;
    }

    dump_stack();
}

void Vmach::dump_stack() const {
    std::stack<Word> tmp = stack;
    std::vector<Word> elems;
    while (!tmp.empty()) { 
        elems.push_back(tmp.top()); 
        tmp.pop(); 
    }
    std::reverse(elems.begin(), elems.end());
    std::cout << "[STACK i=" << i << "] ";
    for (auto w : elems) std::cout << std::hex << w << " ";
    std::cout << std::endl;
}

std::string Vmach::next_op() {
    std::string op;
    if (!(program >> op)) return "";
    std::transform(op.begin(), op.end(), op.begin(), ::tolower);
    return op;
}

std::optional<Vmach::Word> Vmach::sane_stoull(std::string const &str) {
    if (str.empty()) return {};
    
    // Проверяем разные системы счисления
    try {
        if (str.find("0x") == 0 || str.find("0X") == 0) {
            // Шестнадцатеричная
            return std::stoul(str, nullptr, 16);
        } else if (str.find("0b") == 0 || str.find("0B") == 0) {
            // Двоичная
            return std::stoul(str.substr(2), nullptr, 2);
        } else if (str.find("0") == 0 && str.length() > 1) {
            // Восьмеричная
            return std::stoul(str, nullptr, 8);
        } else if (isdigit(str[0]) || str[0] == '-') {
            // Десятичная
            return std::stoul(str, nullptr, 10);
        }
    } catch (...) { 
        return {}; 
    }
    return {};
}

Vmach::Word Vmach::sane_pop(std::stack<Word> &s) {
    if (s.empty()) {
        std::cout << "Stack underflow!" << std::endl;
        halt();
        return 0;
    }
    Word t = s.top(); 
    s.pop(); 
    return t;
}

void Vmach::op_const(Word constant) { 
    stack.push(constant); 
}

void Vmach::op_loop() {
    if (stack.empty()) {
        std::cout << "Stack underflow in LOOP!" << std::endl;
        halt();
        return;
    }
    Word count = sane_pop(stack);
    
    // Если счетчик = 0, сразу переходим к endloop
    if (count == 0) {
        goto_matching_op("endloop", 1);
    } else {
        _loop_stack.push({static_cast<size_t>(program.tellg()), count});
        std::cout << "LOOP started with counter: " << count << std::endl;
    }
}

void Vmach::op_endloop() {
    if (_loop_stack.empty()) {
        std::cout << "ENDLOOP without LOOP!" << std::endl;
        return;
    }
    auto &frame = _loop_stack.top();
    
    // ЗАЩИТА ОТ UNDERFLOW
    if (frame.counter == 0) {
        _loop_stack.pop();
        std::cout << "LOOP finished (zero counter)" << std::endl;
        return;
    }
    
    std::cout << "LOOP iteration, counter: " << frame.counter << std::endl;
    
    if (--frame.counter == 0) {
        _loop_stack.pop();
        std::cout << "LOOP finished" << std::endl;
    } else {
        program.seekg(frame.pos);
        std::cout << "LOOP continuing, jumping back" << std::endl;
    }
}

void Vmach::op_asc() { 
    i = (i + 1) % _ram.len; 
    std::cout << "ASC: i = " << i << std::endl;
}

void Vmach::op_desc() { 
    i = (i - 1 + _ram.len) % _ram.len; 
    std::cout << "DESC: i = " << i << std::endl;
}

void Vmach::op_then() { 
    if (stack.empty()) {
        std::cout << "Stack underflow in THEN!" << std::endl;
        halt();
        return;
    }
    Word cond = sane_pop(stack); 
    std::cout << "THEN condition: " << cond << std::endl;
    if (cond == 0) {
        goto_matching_op("endthen", 1);
    }
}

void Vmach::op_endthen() {
    std::cout << "ENDTHEN reached" << std::endl;
}

void Vmach::op_assert() { 
    if (stack.empty()) {
        std::cout << "Stack underflow in ASSERT!" << std::endl;
        halt();
        return;
    }
    Word arg = sane_pop(stack); 
    std::cout << "ASSERT: " << arg << std::endl;
    if (!arg) halt(); 
}

void Vmach::op_read() { 
    if (stack.empty()) {
        std::cout << "Stack underflow in READ!" << std::endl;
        halt();
        return;
    }
    Word addr = sane_pop(stack); 
    Word val = _ram.read(addr);
    stack.push(val);
    std::cout << "READ from addr " << addr << ": " << val << std::endl;
}

void Vmach::op_write() { 
    if (stack.size() < 2) {
        std::cout << "Stack underflow in WRITE!" << std::endl;
        halt();
        return;
    }
    Word addr = sane_pop(stack); 
    Word val = sane_pop(stack); 
    _ram.write(addr, val);
    std::cout << "WRITE to addr " << addr << ": " << val << std::endl;
}

void Vmach::op_swaponstack() { 
    if (stack.size() < 2) {
        std::cout << "Stack underflow in SWAPONSTACK!" << std::endl;
        halt();
        return;
    }
    Word a = sane_pop(stack); 
    Word b = sane_pop(stack); 
    stack.push(a); 
    stack.push(b);
    std::cout << "SWAP: " << a << " <-> " << b << std::endl;
}

void Vmach::op_last() { 
    if (stack.empty()) {
        std::cout << "Stack underflow in LAST!" << std::endl;
        halt();
        return;
    }
    Word a = stack.top(); 
    stack.push(a); 
    stack.push(a);
    std::cout << "LAST: duplicated " << a << " twice" << std::endl;
}

void Vmach::op_cur() { 
    if (stack.empty()) {
        std::cout << "Stack underflow in CUR!" << std::endl;
        halt();
        return;
    }
    Word a = stack.top(); 
    stack.push(a);
    std::cout << "CUR: duplicated " << a << std::endl;
}

void Vmach::op_equal() { 
    if (stack.size() < 2) {
        std::cout << "Stack underflow in EQUAL?!" << std::endl;
        halt();
        return;
    }
    Word a = sane_pop(stack); 
    Word b = sane_pop(stack); 
    stack.push(a == b ? 0xFFFF : 0x0000);
    std::cout << "EQUAL? " << a << " == " << b << " = " << (a == b) << std::endl;
}

void Vmach::op_not() { 
    if (stack.empty()) {
        std::cout << "Stack underflow in NOT!" << std::endl;
        halt();
        return;
    }
    Word a = sane_pop(stack); 
    stack.push(~a);
    std::cout << "NOT " << a << " = " << (~a) << std::endl;
}

void Vmach::op_xor() { 
    if (stack.size() < 2) {
        std::cout << "Stack underflow in XOR!" << std::endl;
        halt();
        return;
    }
    Word a = sane_pop(stack); 
    Word b = sane_pop(stack); 
    stack.push(a ^ b);
    std::cout << "XOR " << a << " ^ " << b << " = " << (a ^ b) << std::endl;
}

void Vmach::op_and() { 
    if (stack.size() < 2) {
        std::cout << "Stack underflow in AND!" << std::endl;
        halt();
        return;
    }
    Word a = sane_pop(stack); 
    Word b = sane_pop(stack); 
    stack.push(a & b);
    std::cout << "AND " << a << " & " << b << " = " << (a & b) << std::endl;
}

void Vmach::op_or() { 
    if (stack.size() < 2) {
        std::cout << "Stack underflow in OR!" << std::endl;
        halt();
        return;
    }
    Word a = sane_pop(stack); 
    Word b = sane_pop(stack); 
    stack.push(a | b);
    std::cout << "OR " << a << " | " << b << " = " << (a | b) << std::endl;
}

void Vmach::op_add() { 
    if (stack.size() < 2) {
        std::cout << "Stack underflow in ADD!" << std::endl;
        halt();
        return;
    }
    Word a = sane_pop(stack); 
    Word b = sane_pop(stack); 
    stack.push(a + b);
    std::cout << "ADD " << a << " + " << b << " = " << (a + b) << std::endl;
}

void Vmach::op_neg() { 
    if (stack.empty()) {
        std::cout << "Stack underflow in NEG!" << std::endl;
        halt();
        return;
    }
    Word a = sane_pop(stack); 
    stack.push(-a);
    std::cout << "NEG " << a << " = " << (-a) << std::endl;
}

void Vmach::op_read_i() { 
    stack.push(static_cast<Word>(i)); 
    std::cout << "READI: " << i << std::endl;
}

void Vmach::op_write_i() { 
    if (stack.empty()) {
        std::cout << "Stack underflow in WRITEI!" << std::endl;
        halt();
        return;
    }
    i = sane_pop(stack) % _ram.len; 
    std::cout << "WRITEI: i = " << i << std::endl;
}

void Vmach::goto_matching_op(const std::string& target_op, int direction) {
    std::string cmd;
    int depth = 0;
    
    while (program >> cmd) {
        std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);
        
        if (cmd == target_op && depth == 0) {
            std::cout << "Found matching " << target_op << std::endl;
            break;
        }

        if (cmd == "then") depth++;
        else if (cmd == "endthen") depth--;
    }
}

void Vmach::run() {
    while (!_halted && program) {
        step();
    }
    if (_halted) {
        std::cout << "Program halted" << std::endl;
    }
}

void Vmach::op_lshift() { 
    if (stack.size() < 2) {
        std::cout << "Stack underflow in LSHIFT!" << std::endl;
        halt();
        return;
    }
    Word shift = sane_pop(stack); 
    Word value = sane_pop(stack); 
    stack.push(value << shift);
    std::cout << "LSHIFT " << value << " << " << shift << " = " << (value << shift) << std::endl;
}