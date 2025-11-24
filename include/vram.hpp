#pragma once
#include <vector>
#include <cstddef>
#include <map>
#include <cstdint>

class Vram {
public:
    using Word = uint32_t;

    enum ErrorType { NO, STUCK_AT_0, STUCK_AT_1 };

    explicit Vram(size_t len_) : len(len_), _data(len_, 0) {}

    Word read(size_t i) const;
    void write(size_t i, Word word);
    void set_error(size_t addr, size_t bit, ErrorType err) { _errors[addr].push_back(err); }

    size_t len;

private:
    std::vector<Word> _data;
    std::map<size_t, std::vector<ErrorType>> _errors;
};
