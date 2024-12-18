#ifndef STRATUM_H
#define STRATUM_H

int connect_to_pool(const char *address, const char *port);
int login_to_pool(int sock, const char *username, const char *password);
int get_work(int sock, char *job_id, char *prev_hash, uint8_t *target);
int submit_work(int sock, const char *job_id, unsigned int nonce);

#endif
