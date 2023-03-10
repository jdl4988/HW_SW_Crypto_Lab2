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
        printf("(0x8d * %x) mod 0x11b: %x\r\n", i, bff_mult(0xcb, i, 0x11b));
    }
    printf("Division:\r\n");
    for(i = 1; i < 0xff; i++){
        printf("(0xCB / %x): %x\r\n", i, bff_div(0xcb, i));
    }

    printf("Generators for 2^8: %s", generators);
    get_generators(256, 0x11b);
	
	printf("Extended Euclidian:\r\n");
	uint32_t alpha;
	uint32_t beta;
	uint32_t gcd;
    for(i = 1; i < 0xff; i++){
		gcd = extended_euclidean(0x11b, i, 0x11b, &alpha, &beta);
		if(gcd == 1){
			printf("%x^-1: %x in 0x11b\r\n", i, beta);
		}
		else{
			printf("%x^-1: DNE in 0x11b\r\n", i);
		}
    }
    //printf("result:%d\r\n", bff_div(0b10101, 0b10));
   //printf("result:%d\r\n", bff_mod(0b10101, 0b10));
    //printf("eea:%x\r\n", extended_euclidean(0b10011, 0b101, 0x11b));
    //for(i = 0; i < 0xff; i++){
    //    printf("0xCB mod %x = %x\r\n", i, bff_mod(0xcb, i));
    //}

	printf("SUM: 0x%x\r\n", bff_add(0xC3, 0x68, 0x11b));
	printf("mult: 0x%x\r\n", bff_mult(0x2, 0x8D, 0x11b));
	printf("mult: 0x%x\r\n", bff_mult(0b1000010, 0b100001, 0x11b));
	
    return 0;
}

/*
	return an array of 128 bits from the four parameters 
*/
/*
uint8_t* get_array(uint32_t a, uint32_t b, uint32_t c, uint32_t d){
    uint8_t* res = malloc(128*sizeof(uint8_t));
    return res;
}
*/
uint32_t bff_add(uint32_t a, uint32_t b, uint32_t f){
    uint32_t res = a ^ b;
    res = bff_mod(res, f);
    return res;
}
/*
uint8_t* bff_add128(uint8_t* a, uint8_t* b, uint8_t* f){
    uint8_t* res = malloc(128*sizeof(uint8_t));
	uint32_t i;
	for(i = 0; i < 128){
		res[i] = a[i]^b[i];
	}
	
	//= a ^ b;
    //uint32_t deg = get_degree(f);
    //res = bff_mod(res, f);
    return res;
}
*/
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
        uint32_t count = 0;
        uint32_t pow_res = 1;
        for(c = 1; c < m; c++){
            pow_res = bff_mult(pow_res, r, f);
			//if(r == 2){
				//printf("\r\npower res:%x", pow_res);
			//}
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
	printf("\r\nNum Generators: %d\r\n", gen_size);
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

