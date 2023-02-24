/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "platform.h"
#include "xil_printf.h"
#include "helloworld.h"
#include "xtime_l.h"
#include "xil_cache.h"


#define TIMER_LOAD_VALUE 0xFFFFFFFF


int main(){

	init_platform();
	Xil_DCacheEnable();
	Xil_ICacheEnable();
    uint32_t i;
    XTime time;
    unsigned int cycles, cycles1;

    printf("Addition:\r\n");
    for(i = 0; i < 0xff; i++){
        printf("(0xCB + %x) mod 0x11b: %x\r\n", i, bff_add(0xcb, i, 0x11b));
    }
    //printf("result:%x\n\r", bff_mult(a,b,f));
    printf("Multiplication:\r\n");
    for(i = 0; i < 0xff; i++){
        printf("(0xCB * %x) mod 0x11b: %x\r\n", i, bff_mult(0xcb, i, 0x11b));
    }
    printf("Divition:\r\n");
    for(i = 1; i < 0xff; i++){
        printf("(0xCB / %x): %x\r\n", i, bff_div(0xcb, i));
    }

    printf("Generators for 256 in 0x11b:\r\n");
    get_generators(256, 0x11b);

    printf("Extended Euclidian:\r\n");
    uint32_t alpha;
    uint32_t beta;
    uint32_t gcd;
    XTime_SetTime(0);
    XTime_GetTime(&time);
    cycles = time;

    for(i = 1; i <= 0xff; i++){
    	gcd = extended_euclidean(0x11b, i, 0x11b, &alpha, &beta);
    	if(gcd == 1){
    		//printf("%x^-1: %x in 0x11b\r\n", i, beta);
    	}
    	else{
    		//printf("%x^-1: DNE in 0x11b\r\n", i);
    	}
    }
    XTime_GetTime(&time);
    cycles1 = time;
    printf("Cache Enabled, No Optimizations: Inverse Time(cycles):0x%x\r\n", cycles1-cycles);

    Xil_DCacheDisable();
    Xil_ICacheDisable();
    XTime_SetTime(0);
    XTime_GetTime(&time);
    cycles = time;
    for(i = 1; i <= 0xff; i++){
      	gcd = extended_euclidean(0x11b, i, 0x11b, &alpha, &beta);
       	if(gcd == 1){
       		//printf("%x^-1: %x in 0x11b\r\n", i, beta);
       	}
       	else{
       		//printf("%x^-1: DNE in 0x11b\r\n", i);
       	}
    }
    XTime_GetTime(&time);
    cycles1 = time;
    printf("Cache Disabled, No Optimizations: Inverse Time(cycles):0x%x\r\n", cycles1-cycles);

    //printf("result:%d\r\n", bff_mod(0b10101, 0b10));
    //printf("eea:%x\r\n", extended_euclidean(0b10011, 0b101));
    //for(i = 0; i < 0xff; i++){
    //    printf("0xCB mod %x = %x\r\n", i, bff_mod(0xcb, i));
    //}
    cleanup_platform();
    return 0;
}

uint32_t bff_add(uint32_t a, uint32_t b, uint32_t f){
    uint32_t res = a ^ b;
    res = bff_mod(res, f);
    return res;
}

uint32_t bff_mult(uint32_t a, uint32_t b, uint32_t f){
    uint32_t total = 0;
    uint32_t deg = get_degree(f);
    uint32_t i;
    for(i = 0; i <= deg; i++){
        if((b&(1<<i)) >> i == 1){
            total ^= bff_mod(a << i, f);
        }
    }
    return total;
}

uint32_t bff_mod(uint32_t a, uint32_t b){
    if(b == 0){
        return a;
    }
    uint32_t b_deg = get_degree(b);
    uint32_t a_deg = get_degree(a);
    uint32_t res = a;
    while(a_deg >= b_deg){
        uint32_t diff = a_deg-b_deg;
        res = res^(b<<diff);
        a_deg = get_degree(res);
        if(res == 0)
            break;
    }
    return res;
}

uint32_t bff_div(uint32_t a, uint32_t b){
    uint32_t b_deg = get_degree(b);
    uint32_t a_deg = get_degree(a);
    uint32_t current = a;
    uint32_t res = 0;
    while(a_deg >= b_deg){
        uint32_t diff = a_deg - b_deg;
        current = current^(b<<diff);
        res ^= (1 << diff);
        a_deg = get_degree(current);
        if(current == 0)
            break;
    }
    return res;
}


uint32_t get_degree(uint32_t a){
    uint32_t degree = -1;
    while(a > 0){
        a = a >> 1;
        degree++;
    }
    return degree;
}

uint32_t get_list_order(uint32_t* list, uint32_t size){
    uint32_t i, t, order = 0;
    for(t = 0; t < size; t++){
        for(i = 0; i < size; i++){
            if(list[i] == t){
                order ++;
                break;
            }
        }
    }
    return order;
}

void get_generators(uint32_t m, uint32_t f){
    uint32_t r, c, gen_size = 0;

    for(r = 1; r < m; r ++){
        uint32_t count = 0;
        uint32_t pow_res = 1;
        for(c = 1; c < m; c++){
            pow_res = bff_mult(pow_res, r, f);
            count++;
            if(pow_res == 1)
                break;
        }
        if(count == m-1){
            if(gen_size > 0)
                printf(", ");
            printf("%x", r);
            gen_size ++;
        }
    }
    printf("\r\nNum Gens:%d\r\n", gen_size);
}

uint32_t power(uint32_t a, uint32_t b, uint32_t f){
    uint32_t i, output = 1;
    for(i = 0; i < b; i++){
        output = bff_mult(a, output, f);
    }
    return output;
}

uint32_t extended_euclidean(uint32_t a, uint32_t b, uint32_t f, uint32_t* alpha_out, uint32_t* beta_out){
    if(b == 1){
		*alpha_out = 0;
		*beta_out = 1;
		return 1;
	}
	uint32_t alpha2 = 1;
	uint32_t alpha1 = 0;
	uint32_t beta2 = 0;
	uint32_t beta1 = 1;
	uint32_t r1 = b;
	uint32_t r2 = a;

	uint32_t r = 3;
	uint32_t q, alpha, beta;
	while(r >= 2){
		q = bff_div(r2,r1);
		alpha = bff_add(alpha2, bff_mult(alpha1, q, f), f);
		beta = bff_add(beta2, bff_mult(beta1, q, f), f);
		r = bff_mod(r2, r1);
		alpha2 = alpha1;
		alpha1 = alpha;
		beta2 = beta1;
		beta1 = beta;
		r2 = r1;
		r1 = r;
	}
	if(r == 1){
		*alpha_out = alpha;
		*beta_out = beta;
		return r;
	}
	else{
		alpha_out = NULL;
		beta_out = NULL;
		return r2;
	}
    return 0;
}


