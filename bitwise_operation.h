#ifndef BITWISE_OPERATION
#define BITWISE_OPERATION

#define SET_BIT(Register, Bit) Register |= (1 << Bit)
#define CLEAR_BIT(Register, Bit) Register &= ~(1 << Bit)
#define GET_BIT(Register, Bit) (Register & (1 << Bit)) >> Bit
#define TOGGLE_BIT(Register, Bit) Register ^= (1 << Bit)

#endif