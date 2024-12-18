#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "stratum.h"

// Fungsi untuk membuat koneksi ke pool
int connect_to_pool(const char *address, const char *port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) return -1;

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(port));

    if (inet_pton(AF_INET, address, &server.sin_addr) <= 0) return -1;
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) return -1;

    return sock;
}

// Fungsi untuk login ke pool
int login_to_pool(int sock, const char *username, const char *password) {
    char request[256];
    snprintf(request, sizeof(request), "{\"id\":1,\"method\":\"mining.subscribe\",\"params\":[]}\n");
    send(sock, request, strlen(request), 0);

    snprintf(request, sizeof(request), "{\"id\":2,\"method\":\"mining.authorize\",\"params\":[\"%s\",\"%s\"]}\n", username, password);
    send(sock, request, strlen(request), 0);

    return 1; // Anggap sukses untuk contoh ini
}

// Fungsi untuk mendapatkan pekerjaan dari pool
int get_work(int sock, char *job_id, char *prev_hash, uint8_t *target) {
    char buffer[1024];
    recv(sock, buffer, sizeof(buffer), 0);

    // Parsing sederhana (realitanya gunakan JSON parser)
    sscanf(buffer, "{\"id\":%*d,\"result\":[\"%[^\"]\",\"%[^\"]\",%*[^,],\"%[^\"]\"]", job_id, prev_hash, target);
    return 1; // Anggap sukses untuk contoh ini
}

// Fungsi untuk mengirimkan hasil ke pool
int submit_work(int sock, const char *job_id, unsigned int nonce) {
    char request[256];
    snprintf(request, sizeof(request), "{\"id\":4,\"method\":\"mining.submit\",\"params\":[\"%s\",%u]}\n", job_id, nonce);
    send(sock, request, strlen(request), 0);
    return 1; // Anggap sukses untuk contoh ini
}
