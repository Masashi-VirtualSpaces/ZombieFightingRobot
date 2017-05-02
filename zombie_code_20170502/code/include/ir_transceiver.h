#ifndef IR_TRANSCEIVER_H
#define IR_TRANSCEIVER_H

int init_ir(int baud, int num_data_bits, int set_parity, int parity_type);
void stopIr(void);
char getRcvLetter(void);
void setXmtLetter(char c);
void setTransmitModeOn(void);
void setTransmitModeOff(void);

#endif