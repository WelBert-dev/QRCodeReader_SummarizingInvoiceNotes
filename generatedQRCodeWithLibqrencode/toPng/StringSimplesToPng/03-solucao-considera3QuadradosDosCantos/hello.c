#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <qrencode.h>
#include <png.h>

#define OUT_FILE "hello.png"
#define QR_GET_MODULE(qrcode, x, y) (((qrcode)->data[((y) * (qrcode)->width + (x)) / 8] >> (7 - ((x) % 8))) & 1)

int main(int argc, char *argv[]) {

    char *string = "Hello, world!";
    int size = 5;
    int margin = 4;
    int dpi = 72;

    QRcode *qrcode = QRcode_encodeString(string, 0, QR_ECLEVEL_L, QR_MODE_8, 1);

    int width = qrcode->width;
    int total_width = (size * width) + (margin * 2);
    int byte_width = total_width / 8 + ((total_width % 8) ? 1 : 0);
    int pixel_size = byte_width * 8;
    unsigned char *image_data = (unsigned char *)malloc(pixel_size * total_width);

    memset(image_data, 0xff, pixel_size * total_width);

    for (int y = 0; y < width; y++) {
        for (int x = 0; x < width; x++) {
            int real_x = (size * x) + margin;
            int real_y = (size * y) + margin;
            int module = QR_GET_MODULE(qrcode, x, y);
            int byte_index = real_y * byte_width + (real_x / 8);
            unsigned char bit_mask = 1 << (7 - (real_x % 8));
            image_data[byte_index] &= ~bit_mask;
            if (module) {
                image_data[byte_index] |= bit_mask;
            }
        }
    }

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            for (int y = 0; y < margin; y++) {
                for (int x = 0; x < margin; x++) {
                    int real_x = (size * (width + j)) + x;
                    int real_y = (size * i) + y;
                    int byte_index = real_y * byte_width + (real_x / 8);
                    unsigned char bit_mask = 1 << (7 - (real_x % 8));
                    image_data[byte_index] &= ~bit_mask;
                }
            }
        }
    }

    FILE *fp = fopen(OUT_FILE, "wb");

    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop info_ptr = png_create_info_struct(png_ptr);
    png_init_io(png_ptr, fp);
    png_set_IHDR(png_ptr, info_ptr, total_width, total_width, 1, PNG_COLOR_TYPE_GRAY, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    png_write_info(png_ptr, info_ptr);

    for (int y = 0; y < total_width; y++) {
        png_write_row(png_ptr, image_data + y * byte_width);
    }

    png_write_end(png_ptr, NULL);
    fclose(fp);
    QRcode_free(qrcode);
    free(image_data);
    return 0;
}
