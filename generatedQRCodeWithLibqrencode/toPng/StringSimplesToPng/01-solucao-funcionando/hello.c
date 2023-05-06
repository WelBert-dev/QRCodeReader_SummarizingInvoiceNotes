#include <stdio.h>
#include <stdlib.h>
#include <qrencode.h>
#include <png.h>

void write_png_file(char* filename, int width, int height, unsigned char* image_data) {
    FILE *fp;
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;
    png_bytep row = NULL;

    fp = fopen(filename, "wb");
    if (!fp) {
        printf("Error opening file %s for writing.\n", filename);
        return;
    }

    // Initialize PNG write structure
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL) {
        printf("Error initializing PNG write structure.\n");
        fclose(fp);
        return;
    }

    // Initialize PNG info structure
    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
        printf("Error initializing PNG info structure.\n");
        png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
        fclose(fp);
        return;
    }

    // Set error handling
    if (setjmp(png_jmpbuf(png_ptr))) {
        printf("Error during PNG write.\n");
        png_destroy_write_struct(&png_ptr, &info_ptr);
        fclose(fp);
        return;
    }

    // Set PNG write options
    png_set_IHDR(png_ptr, info_ptr, width, height,
                 8, PNG_COLOR_TYPE_GRAY, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    // Write header information
    png_init_io(png_ptr, fp);
    png_write_info(png_ptr, info_ptr);

    // Write image data
    int i;
    for (i = 0; i < height; i++) {
        row = image_data + i * width;
        png_write_row(png_ptr, row);
    }

    // Finalize write
    png_write_end(png_ptr, NULL);

    // Clean up
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
}


int main() {
    // Define a mensagem a ser codificada no QR code
    char* message = "hello word!";

    QRcode* qrcode = QRcode_encodeString(message, 0, QR_ECLEVEL_M, QR_MODE_8, 1);

    // Allocate buffer for image data
    unsigned char* image_data = (unsigned char*)malloc(qrcode->width * qrcode->width * sizeof(unsigned char));

    // Convert QRcode data to grayscale image data
    int i, j;
    for (i = 0; i < qrcode->width; i++) {
        for (j = 0; j < qrcode->width; j++) {
            image_data[i*qrcode->width+j] = qrcode->data[i*qrcode->width+j] ? 0 : 255;
        }
    }

    // Write image data to file
    write_png_file("hello.png", qrcode->width, qrcode->width, image_data);

    // Clean up
    QRcode_free(qrcode);
    free(image_data);

    return 0;
}
