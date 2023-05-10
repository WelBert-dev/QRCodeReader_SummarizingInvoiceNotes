#include "mongoose.h"
#include "/home/welbert/Documentos/github/QRCodeReader_SummarizingInvoiceNotes/backend/examples/webui-rest/cJSON/cJSON.h"
#include "/home/welbert/Documentos/github/QRCodeReader_SummarizingInvoiceNotes/backend/examples/webui-rest/cJSON/cJSON.c"

#include <qrencode.h>
#include <png.h>
#include <string.h>
#include <regex.h>

#ifndef CLOCK_REALTIME
#define CLOCK_REALTIME 0
#endif

#include <time.h>

static const char *s_http_addr = "http://localhost:8000";  // HTTP port
static const char *s_root_dir = "web_root";

static void generate_qrcode(const char* json_str, const char* filename) {
    // Formatar o comando para gerar o QR code
    printf("Antes de executar o comando: %s\n", json_str);
    char command[4096];
    snprintf(command, 4096, "qrencode -o %s.png \"%s\"", filename, json_str);
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

// Retorna o tipo MIME do arquivo de caminho 'path'
// Retorna NULL se o tipo MIME não for conhecido
static const char *get_mime_type(const char *path) {
  const char *ext = strrchr(path, '.');
  if (!ext) {
    return NULL;
  }
  if (strcmp(ext, ".html") == 0 || strcmp(ext, ".htm") == 0) {
    return "text/html";
  }
  if (strcmp(ext, ".css") == 0) {
    return "text/css";
  }
  if (strcmp(ext, ".js") == 0) {
    return "application/javascript";
  }
  if (strcmp(ext, ".png") == 0) {
    return "image/png";
  }
  if (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0) {
    return "image/jpeg";
  }
  return NULL;
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
      char *msg_str = cJSON_GetStringValue(msg);
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
      // cJSON_Delete(root);

      // TUDO CERTO, Faz a conversão string to QRCode PNG

      // Gerar o QR code a partir da string JSON
      printf("MSG STRING APÒS CONCAT: %s\n", msg_str);

      // Primeiro retira os caracteres especiais do link para salvar a imagem com esse nome:
      char *urlBefore = malloc(strlen(msg_str) + 1); // alocar memória para a string resultante
      strcpy(urlBefore, msg_str); // copiar a string original para url

      regex_t regex;
      regmatch_t pmatch[1];
      
      if (regcomp(&regex, "/", REG_EXTENDED) != 0) {
          printf("Erro na compilacao da expressao regular\n");
          return 1;
      }
      
      char *output = (char*) malloc(strlen(urlBefore));
      int output_pos = 0;
      int input_pos = 0;
      
      while (regexec(&regex, urlBefore + input_pos, 1, pmatch, 0) == 0) {
          int match_start = pmatch[0].rm_so;
          int match_end = pmatch[0].rm_eo;
          int match_len = match_end - match_start;
          strncpy(output + output_pos, urlBefore + input_pos, match_start);
          output_pos += match_start;
          input_pos += match_end;
      }
      
      strcpy(output + output_pos, urlBefore + input_pos);
      printf("Input: %s\nOutput: %s\n", urlBefore, output);

      generate_qrcode(msg_str, output);

      int width, height;
      char pathTest[1024];
      snprintf(pathTest, sizeof(pathTest), "%s%s.png", "/home/welbert/Documentos/github/QRCodeReader_SummarizingInvoiceNotes/backend/examples/webui-rest/", output);
      printf("FILE PATH: %s\n", pathTest);

      cJSON *json_response = cJSON_CreateObject();
      cJSON_AddStringToObject(json_response, "path", pathTest);

      // converter o objeto JSON em uma string
      char *json_response_str = cJSON_Print(json_response);
      cJSON_Delete(json_response);

      mg_http_reply(c, 
                    200, 
                    "Content-Type: application/json\r\nAccess-Control-Allow-Origin: *\r\nAccess-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\n",
                    json_response_str);

      free(json_response_str);

      free(output);

      regfree(&regex);

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