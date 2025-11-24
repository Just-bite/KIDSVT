#include "vram.hpp"
#include <stdexcept>

Vram::Word Vram::read(size_t i) const {
    Word word = _data[i];

    try {
        auto const errs = _errors.at(i);
        for (unsigned bit = 0; bit < errs.size(); ++bit) {
            switch (errs[bit]) {
                case NO: break;
                case STUCK_AT_0: word &= ~(1 << bit); break;
                case STUCK_AT_1: word |= (1 << bit); break;
            }
        }
    } catch (std::out_of_range const &) {}

    return word;
}

void Vram::write(size_t i, Word word) { _data[i] = word; }
