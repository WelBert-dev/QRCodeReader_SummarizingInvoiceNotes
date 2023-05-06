#include <stdio.h>
#include <stdlib.h>
#include <qrencode.h>

int main()
{
    // Define a mensagem a ser codificada no QR code
    char* message = "Hello, World!";
    
    // Cria um objeto QRcode a partir da mensagem
    QRcode* qrcode = QRcode_encodeString(message, 0, QR_ECLEVEL_M, QR_MODE_8, 1);
    
    // Imprime a matriz de pontos do QR code
    int i, j;
    for (i = 0; i < qrcode->width; i++) {
        for (j = 0; j < qrcode->width; j++) {
            printf("%c ", qrcode->data[i*qrcode->width+j]?'#':' ');
        }
        printf("\n");
    }
    
    // Libera a memória alocada pelo QRcode
    QRcode_free(qrcode);
    
    return 0;
}