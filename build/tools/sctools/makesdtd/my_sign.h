#ifndef _MY_SIGN_H_
#define _MY_SIGN_H_


int cryptopc(char *input_file, char *output_file);
void cryptopc_end(void);
int cryptopc_init(char *key_file);


#ifdef __cplusplus
}
#endif

#endif /* _MY_SIGN_H_ */
