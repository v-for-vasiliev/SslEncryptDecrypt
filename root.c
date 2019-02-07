/* 
 * File:   root.cpp
 * Author: vasiliev
 *
 * Created on February 20, 2013, 11:32 AM
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <openssl/des.h>
#include <fcntl.h>

unsigned char *Encrypt(char *Key, unsigned char *Data, int size) {

    static char* Result;
    int n = 0;
    DES_cblock Key2;
    DES_key_schedule schedule;

    Result = (unsigned char *) malloc(size);

    /* Prepare the key */
    memcpy(Key2, Key, 8);
    DES_set_odd_parity(&Key2);
    DES_set_key_checked(&Key2, &schedule);

    /* Encryption */
    DES_cfb64_encrypt(Data, Result,
            size, &schedule, &Key2, &n, DES_ENCRYPT);

    return Result;
}

unsigned char *Decrypt(char *Key, unsigned char *Data, int size) {

    static unsigned char *Result;
    int n = 0;

    DES_cblock Key2;
    DES_key_schedule schedule;

    Result = (unsigned char *) malloc(size);

    /* Prepare the key */
    memcpy(Key2, Key, 8);
    DES_set_odd_parity(&Key2);
    DES_set_key_checked(&Key2, &schedule);

    /* Decryption */
    DES_cfb64_encrypt(Data, Result,
            size, &schedule, &Key2, &n, DES_DECRYPT);

    return Result;
}

int main(int argn, char * argv[]) {

    int src_fd, dst_fd, n, err;
    unsigned char buffer[4096];
    unsigned char *outbuffer;
    char * src_path, *dst_path;
    const char *Key = (const char*) "12bhsdf2fasg3312ds"; //passphraase
    /* 0 == ENCRYPT, 1 == DECRYPT */
    int mode = -1;

    if (argn != 4) {
        printf("Wrong argument count.\n");
        exit(1);
    }

    if (strstr(argv[1], (char*) "e") != NULL) {
        mode = 0;
        printf("Mode: encrypt.\n");
    } else if (strstr(argv[1], (char*) "d") != NULL) {
        mode = 1;
        printf("Mode: decrypt.\n");
    } else {
        printf("Wrong mode.\n");
        exit(1);
    }

    src_path = argv[2];
    dst_path = argv[3];
    
    printf("Source file: %s\n", src_path);
    printf("Out file:    %s\n", dst_path);

    src_fd = open(src_path, O_RDONLY);
    dst_fd = open(dst_path, O_RDWR | O_CREAT);
    
    if (src_fd < 0) {
        printf("Error opening file | READ.\n");
        exit(-1);
    }
    
    if (dst_fd < 0) {
        printf("Error opening file | WRITE.\n");
        exit(-1);
    }

    while (1) {
        err = read(src_fd, buffer, 4096);
        if (err == -1) {
            printf("Error reading file.\n");
            exit(-1);
        }
        n = err;

        if (n == 0) break;

        if (mode == 0) {
            outbuffer = Encrypt(Key, buffer, n);
        } else {
            outbuffer = Decrypt(Key, buffer, n);
        }

        err = write(dst_fd, outbuffer, n);
        if (err == -1) {
            printf("Error writing to file.\n");
            exit(1);
        }
    }

    close(src_fd);
    close(dst_fd);

    return 0;
}