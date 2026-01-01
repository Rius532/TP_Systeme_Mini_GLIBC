#include <unistd.h>
#include <errno.h>
#include "mini_lib.h"

//#define DEBUG
#ifdef DEBUG
    #include <stdio.h>
#endif

int BUF_SIZE = 1024;
char* buffer = NULL;
int ind = -1;

void mini_flush(){
    /* Envoie le buffer si il n'est pas vide */
    if (ind >0 && buffer != NULL){
        write(1, buffer, ind);
        ind = 0;
    }
}

void mini_printf(char* msg){
    if (ind == -1){
        buffer = (char *)mini_calloc(sizeof(char), BUF_SIZE);
        if (buffer == NULL) mini_exit(1);
        ind = 0;
    }
    int len = mini_strlen(msg);
    for (int i = 0; i < len; i++){
        buffer[ind] = msg[i];
        ind++;
        if ((ind == BUF_SIZE) || (msg[i] == '\n')){
            write(1, buffer, ind);
            ind = 0;
        }
    }
}

int mini_scanf(char* buffer, int size_buffer){
    mini_flush();
    int n = read(0, buffer, size_buffer -1);
    if (n < 0) mini_exit(1);
    buffer[n] = '\0';
    
    if(buffer[n-1] != '\n' && n == size_buffer - 1){
        char c;
        int ret;
        while(1){
            ret = read(0, &c, 1);
            if (ret <= 0 || c == '\n') {
                break;
            }
        }
    }
    
    if(n > 0 && buffer[n - 1] == '\n'){
        buffer[n - 1] = '\0';
        n--;
    }

    return n;
}

int mini_strcpy(char* dest, char* src){
    if(dest == NULL || src == NULL) return -1; 
    int i = 0;
    while (src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
    return i;
}

int mini_strcmp(char* s1, char* s2){
    if (s1 == NULL || s2 == NULL) return -1; 
    int i = 0;
    while (s1[i] == s2[i] && s1[i] != '\0') {
        i++;
    }
    return (unsigned char)s1[i] - (unsigned char)s2[i];
}

/* Fonction utilitaire : Convertit un int en chaîne */
void int_to_string(int n, char* buffer) {
    int i = 0;
    int is_negative = 0;

    if (n == 0) {
        buffer[0] = '0';
        buffer[1] = '\0';
        return;
    }

    if (n < 0) {
        is_negative = 1;
        n = -n;
    }

    while (n != 0) {
        int rem = n % 10;
        buffer[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        n = n / 10;
    }

    if (is_negative) {
        buffer[i++] = '-';
    }

    buffer[i] = '\0';

    int start = 0;
    int end = i - 1;
    while (start < end) {
        char temp = buffer[start];
        buffer[start] = buffer[end];
        buffer[end] = temp;
        start++;
        end--;
    }
}

void mini_perror(char* message) {
    if (message != NULL) {
        write(2, message, mini_strlen(message));
        write(2, ": ", 2);
    }
    char num_buf[12];
    int_to_string(errno, num_buf);
    write(2, "Error Code ", 11);
    write(2, num_buf, mini_strlen(num_buf));
    write(2, "\n", 1);
}

void mini_putchar(char c) {
    write(1, &c, 1);
}

void mini_putstr(char *str) {
    if (!str) return;
    write(1, str, mini_strlen(str));
}

void mini_putnbr(long n) {
    if (n < 0) {
        mini_putchar('-');
        n = -n;
    }
    if (n >= 10)
        mini_putnbr(n / 10);
    mini_putchar((n % 10) + '0');
}