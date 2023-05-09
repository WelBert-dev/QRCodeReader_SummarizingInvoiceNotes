// Copyright (c) 2022 Cesanta Software Limited
// All rights reserved
//
// REST basics example
// It implements the following endpoints:
//    /api/f1 - respond with a simple mock result
//    /api/sum - respond with the result of adding two numbers
//    any other URI serves static files from s_root_dir
// Results are JSON strings

#include "mongoose.h"
#include "/home/welbert/Documentos/github/QRCodeReader_SummarizingInvoiceNotes/backend/examples/webui-rest/cJSON/cJSON.h"
#include "/home/welbert/Documentos/github/QRCodeReader_SummarizingInvoiceNotes/backend/examples/webui-rest/cJSON/cJSON.c"
#include "/home/welbert/Documentos/github/QRCodeReader_SummarizingInvoiceNotes/backend/examples/webui-rest/stb/stb_image.h"

#include <qrencode.h>
#include <png.h>
#include <string.h>
#include <time.h>

static const char *s_http_addr = "http://localhost:8000";  // HTTP port
static const char *s_root_dir = "web_root";

static void generate_qrcode(const char* json_str, const char* filename) {
    // Formatar o comando para gerar o QR code
    printf("Antes de executar o comando: %s\n", json_str);
    char command[4096];
    snprintf(command, 4096, "qrencode -o %s \"%s\"", filename, json_str);
    printf("DEPOIS de executar o comando: %s\n", json_str);

    // Executar o comando no terminal e capturar a saída
    FILE* fp = popen(command, "r");
    if (!fp) {
        printf("Erro ao executar o comando %s\n", command);
        return;
    }
    printf("DEPOIS DE ABRIR ARQUIVO PARA LEITURA: %s\n", json_str);
    pclose(fp);
    printf("DEPOIS DE DAR CLOSE: %s\n", json_str);
}

static void fn(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {
  // mg_http_add_header(c, "Access-Control-Allow-Origin", "*");

  if (ev == MG_EV_HTTP_MSG) {
    struct mg_http_message *hm = (struct mg_http_message *) ev_data;
    if (mg_http_match_uri(hm, "/api/post")) {

      // impirmindo o json recebido no body:
      printf("Received JSON: %.*s\n", (int) hm->body.len, hm->body.ptr);

      // Extrai o corpo da mensagem HTTP
      char *body = malloc(hm->body.len + 1);
      memcpy(body, hm->body.ptr, hm->body.len);
      body[hm->body.len] = '\0';

      // Faz o parsing do JSON recebido
      cJSON *root = cJSON_Parse(body);

      if (root == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            printf("Error before: %s\n", error_ptr);
        }
        mg_http_reply(c, 400, "Content-Type: application/json\r\nAccess-Control-Allow-Origin: *\r\nAccess-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\nAccess-Control-Allow-Headers: Content-Type\r\n\r\n{\"msg\": \"Hello World!\"}", "{%Q:%s}\n",
                      "error", "ERROR ao tentar extrair o JSON! ;-;");

        free(body);
        return;
      }

      // Extrai o valor da chave "msg"
      cJSON *msg = cJSON_GetObjectItemCaseSensitive(root, "msg");
      char *msg_str = cJSON_PrintUnformatted(msg);
      printf("Valor da chave 'msg': %s\n", msg_str);

      if (msg == NULL || !cJSON_IsString(msg)) {
        printf("Error: invalid JSON\n");

        mg_http_reply(c, 400, "Content-Type: application/json\r\nAccess-Control-Allow-Origin: *\r\nAccess-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\nAccess-Control-Allow-Headers: Content-Type\r\n\r\n{\"msg\": \"Hello World!\"}", "{%Q:%s}\n",
                      "error", "JSON Inválido! ;-;");

        cJSON_Delete(root);
        free(body);
        return;
      }

      // Libera a memória alocada pela cJSON
      cJSON_Delete(root);

      // TUDO CERTO, Faz a conversão string to QRCode PNG

      // Gerar o QR code a partir da string JSON
      generate_qrcode(msg_str, "output.png");
    
      // // Abrir o arquivo de imagem para leitura (modo binário)
      // FILE *fp = fopen("output.png", "rb");
      // if (fp != NULL) {
      //   printf("%s", "FP NÂO È NULL\n");
      //   // Obter o tamanho do arquivo
      //   fseek(fp, 0L, SEEK_END);
      //   long int file_size = ftell(fp);
      //   fseek(fp, 0L, SEEK_SET);

      //   // Alocar um buffer para a imagem
      //   char *buf = (char *)malloc(file_size);
      //   if (buf == NULL) {
      //     // mg_http_send_error(c, 500, "Internal Server Error", "");
      //     mg_http_reply(c, 500, "Internal Server Error", "");
      //     fclose(fp);
      //     return;
      //   }

      //   // Ler a imagem no buffer
      //   size_t bytes_read = fread(buf, 1, file_size, fp);
      //   if (bytes_read != file_size) {
      //     // mg_http_send_error(c, 500, "Internal Server Error", "");

      //     mg_http_reply(c, 500, "Internal Server Error", "");

      //     free(buf);
      //     fclose(fp);
      //     return;
      //   }
      //   // Printa buffer para ver se esta correto (em hexadecimal) e depois verificamos o hexa da imagem
      //   for (int i = 0; i < bytes_read; i++) {
      //     printf("%02x", (unsigned char) buf[i]);
      //   }

      //   // Enviar a imagem como corpo da resposta HTTP
      //   printf("%s", "\nANTES DO RESPONSEEEEE");

      //   char content_type[] = "Content-Type: image/png\r\n";
      //   char access_control[] = "Access-Control-Allow-Origin: *\r\nAccess-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\n";
      //   char content_length[64];
      //   snprintf(content_length, sizeof(content_length), "Content-Length: %ld\r\n", bytes_read);
        
      //   mg_http_reply(c, 200, content_type, access_control, content_length, buf, bytes_read);

      // } else {
      //   // Tratar erro de abertura de arquivo
      // }

    int width, height, channels;
    unsigned char* image_data = stbi_load("output.png", &width, &height, &channels, 0);

    // Verifica se a imagem foi carregada corretamente
    if (image_data == NULL) {
        printf("Erro ao carregar a imagem\n");
    }

    // Faça algo com o buffer de imagem aqui
    printf("BUFFER GERADO: %s\n", " ");
    for (int i = 0; i < width * height * channels; i++) {
      printf("%02x", image_data[i]);
    }

    // Libera a memória usada pelo buffer
    stbi_image_free(image_data);

    } else {
      struct mg_http_serve_opts opts = {.root_dir = s_root_dir};
      mg_http_serve_dir(c, ev_data, &opts);
    }
  }
  (void) fn_data;
}

int main(void) {
  struct mg_mgr mgr;                            // Event manager
  mg_log_set(MG_LL_DEBUG);                      // Set log level
  mg_mgr_init(&mgr);                            // Initialise event manager
  mg_http_listen(&mgr, s_http_addr, fn, NULL);  // Create HTTP listener
  for (;;) mg_mgr_poll(&mgr, 1000);             // Infinite event loop
  mg_mgr_free(&mgr);
  return 0;
}