// cmd.cpp

// tinySSB for ESP32
// 2022-2023 <christian.tschudin@unibas.ch>

// --------------------------------------------------------------------------

#include "cmd.h"

void listDir(File dir, uint8_t levels)
{
    Serial.printf("Listing directory: %s\r\n", dir.path());

    if (!dir)
        return;
    dir.rewindDirectory();
    File file = dir.openNextFile(FILE_READ);
    int  cnt = 0;
    while (file) {
        cnt++;
        if (file.isDirectory()) {
            Serial.printf("  DIR : %s\r\n", file.name());
            if (levels)
                listDir(file, levels - 1);
        } else
            Serial.printf("  FILE: %s\tSIZE: %d\r\n", file.name(), file.size());
        file.close();
        file = dir.openNextFile(FILE_READ);
    }
    if (cnt == 0)
        Serial.printf("  EMPTY\r\n");
}

// --------------------------------------------------------------------------

void cmd_rx(String cmd) {
  cmd.toLowerCase();
  cmd.trim();
  Serial.printf("CMD %s\r\n\n", cmd.c_str());
  switch(cmd[0]) {

    case '?':
      Serial.println("  ?        help");
#ifdef HAS_LORA
      Serial.println("  c        cycle through the LoRA plans");
#endif
      Serial.println("  d        dump GoSET, DMXT and CHKT");
      Serial.println("  f        list file system");
      Serial.println("  i        pretty print the confIg values");
      Serial.println("  l        list log file");
      Serial.println("  m        empty log and peers file");
      Serial.println("  p        list file with peers data");
      Serial.println("  r        reset this repo to blank");
      Serial.println("  x        reboot");
      break;

#ifdef HAS_LORA
    case 'c':
      {
        the_lora_config++;
        if (the_lora_config - lora_configs >= lora_configs_cnt)
          the_lora_config = lora_configs;
        Serial.printf("LoRA plan now is %s:\r\n", the_lora_config->plan);
        lora_init();
        struct bipf_s *v = bipf_mkString(the_lora_config->plan);
        bipf_dict_set(the_config, bipf_mkString("lora_plan"), v);
        config_save(the_config);
        theUI->refresh();
      }
      break;
#endif

    case 'd': // dump
      // goset_dump(theGOset);
      Serial.println("Installed feeds:");
      for (int i = 0; i < theRepo->rplca_cnt; i++) {
        unsigned char *key = theGOset->get_key(i);
        Serial.printf("  %d %s, next_seq=%d\r\n", i, to_hex(key, 32, 0), theRepo->fid2replica(key)->get_next_seq(NULL));
      }
      Serial.printf("DMX table: (%d entries)\r\n", theDmx->dmxt_cnt);
      for (int i = 0; i < theDmx->dmxt_cnt; i++) {
        unsigned char *dmx_val = theDmx->dmxt[i].dmx;
        Serial.printf("  %s", to_hex(dmx_val, DMX_LEN));
        unsigned char *fid = theDmx->dmxt[i].fid;
        if (fid != NULL) {
          int ndx = theGOset->_key_index(fid);
          if (ndx >= 0)
            Serial.printf("  %d.%d\r\n", ndx, theDmx->dmxt[i].seq);
        } else {
          char *d = "?";
          if (!memcmp(dmx_val, thePeers->peer_dmx_rep, DMX_LEN))
            d = "<PreP>";
          if (!memcmp(dmx_val, thePeers->peer_dmx_req, DMX_LEN))
            d = "<PreQ>";
          if (!memcmp(dmx_val, theDmx->goset_dmx, DMX_LEN))
            d = "<GosX>";
          else if (!memcmp(dmx_val, theDmx->want_dmx, DMX_LEN))
            d = "<DreQ>";
          else if (!memcmp(dmx_val, theDmx->chnk_dmx, DMX_LEN))
            d = "<CreQ>";
          else if (!memcmp(dmx_val, theDmx->mgmt_dmx, DMX_LEN))
            d = "<MGMT>";
          Serial.printf("  %s\r\n", d);
        }
      }
      Serial.printf("CHUNK table: (%d entries)\r\n", theDmx->chkt_cnt);
      for (int i = 0; i < theDmx->chkt_cnt; i++) {
        struct hsh_s *bp = theDmx->chkt + i;
        Serial.printf("  %s ", to_hex(bp->h, HASH_LEN, 0));
        for (struct chain_s *tp = bp->front; tp; tp = tp->next) {
          int ndx = theGOset->_key_index(tp->fid);
          Serial.printf(" %d.%d.%d", ndx, tp->seq, tp->cnr);
        }
        Serial.printf("\r\n");
      }
      break;

    case 'f': { // Directory dump
      Serial.printf("File system: %d total bytes, %d used\r\n",
                    MyFS.totalBytes(), MyFS.usedBytes());
      File root = MyFS.open("/feeds");
      if (root) {
          listDir(root, 3); // FEED_DIR, 2);
          root.close();
      }
      break;
    }

    case 'i': { // config values
      Serial.printf("Config of node %s:\r\n", ssid);
      // FIXME: we should not print the mgmt signing key to the console ?
      String s = bipf2String(the_config, "\r\n", 0);
      Serial.printf("%s\r\n\r\n", s.c_str());
      break;
    }

    case 'l': // list Log file
      lora_log.close();
      lora_log = MyFS.open(LORA_LOG_FILENAME, FILE_READ);
      while (lora_log.available()) {
        Serial.write(lora_log.read());
      }
      lora_log.close();
      lora_log = MyFS.open(LORA_LOG_FILENAME, FILE_APPEND);
      break;

    case 'm': // empty Log file
      lora_log.close();
      lora_log = MyFS.open(LORA_LOG_FILENAME, FILE_WRITE);
      peers_log.close();
      peers_log = MyFS.open(PEERS_DATA_FILENAME, FILE_WRITE);
      break;

    case 'p': // list peer data
      peers_log.close();
      peers_log = MyFS.open(PEERS_DATA_FILENAME, FILE_READ);
      while (peers_log.available()) {
        Serial.write(peers_log.read());
      }
      peers_log.close();
      peers_log = MyFS.open(PEERS_DATA_FILENAME, FILE_APPEND);
      break;

    case 'r': // reset
      theRepo->reset(NULL); // does not return
      Serial.println("reset done");
      break;

    case 'x': // reboot
      Serial.printf(">> reboot cmd\r\n");
      /*
      lora_log.close();
      Serial.println("rebooting ...\n");
      */
      esp_restart();
      break;

    default:
      Serial.println("unknown command");
      break;
  }
  Serial.println();
}

// eof
