#ifndef SHA256_H
#define SHA256_H

#include <stdint.h>
#include <stddef.h>

// Struktur konteks SHA-256
typedef struct {
    uint32_t state[8];
    uint64_t count;
    uint8_t buffer[64];
} SHA256_CTX;

// Fungsi untuk inisialisasi, update, dan finalisasi SHA-256
void sha256_init(SHA256_CTX *ctx);
void sha256_update(SHA256_CTX *ctx, const uint8_t *data, size_t len);
void sha256_final(SHA256_CTX *ctx, uint8_t hash[]);

#endif
