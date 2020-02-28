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
  if((debugs & 0x10) && (debugs & 0x01))
  {
    if(key != NULL)
    {
      fprintf(stderr, "ARGS: readWriteFile-fd1:%d, fd2: %d, len:%d debugs:%ld first 4 of key:%02x%02x crypt:%d iv:%s \n", fd1, fd2, len, debugs, key[0],key[1], crypt, iv);
    }
    else
      fprintf(stderr, "ARGS: readWriteFile- len:%d debugs:%ld \n", len, debugs);
  }
  int check_partial;
  int blocksz;
  int writeval;
  int retval;
  unsigned char *shapass = NULL;

  if(key != NULL && crypt == 2)//DECRYPT, GET/STORE KEY 
  {
    DBG_SYSCALL(debugs, "Before read()\n");
    blocksz = read(fd1, buf, 32);
    DBG_SYSCALL(debugs, "After read()\n");
      if(blocksz == 0){
        retval = 56; 
        return retval;
      }
      DBG_LIB(debugs, "Before malloc\n");
      shapass = malloc(64);
      DBG_LIB(debugs, "After malloc\n");

      if(shapass == NULL)
      {
        if(debugs && 0x20)
          fprintf(stderr, "Malloc failed. Exiting.");
        retval = 57;
        return retval;
      }
      DBG_LIB(debugs, "Before memcpy - storing [key] into buffer\n");
      memcpy(shapass, buf, 32);
      DBG_LIB(debugs, "After memcpy - storing [key] into buffer\n");
     

  }

  DBG_SYSCALL(debugs, "Before read().\n");

  while((blocksz = read(fd1, buf, len)) > 0 ){

    if (blocksz < 0) { // failed
      if(debugs && 0x20)
        fprintf(stderr, "Read() failed. Exiting.");
      retval = 8;
      return retval;
    }

    check_partial = 0;

    DBG_SYSCALL(debugs, "Before write(2)\n");

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
            if(shapass != NULL)
              free(shapass);
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
             if(shapass != NULL)
              free(shapass);
            retval = 50;
            return retval;
          } 
          check_partial = check_partial + writeval;  
        }
      }
      else
      {
        debug("not encrypting/decrypting, do nothing\n");
        writeval = write(fd2, buf, blocksz);
        if(writeval < 0)
        {
          retval = 9;
          return retval;
        }
        check_partial = check_partial + writeval;
      }
    }       
    DBG_SYSCALL(debugs, "After write()...\n");
  }
  DBG_SYSCALL(debugs, "After read().\n");


  if((debugs & 0x20) && (debugs & 0x01))
  {
    if(key != NULL){
      fprintf(stderr, "RET: readWriteFile- len:%d debugs:%ld first 4 of key:%02x%02x crypt:%d iv:%s \n", len, debugs, key[0],key[1], crypt, iv);
    }
    else{
      fprintf(stderr, "RET: readWriteFile- len:%d debugs:%ld\n", len, debugs);
    }
  }
 if(shapass != NULL)
    free(shapass);
  return 0;
}

int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
            unsigned char *iv, unsigned char *plaintext, long debugs)
{
  DBG_ENTEXIT(debugs, "Entering decrypt()");
  if((debugs & 0x10) && (debugs & 0x01))
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
    /*
     * Initialise the decryption operation.
     * In this example we are using 256 bit AES (i.e. a 256 bit key). 
     */
    if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_ctr(), NULL, key, iv))
        return -1;
    /*
     * Provide the message to be decrypted, and obtain the plaintext output.
     */
    if(1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
        return -1;

    plaintext_len = len;
    /*
     * Finalise the decryption. Further plaintext bytes may be written at
     * this stage.
     */
    if(1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len))
        return -1;
    plaintext_len += len;

    EVP_CIPHER_CTX_free(ctx);
    DBG_ENTEXIT(debugs, "Exiting decrypt()");

    if((debugs & 0x20) && (debugs & 0x01)){
      fprintf(stderr, "RET: decrypt- ciphertext_len:%d, first 4 of key: %02x%02x, iv:%s, debugs:%ld\n", ciphertext_len, key[0],key[1], iv, debugs);
    }

    return plaintext_len;
}

int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,
  unsigned char *iv, unsigned char *ciphertext, long debugs)
{
    DBG_ENTEXIT(debugs, "Entering encrypt()");

    if((debugs & 0x10) && (debugs & 0x01))
  {
    fprintf(stderr, "ARGS: encrypt- plaintext_len: %d, first 4 of key: %02x%02x, iv: %s, debugs: %ld \n", 
    plaintext_len, key[0], key[1], iv, debugs);
  }
    EVP_CIPHER_CTX *ctx;
    int len;
    int ciphertext_len;
    if(!(ctx = EVP_CIPHER_CTX_new()))
        return -1;


    /*
     * Initialise the encryption operation. 
     * using 256 bit AES (i.e. a 256 bit key).
     */
    if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_ctr(), NULL, key, iv))
        return -1;
    /*
     * Provide the message to be encrypted, and obtain the encrypted output.
     */
    if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
        return -1;
    ciphertext_len = len;
     /*
     * Finalise the encryption. Further ciphertext bytes may be written at
     * this stage.
     */
    if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
        return -1;

    ciphertext_len += len;
    
    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    DBG_ENTEXIT(debugs, "Exiting encrypt()");

    if((debugs & 0x20) && (debugs & 0x01)){
      fprintf(stderr, "RET: encrypt- plaintext_len:%d, first 4 of key: %02x%02x, iv:%s, debugs:%ld\n", plaintext_len, key[0],key[1], iv, debugs);
    }

    return ciphertext_len;
}