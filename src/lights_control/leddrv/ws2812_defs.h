// w2812_defs.h
//
// contains material from Charles Lohr, but changed by me.

#ifndef __WS2812_I2S_DEFS_H__
#define __WS2812_I2S_DEFS_H__

#include <stdint.h>



//transmit rule: (((24bit color + 1bit end) * LED_TOTAL) + 12bits guide) = 7537
#define LED_TOTAL               301             //Led number = 300
#define NUM_RGB_BYTES           903             //color number = 900
#define NUM_I2S_PIXEL_BYTES1    4040            //8byte zero 3756 + 8 = 3764   
#define NUM_I2S_PIXEL_WORDS1    1010             //NUM_I2S_PIXEL_BYTES1 / 4
#define NUM_I2S_PIXEL_BYTES2    4080            //8byte zero 3756 + 8 = 3764   
#define NUM_I2S_PIXEL_WORDS2    1020             //NUM_I2S_PIXEL_BYTES2 / 41


// -----------------------------------------------------
// I2S timing parameters 
#define WS_I2S_BCK          (28)               // >=18
#define WS_I2S_DIV          (13)               // >= 13


//-----------------------------------------------------

#ifndef i2c_bbpll
#define i2c_bbpll                               0x67
#define i2c_bbpll_en_audio_clock_out            4
#define i2c_bbpll_en_audio_clock_out_msb        7
#define i2c_bbpll_en_audio_clock_out_lsb        7
#define i2c_bbpll_hostid                        4
#endif

// -----------------------------------------------------

#define i2c_writeReg_Mask(block, host_id, reg_add, Msb, Lsb, indata)  rom_i2c_writeReg_Mask(block, host_id, reg_add, Msb, Lsb, indata)
#define i2c_readReg_Mask(block, host_id, reg_add, Msb, Lsb)  rom_i2c_readReg_Mask(block, host_id, reg_add, Msb, Lsb)
#define i2c_writeReg_Mask_def(block, reg_add, indata) i2c_writeReg_Mask(block, block##_hostid,  reg_add,  reg_add##_msb,  reg_add##_lsb,  indata)
#define i2c_readReg_Mask_def(block, reg_add) i2c_readReg_Mask(block, block##_hostid,  reg_add,  reg_add##_msb,  reg_add##_lsb)

// -----------------------------------------------------

#ifndef ETS_SLC_INUM
#define ETS_SLC_INUM       1
#endif

// -----------------------------------------------------

// from i2s_reg.h

#define DR_REG_I2S_BASE (0x60000e00)

#define I2STXFIFO  (DR_REG_I2S_BASE + 0x0000)
#define I2SRXFIFO  (DR_REG_I2S_BASE + 0x0004)
#define I2SCONF  (DR_REG_I2S_BASE + 0x0008)
#define I2S_BCK_DIV_NUM 0x0000003F
#define I2S_BCK_DIV_NUM_S 22
#define I2S_CLKM_DIV_NUM 0x0000003F
#define I2S_CLKM_DIV_NUM_S 16
#define I2S_BITS_MOD 0x0000000F
#define I2S_BITS_MOD_S 12
#define I2S_RECE_MSB_SHIFT (BIT(11))
#define I2S_TRANS_MSB_SHIFT (BIT(10))
#define I2S_I2S_RX_START (BIT(9))
#define I2S_I2S_TX_START (BIT(8))
#define I2S_MSB_RIGHT (BIT(7))
#define I2S_RIGHT_FIRST (BIT(6))
#define I2S_RECE_SLAVE_MOD (BIT(5))
#define I2S_TRANS_SLAVE_MOD (BIT(4))
#define I2S_I2S_RX_FIFO_RESET (BIT(3))
#define I2S_I2S_TX_FIFO_RESET (BIT(2))
#define I2S_I2S_RX_RESET (BIT(1))
#define I2S_I2S_TX_RESET (BIT(0))
#define I2S_I2S_RESET_MASK 0xf

#define I2SINT_RAW (DR_REG_I2S_BASE + 0x000c)
#define I2S_I2S_TX_REMPTY_INT_RAW (BIT(5))
#define I2S_I2S_TX_WFULL_INT_RAW (BIT(4))
#define I2S_I2S_RX_REMPTY_INT_RAW (BIT(3))
#define I2S_I2S_RX_WFULL_INT_RAW (BIT(2))
#define I2S_I2S_TX_PUT_DATA_INT_RAW (BIT(1))
#define I2S_I2S_RX_TAKE_DATA_INT_RAW (BIT(0))

