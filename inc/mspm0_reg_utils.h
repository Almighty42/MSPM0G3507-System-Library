#ifndef MSPM0_REG_UTILS_H
#define MSPM0_REG_UTILS_H
#include <stdint.h>


// Bit - level (1 bit)
static inline void write_reg_bit(volatile uint32_t *reg, uint32_t pos, uint32_t value){
    
    *reg = (*reg & ~(1U << pos)) | ((value & 1U) << pos);
}

static inline void clear_reg_bit(volatile uint32_t *reg, uint32_t pos){
    
    uint32_t clear_shift = 1U << pos;
    *reg &= ~clear_shift;

}

static inline uint32_t read_reg_bit(const volatile uint32_t *reg, uint32_t pos){
    
    uint32_t read_bit = 1U << pos;
    
    read_bit = *reg & read_bit;
    
    read_bit >>= pos;

    return read_bit;
}

// Field-level (N bits)

static inline void write_reg_field(volatile uint32_t *reg, uint32_t pos, uint32_t width, uint32_t value){
    
    uint32_t mask = ((1U << width) - 1U) << pos;

    *reg = (*reg &~ mask) | ((value << pos) & mask);

}

static inline void clear_reg_field(volatile uint32_t *reg, uint32_t pos, uint32_t width){
    
    uint32_t mask = ((1U << width) - 1U) << pos;
    *reg &= ~mask;

}

static inline uint32_t read_reg_field(const volatile uint32_t *reg, uint32_t pos, uint32_t width){

    uint32_t mask = ((1U << width) - 1U) << pos;

    uint32_t read_bits = *reg & mask;

    read_bits >>= pos;
    
    return read_bits;
}
#endif // MSPM0_REG_UTILS_H