#include "button.h"
#include <JC_Button.h>

Button btn(39, true, 10);

void initButton() {
    btn.begin();
}

void handleButton() {
    btn.read();
}