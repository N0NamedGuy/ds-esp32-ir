#define IR_IN_PIN 32

#define IR_OUT_PIN 12
#define IR_OUT_PIN_EXT 26

void initIr();
void handleIr();

void recvIr();
void sendIr();

void sendOn();