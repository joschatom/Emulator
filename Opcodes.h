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

// Jump to Sub Rotine
#define OPCODE_JSR				HEX(20)
#define OPCODE_RTS				HEX(60)

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

