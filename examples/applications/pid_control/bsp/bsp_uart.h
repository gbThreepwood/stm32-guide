#ifndef _BSP_UART_H
#define _BSP_UART_H

void bsp_init_uart();

char *intToStrDec(uint32_t x, char *s);

void print_uint32(uint32_t val);

void print_str(char *str);

#endif /* _BSP_UART_H */
