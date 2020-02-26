int readWriteFile(int fd1, void* buf , int len, int fd2, long debugs, 
    unsigned char *key,  unsigned char *iv, unsigned char *ciphertext, int crypt);
int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,
            unsigned char *iv, unsigned char *ciphertext);
int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
            unsigned char *iv, unsigned char *plaintext);