#define I2SINT_ST (DR_REG_I2S_BASE + 0x0010)
#define I2S_I2S_TX_REMPTY_INT_ST (BIT(5))
#define I2S_I2S_TX_WFULL_INT_ST (BIT(4))
#define I2S_I2S_RX_REMPTY_INT_ST (BIT(3))
#define I2S_I2S_RX_WFULL_INT_ST (BIT(2))
#define I2S_I2S_TX_PUT_DATA_INT_ST (BIT(1))
#define I2S_I2S_RX_TAKE_DATA_INT_ST (BIT(0))

#define I2SINT_ENA (DR_REG_I2S_BASE + 0x0014)
#define I2S_I2S_TX_REMPTY_INT_ENA (BIT(5))
#define I2S_I2S_TX_WFULL_INT_ENA (BIT(4))
#define I2S_I2S_RX_REMPTY_INT_ENA (BIT(3))
#define I2S_I2S_RX_WFULL_INT_ENA (BIT(2))
#define I2S_I2S_TX_PUT_DATA_INT_ENA (BIT(1))
#define I2S_I2S_RX_TAKE_DATA_INT_ENA (BIT(0))

#define I2SINT_CLR (DR_REG_I2S_BASE + 0x0018)
#define I2S_I2S_TX_REMPTY_INT_CLR (BIT(5))
#define I2S_I2S_TX_WFULL_INT_CLR (BIT(4))
#define I2S_I2S_RX_REMPTY_INT_CLR (BIT(3))
#define I2S_I2S_RX_WFULL_INT_CLR (BIT(2))
#define I2S_I2S_PUT_DATA_INT_CLR (BIT(1))
#define I2S_I2S_TAKE_DATA_INT_CLR (BIT(0))

#define I2STIMING (DR_REG_I2S_BASE + 0x001c)
#define I2S_TRANS_BCK_IN_INV (BIT(22))
#define I2S_RECE_DSYNC_SW (BIT(21))
#define I2S_TRANS_DSYNC_SW (BIT(20))
#define I2S_RECE_BCK_OUT_DELAY 0x00000003
#define I2S_RECE_BCK_OUT_DELAY_S 18
#define I2S_RECE_WS_OUT_DELAY 0x00000003
#define I2S_RECE_WS_OUT_DELAY_S 16
#define I2S_TRANS_SD_OUT_DELAY 0x00000003
#define I2S_TRANS_SD_OUT_DELAY_S 14
#define I2S_TRANS_WS_OUT_DELAY 0x00000003
#define I2S_TRANS_WS_OUT_DELAY_S 12
#define I2S_TRANS_BCK_OUT_DELAY 0x00000003
#define I2S_TRANS_BCK_OUT_DELAY_S 10
#define I2S_RECE_SD_IN_DELAY 0x00000003
#define I2S_RECE_SD_IN_DELAY_S 8
#define I2S_RECE_WS_IN_DELAY 0x00000003
#define I2S_RECE_WS_IN_DELAY_S 6
#define I2S_RECE_BCK_IN_DELAY 0x00000003
#define I2S_RECE_BCK_IN_DELAY_S 4
#define I2S_TRANS_WS_IN_DELAY 0x00000003
#define I2S_TRANS_WS_IN_DELAY_S 2
#define I2S_TRANS_BCK_IN_DELAY 0x00000003
#define I2S_TRANS_BCK_IN_DELAY_S 0

#define I2S_FIFO_CONF (DR_REG_I2S_BASE + 0x0020)
#define I2S_I2S_RX_FIFO_MOD 0x00000007
#define I2S_I2S_RX_FIFO_MOD_S 16
#define I2S_I2S_TX_FIFO_MOD 0x00000007
#define I2S_I2S_TX_FIFO_MOD_S 13
#define I2S_I2S_DSCR_EN (BIT(12))
#define I2S_I2S_TX_DATA_NUM 0x0000003F
#define I2S_I2S_TX_DATA_NUM_S 6
#define I2S_I2S_RX_DATA_NUM 0x0000003F
#define I2S_I2S_RX_DATA_NUM_S 0

#define I2SRXEOF_NUM (DR_REG_I2S_BASE + 0x0024)
#define I2S_I2S_RX_EOF_NUM 0xFFFFFFFF
#define I2S_I2S_RX_EOF_NUM_S 0

#define I2SCONF_SIGLE_DATA (DR_REG_I2S_BASE + 0x0028)
#define I2S_I2S_SIGLE_DATA 0xFFFFFFFF
#define I2S_I2S_SIGLE_DATA_S 0

#define I2SCONF_CHAN (DR_REG_I2S_BASE + 0x002c)
#define I2S_RX_CHAN_MOD 0x00000003
#define I2S_RX_CHAN_MOD_S 3
#define I2S_TX_CHAN_MOD 0x00000007
#define I2S_TX_CHAN_MOD_S 0

// -----------------------------------------------------

#endif

// end of file
