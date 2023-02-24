#ifndef MAINH
#define MAINH

uint32_t get_degree(uint32_t a);
uint32_t bff_div(uint32_t a, uint32_t b);
uint32_t bff_mod(uint32_t a, uint32_t b);
uint32_t bff_add(uint32_t, uint32_t, uint32_t);
uint8_t* bff_add128(uint32_t, uint32_t, uint32_t);
uint32_t bff_mult(uint32_t, uint32_t, uint32_t);
void get_generators(uint32_t, uint32_t);
uint32_t get_list_order(uint32_t*, uint32_t);
uint32_t power(uint32_t, uint32_t, uint32_t);
uint32_t extended_euclidean(uint32_t, uint32_t, uint32_t, uint32_t*, uint32_t*);//a, b, f, alpha_out, beta_out


#endif



