
#include <stdio.h>
#include "EONEval.h"


EON::Eval g_eval;

int main(int argc, char **argv)
{
    uint8_t eon[] = {0x01, 0x01, 0x00, 0x02, 0x00, 0x06, 0x02, 0x00, 0x03, 0x02, 0x17, 0x00, 0x00, 0x02, 0x01, 0x00, 0x00, 0x01, 0x02, 0x00, };
    g_eval.load(eon, sizeof(eon));
    g_eval.turn();
    return 0;
}
