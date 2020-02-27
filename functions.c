#include "functions.h"
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include "macros.h"
#include <openssl/sha.h>

/* unsigned char *plaintext, int plaintext_len, unsigned char *key,
  unsigned char *iv, unsigned char *ciphertext */
int readWriteFile(int fd1, void* buf , int len, int fd2, long debugs,
  unsigned char *key, unsigned char *iv, unsigned char *ciphertext, int crypt)
{
  if(debugs & 0x10)
    if(key != NULL)
      fprintf(stderr, "ARGS: readWriteFile- len:%d debugs:%ld first 4 of key:%02x%02x crypt:%d iv:%s \n", len, debugs, key[0],key[1], crypt, iv);
    else
      fprintf(stderr, "ARGS: readWriteFile- len:%d debugs:%ld \n", len, debugs);

  int check_partial;
  int blocksz;
  int writeval;
  int retval;
  int passmatch = -1;
  unsigned char *shapass = NULL;

  if(key != NULL && crypt == 2)//DECRYPT, GET/STORE KEY 
  {
    blocksz = read(fd1, buf, 32);
      if(blocksz = 0){
        retval = 56; 
        return retval;
      }
    int i;
      shapass = malloc(64);
      if(shapass == NULL)
      {
        if(debugs && 0x20)
          fprintf(stderr, "Malloc failed. Exiting.");
        retval = 57;
        return retval;
      }

      memcpy(shapass, buf, 32);
  }

  DBG_SYSCALL(debugs, "Before read()");

  while((blocksz = read(fd1, buf, len)) > 0 ){

    if (blocksz < 0) { // failed
      if(debugs && 0x20)
        fprintf(stderr, "Read() failed. Exiting.");
      retval = 8;
      return retval;
    }

    check_partial = 0;

    DBG_SYSCALL(debugs, "Before write()");

    while(check_partial < blocksz)
    {
      if(key != NULL && crypt > 0)
      {
        //CHECK FOR PASSMATCH
        if(crypt == 1)//ENCRYPT
        {
          
          int shortval2 = 0;
          while(shortval2 != 32)
          {
            writeval = write(fd2, key, 32);
            if(writeval < 0)
            {
              if(debugs && 0x20)
                fprintf(stderr, "Write error. Exiting");
              return 50;
            }
            shortval2 = shortval2 + writeval;
          }
          int ciphertext_len;
          unsigned char ciphertext2[len];
          ciphertext_len = encrypt(buf, blocksz, key, iv,
                          ciphertext2, debugs);
          blocksz = ciphertext_len;
          writeval = write(fd2, ciphertext2, ciphertext_len);
          if(writeval < 0)
          {
            retval = 49;
            return retval;
          } 

          check_partial = check_partial + writeval; 
        }
        else if(crypt == 2)//DECRYPT
        {
          if(shapass == NULL)
          {
            if(debugs && 0x20)
              fprintf(stderr, "Unexpected error. Exiting.\n");
            retval = 61; 
            return retval;
          }

          if(memcmp(shapass, key, 32) != 0)
          {
            if(debugs && 0x20)
              fprintf(stderr, "Passwords do not match. Exiting.\n");
            retval = 62;
            return retval;
          }
          int decryptedtext_len;
          unsigned char dcrtxt2[len];
          decryptedtext_len = decrypt(buf, blocksz, key, iv,
                              dcrtxt2, debugs);
          blocksz = decryptedtext_len;

          writeval = write(fd2, dcrtxt2, decryptedtext_len);
          if(writeval < 0)
          {
            retval = 50;
            return retval;
          } 
          check_partial = check_partial + writeval;  
        }
      }
      else
      {
        debug("null, do nothing\n");
        writeval = write(fd2, buf, blocksz);
        if(writeval < 0)
        {
          retval = 9;
          return retval;
        }
        check_partial = check_partial + writeval;
      }
    }       
    DBG_SYSCALL(debugs, "After write()");
  }
  DBG_SYSCALL(debugs, "After read()");


  if(debugs & 0x20)
    if(key != NULL){
      fprintf(stderr, "RET: readWriteFile- len:%d debugs:%ld first 4 of key:%02x%02x crypt:%d iv:%s \n", len, debugs, key[0],key[1], crypt, iv);
    }
    else{
      fprintf(stderr, "RET: readWriteFile- len:%d debugs:%ld\n", len, debugs);
    }

  return 0;
}

int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
            unsigned char *iv, unsigned char *plaintext, long debugs)
{
  if(debugs & 0x10)
  {
    fprintf(stderr, "ARGS: decrypt- ciphertext_len: %d, first 4 of key: %02x%02x, iv: %s, debugs: %ld\n", 
    ciphertext_len, key[0], key[1], iv, debugs);
  }
    EVP_CIPHER_CTX *ctx;
    int len;
    int plaintext_len;
    /* Create and initialise the context */
    if(!(ctx = EVP_CIPHER_CTX_new()))
        return -1;

    if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_ctr(), NULL, key, iv))
        return -1;

    if(1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
        return -1;

    plaintext_len = len;

    if(1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len))
        return -1;
    plaintext_len += len;

    EVP_CIPHER_CTX_free(ctx);

    if(debugs && 0x20){
      fprintf(stderr, "RET: decrypt- ciphertext_len:%d, first 4 of key: %02x%02x, iv:%s, debugs:%ld\n", ciphertext_len, key[0],key[1], iv, debugs);
    }

    return plaintext_len;
}

int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,
  unsigned char *iv, unsigned char *ciphertext, long debugs)
{
    if(debugs & 0x10)
  {
    fprintf(stderr, "ARGS: encrypt- plaintext_len: %d, first 4 of key: %02x%02x, iv: %s, debugs: %ld \n", 
    plaintext_len, key[0], key[1], iv, debugs);
  }
    EVP_CIPHER_CTX *ctx;
    int len;
    int ciphertext_len;
    if(!(ctx = EVP_CIPHER_CTX_new()))
        return -1;

    if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_ctr(), NULL, key, iv))
        return -1;
    if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
        return -1;
    ciphertext_len = len;
    if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
        return -1;

    ciphertext_len += len;
    EVP_CIPHER_CTX_free(ctx);

    if(debugs && 0x20){
      fprintf(stderr, "RET: encrypt- plaintext_len:%d, first 4 of key: %02x%02x, iv:%s, debugs:%ld\n", plaintext_len, key[0],key[1], iv, debugs);
    }

    return ciphertext_len;
}