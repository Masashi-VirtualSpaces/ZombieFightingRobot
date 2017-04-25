#ifndef WEB_H
#define WEB_H

char* getMyIP (char* interface);
void broadcast_msg(char *mess, char *broadcastIP);
const char* getUDPmessage(void);

#endif