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

#include <curl/curl.h>

#define API_KEY "AIzaSyDJ2ysDUfcerIWw16djY_THaNGRnPS39ys"
#define UPLOAD_URL "https://www.googleapis.com/upload/drive/v3/files?uploadType=multipart"

static const char *s_http_addr = "http://localhost:8000";  // HTTP port
static const char *s_root_dir = "web_root";

// Definições de Paths:
char imgsDirPath[] = "/home/welbert/Documentos/github/QRCodeReader_SummarizingInvoiceNotes/backend/examples/webui-rest/images/";

char content_type[] = "Content-Type: application/json\r\n";
char access_control[] = "Access-Control-Allow-Origin: http://localhost:3000\r\nAccess-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\nAccess-Control-Allow-Headers: content-type, mode\r\n\r\n";
char content_length[64];

static void generate_qrcode(const char* json_str, const char* filename) {
    // Formatar o comando para gerar o QR code
    printf("Antes de executar o comando: %s\n", json_str);
    char command[4096];
    snprintf(command, 4096, "qrencode -o %s%s.png \"%s\"", imgsDirPath, filename, json_str);
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

// // Retorna o tipo MIME do arquivo de caminho 'path'
// // Retorna NULL se o tipo MIME não for conhecido
// static const char *get_mime_type(const char *path) {
//   const char *ext = strrchr(path, '.');
//   if (!ext) {
//     return NULL;
//   }
//   if (strcmp(ext, ".html") == 0 || strcmp(ext, ".htm") == 0) {
//     return "text/html";
//   }
//   if (strcmp(ext, ".css") == 0) {
//     return "text/css";
//   }
//   if (strcmp(ext, ".js") == 0) {
//     return "application/javascript";
//   }
//   if (strcmp(ext, ".png") == 0) {
//     return "image/png";
//   }
//   if (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0) {
//     return "image/jpeg";
//   }
//   return NULL;
// }

// Função de callback para ler o arquivo de imagem
// static size_t read_callback(void *ptr, size_t size, size_t nmemb, void *stream) {
//     size_t retcode = fread(ptr, size, nmemb, stream);
//     return retcode;
// }

static void fn(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {
  //mg_http_add_header(c, "Access-Control-Allow-Origin", "*");
  // struct mg_http_message *hm = (struct mg_http_message *) ev_data;
  // printf("\n\n\nREQUISIÇÂO METHOD: %s\n\n\n", hm->method.ptr);
  if (ev == MG_EV_HTTP_MSG) {
    struct mg_http_message *hm = (struct mg_http_message *) ev_data;
    printf("\n\n\nREQUISIÇÂO METHOD: %s\n\n\n", hm->method.ptr);
    if (mg_http_match_uri(hm, "/api/post") && mg_vcmp(&hm->method, "OPTIONS") == 0) {
      printf("Recebida requisição OPTIONS pré-voo\n");

      // Definições dos atributos comuns nas respostas HTTP:
      // char content_type[] = "Content-Type: application/json\r\n";
      // char access_control[] = "Access-Control-Allow-Origin: http://localhost:3000\r\nAccess-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\nAccess-Control-Allow-Headers: Content-type, mode\r\n\r\n";
      // // char content_length[64];
      
      mg_http_reply(c, 204, "Content-Type: application/json\r\nAccess-Control-Allow-Origin: *\r\nAccess-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\nAccess-Control-Allow-Headers: content-type, mode\r\n\r\n", "");
      // mg_http_reply(c, 204, content_type, access_control, "");
      return;
    } 
    else if (mg_http_match_uri(hm, "/api/post") && mg_vcmp(&hm->method, "POST") == 0) {
      printf("\n\n\nREQUISIÇÂO METHOD: %s\n\n\n", hm->method.ptr);

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

        mg_http_reply(c, 400, content_type, access_control, "Error ao tentar ler Json do body");

        // mg_http_reply(c, 400, "Content-Type: application/json\r\nAccess-Control-Allow-Origin: *\r\nAccess-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\nAccess-Control-Allow-Headers: Content-Type\r\n\r\n",
        //               "error", "ERROR ao tentar extrair o JSON! ;-;");

        free(body);
        return;
      }

      // Extrai o valor da chave "msg"
      cJSON *msg = cJSON_GetObjectItemCaseSensitive(root, "msg");
      char *msg_str = cJSON_GetStringValue(msg);
      printf("Valor da chave 'msg': %s\n", msg_str);

      if (msg == NULL || !cJSON_IsString(msg)) {
        printf("Error: invalid JSON\n");

        mg_http_reply(c, 400, content_type, access_control, "Error json lido porém inválido");

        // mg_http_reply(c, 400, "Content-Type: application/json\r\nAccess-Control-Allow-Origin: *\r\nAccess-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\nAccess-Control-Allow-Headers: Content-Type\r\n\r\n{\"msg\": \"Hello World!\"}", "{%Q:%s}\n",
        //               "error", "JSON Inválido! ;-;");

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
      
      if (regcomp(&regex, "[^[:alnum:]]", REG_EXTENDED) != 0) {
          printf("Erro na compilacao da expressao regular\n");
          return;
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

      // começa 

      int width, height;
      char pathTest[1024];
      snprintf(pathTest, sizeof(pathTest), "%s%s.png", imgsDirPath, output);
      printf("FILE PATH: %s\n", pathTest);

      free(output);
      regfree(&regex);

      FILE *fp = fopen(pathTest, "rb");
      if (fp != NULL) {
        printf("\n\n\n%s\n\n\n", "FP NÂO È NULL\n");
        // Obter o tamanho do arquivo
        fseek(fp, 0L, SEEK_END);
        size_t file_size = ftell(fp);
        fseek(fp, 0L, SEEK_SET);

        printf("\n\n\n%s\n\n\n","Alocando o buffer em memoria com o tamanho do file_size");
        // Alocar um buffer para a imagem
        char *buf = (char *)malloc(file_size);
        if (buf == NULL) {
          // mg_http_send_error(c, 500, "Internal Server Error", "");
          mg_http_reply(c, 500, "Internal Server Error", "");
          fclose(fp);
          return;
        }

        // Ler a imagem no buffer
        size_t bytes_read = fread(buf, 1, file_size, fp);
        if (bytes_read != file_size) {
          // mg_http_send_error(c, 500, "Internal Server Error", "");

          mg_http_reply(c, 500, "Internal Server Error", "");

          free(buf);
          fclose(fp);
          return;
        }

        // Printa buffer para ver se esta correto (em hexadecimal) e depois verificamos o hexa da imagem
        for (int i = 0; i < bytes_read; i++) {
          printf("%02x", (unsigned char) buf[i]);
        }

        // printf("\n\n\n%s\n\n\n", "CHEGOU NA TAREFA DO CURL!!!!!");
        // Faz upload da imagem no google drive, pois esta dando problema enviando na resp da req a imagem em buffer:
        // CURL *curl;
        // CURLcode res;

        // curl_global_init(CURL_GLOBAL_ALL);

        // curl = curl_easy_init();
        // if (curl) {
        //     printf("\n\n\n%s\n\n\n", "CHEGOU NO IF QUE VERIFICA SE CURL CARREGOU CORRETAMENTE!!!!!");
        //     struct curl_slist *headers = NULL;

        //     // Definir os cabeçalhos de autenticação e tipo de conteúdo
        //     headers = curl_slist_append(headers, "Authorization: Bearer AIzaSyDJ2ysDUfcerIWw16djY_THaNGRnPS39ys");
        //     headers = curl_slist_append(headers, "Content-Type: image/png");

        //     printf("\n\n\n%s\n\n\n", "DEFININDO OS DADOS DA REQUISIÇÂO POST!!!!!");
        //     // Definir os dados da solicitação HTTP POST
        //     curl_easy_setopt(curl, CURLOPT_POST, 1L);
        //     curl_easy_setopt(curl, CURLOPT_URL, UPLOAD_URL);
        //     printf("\n\n\n%s\n\n\n", "CARREGANDO IMAGEM EM BUFFER E SETANDO NO CURL!!!!!");
        //     curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);
        //     printf("\n\n\n%s\n\n\n", "CARREGOU POIS PASSOU DA LINHA!!!!!");
        //     curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
        //     curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        //     curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE_LARGE, (curl_off_t)-1);
        //     curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2TLS);
        //     curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        //     curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

        //     printf("\n\n\n%s\n\n\n", "SETADO TUDO FINALMENTE EXECUTA A REQUISIÇÂO CURL!!!!!");
        //     // Executar a solicitação HTTP POST
        //     res = curl_easy_perform(curl);
        //     printf("\n\n\n%s\n\n\n", "PASSOU DA LINHA QUE EXECUTA !!!!!");
        //     if (res != CURLE_OK) {
        //       printf("\n\n\n%s\n\n\n", "RESPOSTA DA REQUISIÇÂO FALHOU !!!!!");
        //       fprintf(stderr, "curl_easy_perform() falhou: %s\n",
        //               curl_easy_strerror(res));
        //     }
        //     printf("\n\n\n%s\n\n\n", "RESPOSTA DA REQUISIÇÂO OK !!!!!");

        //     // Analisar a resposta da API do Google Drive
        //     long response_code;
        //     curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        //     if (response_code == 200) {
        //         printf("Upload de imagem PNG concluído com sucesso.\n");
        //     } else {
        //         printf("Erro ao fazer upload de imagem PNG. Código de resposta: %ld\n", response_code);
        //     }

        //     // Liberar memória
        //     curl_slist_free_all(headers);
        //     curl_easy_cleanup(curl);
        // }

        // curl_global_cleanup();

        // Enviar a imagem como corpo da resposta HTTP
        printf("%s", "\nANTES DO RESPONSEEEEE\n\n");

        // mg_http_reply(c, 200, content_type, access_control, content_length, buf, bytes_read);

        // size_t content_length_size = sizeof("\r\nContent-Length: \r\n") + 10; // 10 é o tamanho máximo para o valor
        // char content_length[content_length_size];
        // snprintf(content_length, content_length_size, "Content-Length: %zu\r\n", bytes_read);


        printf("\n\n%s\n\n", content_length);

        // int bytes_read_int = (int) bytes_read;
        // snprintf(content_length, sizeof(content_length), "Content-Length: %d\r\n\0", bytes_read);
        // printf("\n\n\n\n\nINTEIRO: %d\n\n\n\n\n", bytes_read_int);

        // snprintf(content_length, sizeof(content_length), "%zu", file_size);

        // mg_http_se(c, 200, file_size, "Content-Type: image/png\r\nContent-Length: %s\r\n", content_length);
        // mg_http_send(c, buf, file_size);

        // mg_http_reply(c, 
        //               200, 
        //               "Content-Type: image/png\r\nAccess-Control-Allow-Origin: *\r\nAccess-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\nAccess-Control-Allow-Headers: Content-Type\r\n", content_length, buf);


        // mg_http_reply(c, 
        //               200, 
        //               "Content-Type: image/png\r\nAccess-Control-Allow-Origin: *\r\nAccess-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\nAccess-Control-Allow-Headers: Content-Type\r\n", 
        //               buf, 
        //               350);

        free(buf);
        fclose(fp);
        
        // mg_http_reply(c, 
        //       200, 
        //       "Content-Type: image/png\r\nAccess-Control-Allow-Origin: *\r\nAccess-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\nAccess-Control-Allow-Headers: Content-Type\r\n\r\n",
        //       content_length, buf, bytes_read);

        return;
      } else {
        // Tratar erro de abertura de arquivo
      }

      // finaliza

      // int width, height;
      // char pathTest[1024];
      // snprintf(pathTest, sizeof(pathTest), "%s%s", "/home/welbert/Documentos/github/QRCodeReader_SummarizingInvoiceNotes/backend/examples/webui-rest/images/", output);
      // printf("FILE PATH: %s\n", pathTest);

      // cJSON *json_response = cJSON_CreateObject();
      // cJSON_AddStringToObject(json_response, "path", pathTest);

      // // converter o objeto JSON em uma string
      // char *json_response_str = cJSON_Print(json_response);
      // cJSON_Delete(json_response);
      
      // mg_http_reply(c, 
      //               200, 
      //               "Content-Type: application/json\r\nAccess-Control-Allow-Origin: *\r\nAccess-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\nAccess-Control-Allow-Headers: Content-Type\r\n\r\n",
      //               json_response_str);

      // free(json_response_str);

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