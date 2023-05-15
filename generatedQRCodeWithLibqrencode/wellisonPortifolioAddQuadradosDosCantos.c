#include <stdio.h>
#include <stdlib.h>
#include "qrencode.h"


void printQRCode(QRcode *qrcode) {
    int width = qrcode->width;
    unsigned char *data = qrcode->data;
    
    printf("Width do QR gerado: %d\n", qrcode->width);
    // Imprime a matriz do QR code com os quadrados de Position Detection Patterns
    int i, j;
    for (i = 0; i < width; i++) {
        for (j = 0; j < width; j++) {
            // Verifica se a posição corresponde a um quadrado de Position Detection Pattern
            int isPD = (i < 7 && j < 7) ||       // Canto superior esquerdo 7x7 Módulo
                       (i < 7 && j >= width - 7) ||      // Canto superior direito 7x7 Módulo
                       (i >= width - 7 && j < 7);        // Canto inferior esquerdo 7x7 módulo

            // Imprime '#' se for um quadrado de Position Detection Pattern, e caso contrário, 
            // faz outra verificação para saber se o elemento corrente no looping for é o 
            // correspondente do payload (carga útil que é a informação em sí + bits redundantes 
            // para correções de erros ditos anteriormente) e assim imprime o correspondente 
            // 0 ou 1 no console: 

            // Mais detalhes sobre PD: 
            // O Módulo vai depender do 4o parâmetro da chamada na função principal
            // QRcode *qrcode = QRcode_encodeString(string, 0, QR_ECLEVEL_L, QR_MODE_8, 1);
            // Ou seja, se o modulo for esse, então serão 7x7 Caracteres pois é o tipo de
            // codificação que utilizamos para suportar o UTF8 (cada caractere é 8bits).
            // esse PD sempre vai ter a mensma dimensão independente da informação armazenada
            // no QRCode.

            // Mais sobre width: A Largura vai depender e se auto dimensionar de acordo com 
            // o tamanho da informação armazenada neste QRCode em sí, o papel dela é quebrar 
            // a linha simulando um array bidimensional, dispondo a informação em formato de
            // matriz.
            
            printf("%c", isPD ? '#' : (data[i * width + j] ? '1' : '0'));
        }
        printf("\n");
    }
}

int main() {
    char string[] = "hello, world!";
    QRcode *qrcode = QRcode_encodeString(string, 0, QR_ECLEVEL_L, QR_MODE_8, 1);

    // Imprime o QR code com os Position Detection Patterns
    printQRCode(qrcode);

    QRcode_free(qrcode);
    
    return 0;
}

























