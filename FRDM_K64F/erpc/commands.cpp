/*
 * Copyright (c) 2015, Wes Calvert
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the
 * names of its contributors may be used to endorse or promote products
 * derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "mbed.h"
#include "commands.h"

extern uint32_t rpc_args[MAX_ARGS];
extern uint32_t num_args;

AnalogOut aout(DAC0_OUT);
PwmOut pwm1(PTC10);

RPCMethod command_table[] = {
    { add,  "add",  2, 2, "Add two numbers",                        "int,int" },
    { mult, "mult", 2, 2, "Multiply two numbers",                   "int,int" },
    { dac,  "dac",  1, 1, "Set DAC0 output",                        "int16" },
    { pwm,  "pwm",  2, 2, "Set PTC10 period and pulsewidth (uS)",   "int,int" }
};

uint32_t add(void) {
    uint32_t total = 0;
    for(uint32_t i=0; i<num_args; i++) {
        #ifdef DEBUG
        printf("adding: %d\n", rpc_args[i]);
        #endif
        total += rpc_args[i];
    }
    return total;
}

uint32_t mult(void) {
    uint32_t total = rpc_args[0];
    for(uint32_t i=1; i<num_args; i++) {
        total *= rpc_args[i];
    }
    return total;
}

uint32_t dac(void) {
    if(rpc_args[0] > 65535) {
        return 0;
    }
    aout.write_u16((uint16_t)rpc_args[0]);
    return 1;
}

uint32_t pwm(void) {
    if(rpc_args[1] > rpc_args[0]) {
        return 0;
    }
    pwm1.period_us(rpc_args[0]);
    pwm1.pulsewidth_us(rpc_args[1]);
    return 1;
}
