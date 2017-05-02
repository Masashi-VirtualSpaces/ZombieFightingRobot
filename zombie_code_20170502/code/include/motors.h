#ifndef MOTORS_H
#define MOTORS_H

#define DEFAULT_HI_DUTY     100
#define DEFAULT_LO_DUTY      60
#define LEFT_MOTOR            0
#define RIGHT_MOTOR           1

// void* motorDrive(void* num);
// void startDrive(int motor_num, int direction, int cruiseSpeed, float duration);
int init_motors(void);
void stopMotor(int motor_num);
void stopAllMotors(void);
void start_ISRs(void);
int getCount(int motor_num);
int getDirection(int motor_num);
void setDutyCycle(int motor_num, int dutyCycle);
void setDirection(int motor_num, int direction);

#endif
