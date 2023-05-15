#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <qrencode.h>

void printQRCode(QRcode *qrcode) {
    int width = qrcode->width;
    unsigned char *data = qrcode->data;

    // Imprime a matriz do QR code
    int i, j;
    for (i = 0; i < width; i++) {
        for (j = 0; j < width; j++) {
            printf("%s", (data[i * width + j] ? "██" : "░░"));
        }
        printf("\n");
    }
}

int main() {
    char string[] = "Olá, mundo! 🌍";  // String UTF-8 com caracteres acentuados e emoji
    size_t string_len = strlen(string);

    // Aloca memória para os bytes UTF-8
    unsigned char *byte_data = (unsigned char *)malloc(string_len + 1);
    memcpy(byte_data, string, string_len + 1);

    // Codifica a string no padrão Byte Mode
    QRcode *qrcode = QRcode_encodeString(byte_data, 0, QR_ECLEVEL_L, QR_MODE_8, 1);

    // Imprime o QR code com caracteres UTF-8
    printQRCode(qrcode);

    QRcode_free(qrcode);
    free(byte_data);
    return 0;
}
