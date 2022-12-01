#pragma once

#define OPCODE(ins) OPCODE_##ins
#define HEX(_) 0x##_

#define OPCODE_HOSTCALL			HEX(42)

// LDA
#define OPCODE_LDA_IM			HEX(A9)
#define OPCODE_LDA_ZP			HEX(A5)
#define OPCODE_LDA_ZPX			HEX(B5)
#define OPCODE_LDA_ABS			HEX(AD)
#define OPCODE_LDA_ABSX			HEX(BD)
#define OPCODE_LDA_ABSY			HEX(B9)
#define OPCODE_LDA_INDX			HEX(A1)
#define OPCODE_LDA_INDY			HEX(B1)
// LDX
#define OPCODE_LDX_IM			HEX(A2)
#define OPCODE_LDX_ZP			HEX(A6)
#define OPCODE_LDX_ZPY 			HEX(B6)
#define OPCODE_LDX_ABS 			HEX(AE)
#define OPCODE_LDX_ABSY			HEX(BE)
// LDY
#define OPCODE_LDY_IM			HEX(A0)
#define OPCODE_LDY_ZP 			HEX(A4)
#define OPCODE_LDY_ZPX			HEX(B4)
#define OPCODE_LDY_ABS			HEX(AC)
#define OPCODE_LDY_ABSX			HEX(BC)

// Jumps
#define OPCODE_JSR				HEX(20)
#define OPCODE_RTS				HEX(60)
#define OPCODE_JMP_ABS			HEX(4C)
#define OPCODE_JMP_IND			HEX(6C)

// Other OPCODES
#define OPCODE_NOP				HEX(EA)

// STA
#define OPCODE_STA_ZP			HEX(85)
#define OPCODE_STA_ZPX			HEX(95)
#define OPCODE_STA_ABS			HEX(8D)
#define OPCODE_STA_ABSX			HEX(9D)
#define OPCODE_STA_ABSY			HEX(99)
#define OPCODE_STA_INDX			HEX(81)
#define OPCODE_STA_INDY			HEX(91)


// STX
#define OPCODE_STX_ZP			HEX(86)
#define OPCODE_STX_ZPY			HEX(96)
#define OPCODE_STX_ABS			HEX(8E)

// STY
#define OPCODE_STY_ZP			HEX(84)
#define OPCODE_STY_ZPX			HEX(94)
#define OPCODE_STY_ABS			HEX(8C)

// Stack
#define OPCODE_TSX				HEX(BA)
#define OPCODE_TXS				HEX(9A)
#define OPCODE_PHA				HEX(48)
#define OPCODE_PLA				HEX(68)
#define OPCODE_PHP				HEX(08)
#define OPCODE_PLP				HEX(28)


// Compare

#define OPCODE_CMP				HEX(C9)
#define OPCODE_CMP_ZP			HEX(C5)
#define OPCODE_CMP_ZPX			HEX(D5)
#define OPCODE_CMP_ABS			HEX(CD)
#define OPCODE_CMP_ABSX			HEX(DD)
#define OPCODE_CMP_ABSY 		HEX(D9)
#define OPCODE_CMP_INDX 		HEX(C1)
#define OPCODE_CMP_INDY 		HEX(D1)

#define OPCODE_CPX				HEX(E0)
#define OPCODE_CPY				HEX(C0)
#define OPCODE_CPX_ZP			HEX(E4)
#define OPCODE_CPY_ZP			HEX(C4)
#define OPCODE_CPX_ABS			HEX(EC)
#define OPCODE_CPY_ABS			HEX(CC)

// Locical  Ops

//AND
#define OPCODE_AND_IM			HEX(29)
#define OPCODE_AND_ZP			HEX(25)
#define OPCODE_AND_ZPX			HEX(35)
#define OPCODE_AND_ABS			HEX(2D)
#define OPCODE_AND_ABSX			HEX(3D)
#define OPCODE_AND_ABSY			HEX(39)
#define OPCODE_AND_INDY			HEX(31)
#define OPCODE_AND_INDX			HEX(21)

// OR
#define OPCODE_ORA_IM			HEX(09)
#define OPCODE_ORA_ZP			HEX(05)
#define OPCODE_ORA_ZPX			HEX(15)
#define OPCODE_ORA_ABS			HEX(0D)
#define OPCODE_ORA_ABSX			HEX(1D)
#define OPCODE_ORA_ABSY			HEX(19)
#define OPCODE_ORA_INDX			HEX(01)
#define OPCODE_ORA_INDY			HEX(11)

// EOR
#define OPCODE_EOR_IM			HEX(49)
#define OPCODE_EOR_ZP			HEX(45)
#define OPCODE_EOR_ZPX			HEX(55)
#define OPCODE_EOR_ABS			HEX(4D)
#define OPCODE_EOR_ABSX			HEX(5D)
#define OPCODE_EOR_ABSY			HEX(59)
#define OPCODE_EOR_INDX			HEX(41)
#define OPCODE_EOR_INDY			HEX(51)

// Flags
#define OPCODE_CLC				HEX(18)
#define OPCODE_SEC				HEX(38)
#define OPCODE_CLD				HEX(D8)
#define OPCODE_SED				HEX(F8)
#define OPCODE_CLI				HEX(58)
#define OPCODE_SEI				HEX(78)
#define OPCODE_CLV				HEX(B8)

// Increments, Decrements
#define OPCODE_INX				HEX(E8)
#define OPCODE_INY				HEX(C8)
#define OPCODE_DEY				HEX(88)
#define OPCODE_DEX				HEX(CA)


