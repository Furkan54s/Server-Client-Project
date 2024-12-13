#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <time.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8020
#define MAX_BILET 20

int main() {
    WSADATA wsa;
    SOCKET server_fd, new_socket;
    struct sockaddr_in server, client;
    int client_len, biletSayisi = MAX_BILET, aktifBaglantiSayisi = 0;
    char mesaj[100], client_msg[10];

    // Winsock baslat
    printf("Winsock baslatiliyor...\n");
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Baslatma hatasi: %d\n", WSAGetLastError());
        return 1;
    }
    printf("Winsock baslatildi.\n");

    // Sunucu soketi olustur
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Socket olusturulamadi: %d\n", WSAGetLastError());
        return 1;
    }
    printf("Socket olusturuldu.\n");

    // Sunucu adresi ayarla
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    // Bagla
    if (bind(server_fd, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("Bind hatasi: %d\n", WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }
    printf("Baglama basarili.\n");

    // Dinleme moduna gec
    listen(server_fd, 5);
    printf("Sunucu baslatildi. Bilet sistemi calisiyor...\n");

    srand(time(NULL)); // Rastgele sayi uretimi icin seed ayarla

    while (1) {
        client_len = sizeof(struct sockaddr_in);

        // Gelen baglantiyi kabul et
        new_socket = accept(server_fd, (struct sockaddr *)&client, &client_len);
        if (new_socket == INVALID_SOCKET) {
            printf("Baglanti kabul hatasi: %d\n", WSAGetLastError());
            continue;
        }

        // Aktif baglanti sayisini artir
        aktifBaglantiSayisi++;
        printf("Yeni baglanti kabul edildi. Aktif baglanti sayisi: %d\n", aktifBaglantiSayisi);

        // Istemci ile surekli iletisim
        while (1) {
            // Istemci mesajini al
            memset(client_msg, 0, sizeof(client_msg));
            int recv_result = recv(new_socket, client_msg, sizeof(client_msg), 0);
            if (recv_result <= 0) {
                printf("Baglanti kesildi. Aktif baglanti sayisi: %d\n", --aktifBaglantiSayisi);
                closesocket(new_socket);
                break;
            }

            // Komut isle
            if (strcmp(client_msg, "1") == 0) { // Yeni bilet iste
                if (biletSayisi > 0) {
                    int biletNo = (rand() % 9000) + 1000; // 1000-9999 arasi 4 haneli sayi
                    snprintf(mesaj, sizeof(mesaj), "Bilet Numaraniz: %d\n", biletNo);
                    send(new_socket, mesaj, strlen(mesaj), 0);
                    printf("Bilet verildi: %d - Kalan Bilet Sayisi: %d - Aktif Baglanti Sayisi: %d\n", biletNo, --biletSayisi, aktifBaglantiSayisi);
                } else {
                    snprintf(mesaj, sizeof(mesaj), "Uzgunuz, tum biletler tukendi!\n");
                    send(new_socket, mesaj, strlen(mesaj), 0);
                }
            } else if (strcmp(client_msg, "2") == 0) { // Baglantiyi kapat
                printf("Istemci baglantiyi kapatti. Aktif baglanti sayisi: %d\n", --aktifBaglantiSayisi);
                closesocket(new_socket);
                break;
            } else {
                snprintf(mesaj, sizeof(mesaj), "Gecersiz komut! 1: Yeni bilet iste, 2: Cikis yap.\n");
                send(new_socket, mesaj, strlen(mesaj), 0);
            }
        }
    }

    // Sunucu kapatma
    closesocket(server_fd);
    WSACleanup();
    return 0;
}
