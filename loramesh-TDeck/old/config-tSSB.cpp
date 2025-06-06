// config-tSSB.cpp

// tinySSB for ESP32
// (c) 2023 <christian.tschudin@unibas.ch>

#include "config.h"

#include <sodium/crypto_auth.h>

#define LORA_TX_POWER  10 // 20 
#define LORA_tSSB_SYNC 0x58  // for "SB, Scuttlebutt". see https://forum.lora-developers.semtech.com/t/sx1272-and-sx1262-lora-sync-word-compatibility/988
// #define LORA_tSSB_SYNC 0x5484  // for "SB, Scuttlebutt". see https://forum.lora-developers.semtech.com/t/sx1272-and-sx1262-lora-sync-word-compatibility/988

struct lora_config_s lora_configs[] = {
  // FIXME: these values are copying TNN values - we should step around these
  {"AU915.a", 917500000, 500000, 8, 5, LORA_tSSB_SYNC, LORA_TX_POWER},
  {"AU915.b", 917500000, 125000, 7, 5, LORA_tSSB_SYNC, LORA_TX_POWER},
  {"EU868.a", 868300000, 250000, 7, 5, LORA_tSSB_SYNC, LORA_TX_POWER},
  {"EU868.b", 868300000, 125000, 7, 5, LORA_tSSB_SYNC, LORA_TX_POWER},
  {"US915.a", 904600000, 500000, 8, 5, LORA_tSSB_SYNC, LORA_TX_POWER},
  {"US915.b", 904600000, 125000, 7, 5, LORA_tSSB_SYNC, LORA_TX_POWER}
};

short lora_configs_size = sizeof(lora_configs) / sizeof(struct lora_config_s);

// ---------------------------------------------------------------------------

struct bipf_s* config_load() // returns a BIPF dict with the persisted config dict
{
  File f = MyFS.open(CONFIG_FILENAME, FILE_READ, false);

  if (f == NULL) { // define some defaults
    struct bipf_s *dict = bipf_mkDict();
    
    bipf_dict_set(dict, bipf_mkString("bubbles"),
                  bipf_mkList()); // empty list of bubble publ keys

    bipf_dict_set(dict, bipf_mkString("lora_plan"),
                  bipf_mkString("AU915.b"));

    unsigned char key[crypto_auth_hmacsha512_KEYBYTES]; // 48B
    memset(key, 1, crypto_auth_hmacsha512_KEYBYTES);    // default is #0101...
    bipf_dict_set(dict, bipf_mkString("mgmt_sign_key"),
                  bipf_mkBytes(key, crypto_auth_hmacsha512_KEYBYTES));

    config_save(dict);
    return dict;
  }

  int len = f.size();
  unsigned char *buf = (unsigned char*) malloc(len);
  f.read(buf, len);
  f.close();
  struct bipf_s *dict = bipf_loads(buf, len);
  free(buf);

  bipf_dict_set(dict, bipf_mkString("lora_plan"),
                bipf_mkString("AU915.b"));

  return dict;
}

void config_save(struct bipf_s *dict) // persist the BIPF dict
{
  // FIXME: we should not print the mgmt signing key to the console
  // Serial.printf("storing config %s\r\n", bipf2String(dict).c_str());
  int len = bipf_encodingLength(dict);
  unsigned char *buf = (unsigned char*) malloc(len);
  if (!buf) {
    Serial.printf("malloc failed\n");
    return;
  }
  bipf_encode(buf, dict);
  File f = MyFS.open(CONFIG_FILENAME, FILE_WRITE, true);
  f.write(buf, len);
  f.close();
  free(buf);
}

// eof
