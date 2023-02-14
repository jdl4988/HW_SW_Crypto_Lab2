#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "main.h"

int main(){
    uint32_t a = 0b1100;
    uint32_t b = 0b111;
    uint32_t f = 0b10011;
    char generators[500];
    uint32_t i;
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

    get_generators(256, 0x11b);
    printf("Generators for 2^8: %s", generators);
    //printf("result:%d\r\n", bff_div(0b10101, 0b10));
   //printf("result:%d\r\n", bff_mod(0b10101, 0b10));
    printf("eea:%x\r\n", extended_euclidean(0b10011, 0b101, 0x11b));
    //for(i = 0; i < 0xff; i++){
    //    printf("0xCB mod %x = %x\r\n", i, bff_mod(0xcb, i));
    //}
    return 0;
}

uint32_t bff_add(uint32_t a, uint32_t b, uint32_t f){
    uint32_t res = a ^ b;
    uint32_t deg = get_degree(f);
    //if(res > 0b10 << deg){
     //   res = bff_mod(res, (0b10 << deg));
    //}
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
    uint32_t i, res = a;
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
    uint32_t i, current = a;
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
    uint32_t vals[255];
    uint32_t range[255];
    uint32_t r, c, i, gen_size = 0;

    for(r = 1; r < m; r ++){
        uint32_t cols[512];
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
            vals[gen_size] = r;
            gen_size ++;
        }
    }
}

uint32_t power(uint32_t a, uint32_t b, uint32_t f){
    uint32_t i, output = 1;
    for(i = 0; i < b; i++){
        output = bff_mult(a, output, f);
    }
    return output;
}

uint32_t extended_euclidean(uint32_t a, uint32_t b, uint32_t f){
    uint32_t r0;
    uint32_t r1 = b;
    uint32_t q0 = bff_div(a, b);
	
    printf("mul:%x\r\n", bff_mult(b, q0, f));
    r0 = bff_add(a, bff_mult(b, q0, f), f);
    printf("q:%x\r\n", q0);
    printf("r:%x\r\n", r0);
    //uint32_t q1 = bff_div(b, r0);
	
    while(r0 > 0){ 
        q0 = bff_div(r1, r0);
		printf("-q:%x\r\n", q0);
		r0 = bff_add(r1, bff_mult(r0, q0, f), f);
    }
	printf("q:%x\r\n", q0);
    printf("r:%x\r\n", r0);

}

