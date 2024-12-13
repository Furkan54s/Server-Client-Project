#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8020
#define SERVER_ADDRESS "127.0.0.1"

int main() {
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in server;
    char mesaj[100] = {0};
    char komut[2];

    // Winsock baslat
    printf("Winsock baslatiliyor...\n");
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Baslatma hatasi: %d\n", WSAGetLastError());
        return 1;
    }
    printf("Winsock baslatildi.\n");

    // Socket olustur
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Socket olusturulamadi: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }
    printf("Socket olusturuldu.\n");

    // Sunucu adresi ayarla
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);

    // Sunucuya baglan
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        printf("Baglanti hatasi\n");
        closesocket(sock);
        WSACleanup();
        return 1;
    }
    printf("Sunucuya baglanti basarili.\n");

    // Kullanici arayuzu dongusu
    while (1) {
        printf("\n1: Yeni bilet iste\n2: Baglantiyi kapat\nSeciminizi yapin: ");
        scanf("%s", komut);

        // Komutu sunucuya gonder
        send(sock, komut, strlen(komut), 0);

        if (strcmp(komut, "1") == 0) {
            // Sunucudan cevap al
            memset(mesaj, 0, sizeof(mesaj));
            recv(sock, mesaj, sizeof(mesaj), 0);
            printf("Sunucudan gelen mesaj: %s\n", mesaj);
        } else if (strcmp(komut, "2") == 0) {
            printf("Baglanti kapatiliyor...\n");
            break;
        } else {
            printf("Gecersiz komut!\n");
        }
    }

    // Baglantiyi kapat
    closesocket(sock);
    WSACleanup();
    return 0;
}
