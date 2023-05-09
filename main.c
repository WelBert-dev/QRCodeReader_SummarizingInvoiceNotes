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

static const char *s_http_addr = "127.0.0.1:8000";  // HTTP port
static const char *s_root_dir = "web_root";

static void fn(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {
  if (ev == MG_EV_HTTP_MSG) {
    struct mg_http_message *hm = (struct mg_http_message *) ev_data;
    if (mg_http_match_uri(hm, "/api/get")) {
      // Returns home page (GET)

      mg_http_reply(c, 200, "Content-Type: application/json\r\n", "{%Q:%d}\n",
                    "result", 123);

    } else if (mg_http_match_uri(hm, "/api/post")) {
      // Attempt to fetch a JSON array from the body, hm->body

      // Aqui é aonde ele está LISTENING para POST
      struct mg_str json = hm->body;

      printf("JSON received: %.*s\n", (int) json.len, json.ptr);

      // Extrai o JSON do corpo da mensagem HTTP
      char *json_text = calloc(hm->body.len + 1, sizeof(char));
      memcpy(json_text, hm->body.ptr, hm->body.len);
      json_text[hm->body.len] = '\0';

      printf("JSON%s\n", json_text);
      cJSON *root = cJSON_Parse(json_text);

      // Verifica se o JSON foi extraído com sucesso
      if (root == NULL) {
        printf("JSON IS NULL: %s\n", &root);
        mg_http_reply(c, 400, "", ""); // Responder com erro 400 se o JSON estiver malformado
        free(json_text);
        return;
      }

      // Extrai valores do JSON usando cJSON
      cJSON *msg = cJSON_GetObjectItem(root, "msg");

      // Verifica se os valores foram extraídos com sucesso
      if (msg == NULL ) {
        printf("MSG IS NULL: %s\n", &msg);
        mg_http_reply(c, 400, "", ""); // Responder com erro 400 se alguma chave não foi encontrada
        cJSON_Delete(root);
        free(json_text);
        return;
      }

      // Cria uma resposta JSON
      cJSON *response_root = cJSON_CreateObject();
      cJSON_AddItemToObject(response_root, "status", cJSON_CreateString("ok"));
      cJSON_AddItemToObject(response_root, "msg", cJSON_CreateString(msg->valuestring));
      char *response_text = cJSON_PrintUnformatted(response_root);

      printf("response_text: %s", response_text);

      // Envia a resposta
      mg_http_reply(c, 200, "Content-Type: application/json", response_text);

      // Libera a memória alocada
      cJSON_Delete(response_root);
      cJSON_Delete(root);
      free(json_text);
      free(response_text);

      // char *str_value = mg_json_get_str(json, path); // Chame a função para obter a string JSON
      // if (str_value != NULL) {
      //   printf("Valor da string JSON: %s\n", str_value);

      //    // Success! create a JSON response
      //   mg_http_reply(c, 200, "Content-Type: application/json\r\n", "{%s:%s}\n",
      //                 "result", msg);

      //   free(str_value); // Não se esqueça de liberar a memória alocada pela função
      // } else {
      //   printf("A chave %s não foi encontrada na string JSON\n", path);
      // }
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
