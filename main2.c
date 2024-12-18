#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "sha256.h"
#include "stratum.h"

// Fungsi untuk mencetak hash dalam format heksadesimal
void print_hash(const uint8_t hash[]) {
    for (int i = 0; i < 32; i++) {
        printf("%02x", hash[i]);
    }
    printf("\n");
}

// Fungsi untuk memeriksa apakah hash valid (sama atau lebih kecil dari target)
int is_valid_hash(const uint8_t hash[], const uint8_t target[]) {
    for (int i = 0; i < 32; i++) {
        if (hash[i] < target[i]) return 1; // Hash valid
        if (hash[i] > target[i]) return 0; // Hash tidak valid
    }
    return 1;
}

int main() {
    const char *pool_address = "sha256.na.mine.zpool.ca";
    const char *pool_port = "3333";
    const char *username = "D8zcvGHWayuBq5MRncKTnKtFcooQCueZLP";
    const char *password = "c=DGB,zap=DGC-sha";

    // Koneksi ke pool
    int pool_socket = connect_to_pool(pool_address, pool_port);
    if (pool_socket < 0) {
        fprintf(stderr, "Gagal terhubung ke pool\n");
        return 1;
    }
    printf("Terhubung ke pool %s:%s\n", pool_address, pool_port);

    // Kirim login ke pool
    if (!login_to_pool(pool_socket, username, password)) {
        fprintf(stderr, "Gagal login ke pool\n");
        close(pool_socket);
        return 1;
    }
    printf("Login berhasil sebagai %s\n", username);

    SHA256_CTX ctx;
    uint8_t hash[32];
    char job_id[64], prev_hash[64], data[128];
    uint8_t target[32];
    unsigned int nonce;

    while (1) {
        // Ambil pekerjaan dari pool
        if (!get_work(pool_socket, job_id, prev_hash, target)) {
            fprintf(stderr, "Gagal mendapatkan pekerjaan dari pool\n");
            close(pool_socket);
            return 1;
        }
        printf("Pekerjaan diterima: Job ID = %s, Target = ", job_id);
        print_hash(target);

        nonce = 0;
        while (1) {
            // Gabungkan data dengan nonce
            snprintf(data, sizeof(data), "%s:%u", prev_hash, nonce);

            // Proses hashing
            sha256_init(&ctx);
            sha256_update(&ctx, (const uint8_t *)data, strlen(data));
            sha256_final(&ctx, hash);

            // Periksa apakah hash valid
            if (is_valid_hash(hash, target)) {
                printf("Hash valid ditemukan!\n");
                printf("Job ID: %s\n", job_id);
                printf("Nonce: %u\n", nonce);
                printf("Hash: ");
                print_hash(hash);

                // Kirim hasil ke pool
                if (submit_work(pool_socket, job_id, nonce)) {
                    printf("Hasil berhasil dikirim ke pool\n");
                } else {
                    fprintf(stderr, "Gagal mengirim hasil ke pool\n");
                }
                break; // Keluar dari loop nonce setelah hash valid ditemukan
            }

            nonce++;
            if (nonce % 1000000 == 0) {
                printf("Nonce saat ini: %u\n", nonce);
            }
        }
    }

    close(pool_socket);
    return 0;
}
