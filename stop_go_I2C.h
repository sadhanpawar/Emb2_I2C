/*
 * stop_go.h
 *
 *  Created on: Jan 27, 2023
 *      Author: sadhan
 */

#ifndef STOP_GO_I2C_H_
#define STOP_GO_I2C_H_


extern void configI2c0Mcp2308(void);
extern void mcpGpioPortBIntHandler(void);
extern void talkToSpiDev(uint8_t opcode, uint8_t reg, uint32_t *data, bool flag);
extern void writeI2c0RegData(void);

#endif /* STOP_GO_I2C_H_ */
