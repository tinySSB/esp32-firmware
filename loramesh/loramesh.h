// loramesh.h

#include "src/lib/tinySSBlib.h"
#include <esp_mac.h>

#include "src/ui-heltec.h"
#include "src/ui-t5gray.h"
#include "src/ui-t5s3pro.h"
#include "src/ui-tbeam.h"
#include "src/ui-tdeck.h"
#include "src/ui-twatch.h"
#include "src/ui-twrist.h"
#include "src/ui-wlpaper.h"

#include "src/hardware.h"

#if !defined(UTC_OFFSET)
# define UTC_OFFSET ""
#endif

#if !defined(UTC_COMPILE_TIME)
# define UTC_COMPILE_TIME __DATE__ " " __TIME__ UTC_OFFSET
#endif

// include files with actual code and data structures:
#include "src/lib/node.h"
#if defined(HAS_BT)
# include "src/lib/kiss.h"
#endif

// --- global variables

struct bipf_s *the_config;

char *utc_compile_time;
unsigned char my_mac[6];
char ssid[sizeof(tSSB_WIFI_SSID) + 6];

DmxClass   *theDmx;
UIClass    *theUI;
Repo2Class *theRepo;
GOsetClass *theGOset;
PeersClass *thePeers;
SchedClass *theSched;

#if defined(TINYSSB_BOARD_T5S3PRO)
App_TAV_Class *theTAV;
#endif

// ---------------------------------------------------------------------------

void theGOset_rx(unsigned char *pkt, int len, unsigned char *aux,
                 struct face_s *f)
{
  theGOset->rx(pkt, len, aux, f);
}

void probe_for_goset_vect(unsigned char **pkt,
                          unsigned short *len,
                          unsigned short *reprobe_in_millis,
                          const char **origin)
{
  theGOset->probe_for_goset_vect(pkt, len, reprobe_in_millis, origin);
}

void probe_for_peers_beacon(unsigned char **pkt,
                            unsigned short *len,
                            unsigned short *reprobe_in_millis,
                            const char **origin)
{
  thePeers->probe_for_peers_beacon(pkt, len, reprobe_in_millis, origin);
}

// ---------------------------------------------------------------------------

File lora_log;
File peers_log;

long next_lora_log;
long next_serial_ts;
#define LORA_LOG_INTERVAL  (5*60*1000) // every 5 minutes
#define SERIAL_TS_INTERVAL (15*1000)   // every 15 sec

char* _ts()
{
  static char buf[40];

#ifdef HAS_GPS
  if (gps.date.isValid() && gps.date.year() != 2000) {
    sprintf(buf, "%04d-%02d-%02dT%02d:%02d:%02dZ",
            gps.date.year(), gps.date.month(), gps.date.day(),
            gps.time.hour(), gps.time.minute(), gps.time.second());
    return buf;
  }
#endif
  long now = millis();
  sprintf(buf, "%d.%03d", now/1000, now%1000);
  return buf;
}

void _wr(File f, char *fmt, va_list ap)
{
  char buf[250];
  vsprintf(buf, fmt, ap);
  f.printf("%s %s\r\n", _ts(), buf);
}

void lora_log_wr(char *fmt, ...)
{
  if (!lora_log) return;
  va_list args; va_start(args, fmt); _wr(lora_log, fmt, args); va_end(args);
}

void peers_log_wr(char *fmt, ...)
{
  if (!peers_log) return;
  va_list args; va_start(args, fmt); _wr(peers_log, fmt, args); va_end(args);
}

void time_stamp()
{
  if (next_lora_log < millis()) {
    lora_log_wr("F=%d E=%d C=%d |dmxt|=%d |chkt|=%d |heap|=%d",
                theRepo->rplca_cnt, theRepo->entry_cnt, theRepo->chunk_cnt,
                theDmx->dmxt_cnt, theDmx->chkt_cnt, ESP.getFreeHeap()
                /* ,lora_sent_pkts, lora_rcvd_pkts, lora_pps */);
    next_lora_log = millis() + LORA_LOG_INTERVAL;
  }
  
  if (next_serial_ts < millis()) {
    Serial.printf("-- %s F=%d E=%d C=%d |dmxt|=%d |chkt|=%d |heap|=%d\r\n",
                  _ts(),
                  theRepo->rplca_cnt, theRepo->entry_cnt, theRepo->chunk_cnt,
                  theDmx->dmxt_cnt, theDmx->chkt_cnt, ESP.getFreeHeap()
                  /* ,lora_sent_pkts, lora_rcvd_pkts, lora_pps */);

    next_serial_ts = millis() + SERIAL_TS_INTERVAL;
  }
}

// ---------------------------------------------------------------------------

#if defined(TINYSSB_BOARD_TWATCH) || defined(TINYSSB_BOARD_T5S3PRO)
static void cb_new_log_entry(unsigned char *fid, int seqnr)
{
  Serial.printf("# cb_new_log_entry %s:%d\r\n", to_hex(fid,10), seqnr);
  ReplicaClass *r = theRepo->fid2replica(fid);
  if (r) {
    int max_sz;
    int sz = r->get_content_len(seqnr, &max_sz);
    Serial.printf("#   sz=%d max_sz=%d\r\n", sz, max_sz);
    if (sz > 0 && sz < 2024 && max_sz == sz) { // only consider full entries
      unsigned char *buf = r->read(seqnr, &max_sz);
      if (buf != NULL) {
        struct bipf_s *b = bipf_loads(buf, max_sz);
        if (b != NULL) {
          // Serial.printf("#   %s\r\n", bipf2String(b));
          theTAV->incoming(fid, b);
          bipf_free(b);
        }
        free(buf);
      }
    }
  }
}
#endif

// ---------------------------------------------------------------------------

int esp_efuse_mac_get_default(uint8_t *mac);

void setup()
{
  char *msg;

  Serial.begin(115200);
  delay(10);

  utc_compile_time = (char*) malloc(strlen(UTC_COMPILE_TIME) + 1);
  strcpy(utc_compile_time, UTC_COMPILE_TIME);
  for (char *cp = utc_compile_time; *cp; cp++)
    if (*cp == '_') *cp = ' ';

  Serial.println();
  Serial.println("** Welcome to the tinySSB virtual pub "
                 "running on a " DEVICE_MAKE);
  // Serial.println("** compiled " __DATE__ ", " __TIME__ UTC_OFFSET);
  Serial.printf("** compiled %s\r\n", utc_compile_time);

  esp_efuse_mac_get_default(my_mac);
#if !defined(HAS_UDP)  // in case of no Wifi: display BT mac addr, instead
  my_mac[5] += 2;
  // https://docs.espressif.com/projects/esp-idf/en/release-v3.0/api-reference/system/base_mac_address.html
#endif
  
  // #if defined(HAS_UDP) || defined(HAS_BT) || defined(HAS_BLE)
  sprintf(ssid, "%s-%s", tSSB_WIFI_SSID, to_hex(my_mac+4, 2));
  Serial.printf("** this is node %s\r\n\r\n", ssid);
  // #endif
  delay(1000);

  hw_init();

  Serial.println("after hw_init");
  // delay(3500);

#if defined(TINYSSB_BOARD_HELTEC) || defined(TINYSSB_BOARD_HELTEC3)
  theUI    = new UI_Heltec_Class();
#elif defined(TINYSSB_BOARD_T5GRAY)
  theUI    = new UI_T5gray_Class();
#elif defined(TINYSSB_BOARD_T5S3PRO)
  theUI    = new UI_T5s3pro_Class();
#elif defined(TINYSSB_BOARD_TBEAM)
  theUI    = new UI_TBeam_Class();
#elif defined(TINYSSB_BOARD_TDECK)
  theUI    = new UI_TDeck_Class();
#elif defined(TINYSSB_BOARD_TWATCH)
  theUI    = new UI_TWatch_Class();
#elif defined(TINYSSB_BOARD_TWRIST)
  theUI    = new UI_TWrist_Class();
#elif defined(TINYSSB_BOARD_WLPAPER)
  theUI    = new UI_WLpaper_Class();
#endif
  // theUI->show_node_name(ssid);
  // theUI->spinner(true);
  // theUI->show_boot_msg("mounting file system");

  // delay(500);

  // --- file system

  if (!MyFS.begin(true, "/littlefs", 20))
    msg = "could not mount file system, partition was reformatted";
  else
    msg = "file system was mounted";
  theUI->show_boot_msg(msg);
  Serial.println(msg);

  // MyFS.format(); // uncomment and run once after a change in partition size

  MyFS.mkdir(FEED_DIR);
  Serial.printf("file system: %d total bytes, %d used\r\n",
                MyFS.totalBytes(), MyFS.usedBytes());

  // --- tinySSB config

  theUI->show_boot_msg("load config");
  the_config = config_load();

  // FIXME: we should not print the mgmt signing key to the console ?
  Serial.printf("tinySSB config is %s\r\n",
                bipf2String(the_config, "\r\n", 1).c_str());
  Serial.println();

  theUI->show_boot_msg("setup log files");
  msg = "** starting";
  lora_log = MyFS.open(LORA_LOG_FILENAME, FILE_APPEND);
  lora_log_wr(msg);
  // peers_log = MyFS.open(PEERS_DATA_FILENAME, FILE_APPEND);
  // peers_log_wr(msg);

  // --- IO init
  
#ifdef USE_RADIO_LIB
  int state = radio.begin();
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println("RadioLib begin() success!");
  } else {
    Serial.print(F("RadioLib begin() failed, code "));
    Serial.println(state);
  }
#endif

#ifdef USE_LORA_LIB
  theUI->show_boot_msg("init LoRa");
  SPI.begin(SCK,MISO,MOSI,SS);
  LoRa.setPins(SS,RST,DI0);  
  if (!LoRa.begin(the_lora_config->fr)) {
    msg = "  LoRa init failed!";
    theUI->show_boot_msg(msg);
    while (1) {
      Serial.println(msg);
      delay(2000);
    }
  }
  // LoRa.setTxPower(the_lora_config->tx, RF_PACONFIG_PASELECT_PABOOST);
#endif

  theUI->show_boot_msg("init BLE, WIFI");
  io_init(); // network interfaces

  theDmx   = new DmxClass();
  theRepo  = new Repo2Class();
  theGOset = new GOsetClass();
  thePeers = new PeersClass();
  theSched = new SchedClass(probe_for_goset_vect,
                            probe_for_peers_beacon,
                            probe_for_want_vect,
                            probe_for_chnk_vect);
  {
    unsigned char h[32];
    crypto_hash_sha256(h, (unsigned char*) GOSET_DMX_STR, strlen(GOSET_DMX_STR));
    memcpy(theDmx->goset_dmx, h, DMX_LEN);
    theDmx->arm_dmx(theDmx->goset_dmx, theGOset_rx, NULL);
    Serial.printf("   DMX for GosX is %s\r\n", to_hex(theDmx->goset_dmx, 7, 0));
  }

  theUI->show_boot_msg("load feed data ...");
  theRepo->load();


#if defined(TINYSSB_BOARD_TWATCH) || defined(TINYSSB_BOARD_T5S3PRO)
#if defined(TINYSSB_BOARD_TWATCH)
    UI_TWatch_Class *ui  = (UI_TWatch_Class*) theUI;
#else
    UI_T5s3pro_Class *ui = (UI_T5s3pro_Class*) theUI;
#endif
  // if we have an ED25519 key pair in the NVS, make sure we have a feed
  if ( ui->myid_valid ) {
    unsigned char *fid = ui->myid_pk;
    ReplicaClass *r = theRepo->fid2replica(fid);
    if (!r) {
      Serial.printf("creating new replica for %s\r\n",
                    to_hex(ui->myid_pk,32));
      theRepo->add_replica(fid);
      theGOset->populate(fid);
      theGOset->populate(NULL); // triggers sorting, and setting the want_dmx
      r = theRepo->fid2replica(fid);

      // announce our name
      struct bipf_s *lst = bipf_mkList();
      bipf_list_append(lst, bipf_mkString("IAM"));
#if defined(TINYSSB_BOARD_TWATCH)
      bipf_list_append(lst, bipf_mkString("cft's SSB.watch"));
#elif defined(TINYSSB_BOARD_T5S3PRO)
      bipf_list_append(lst, bipf_mkString("cft's T5s3pro"));
#endif
      int len = bipf_encodingLength(lst);
      unsigned char *buf = (unsigned char *) malloc(len);
      bipf_encode(buf, lst);
      bool rc = r->write(buf, len, ui->my_signing_fct);
      bipf_free(lst);
      free(buf);
    }
    // else if (r->get_next_seq() > 25) // purge, for dev purposes
    //   theRepo->reset(FEED_DIR);
  }
#endif

  Serial.printf("\r\n   Repo: %d feeds, %d entries, %d chunks\r\n",
                theRepo->rplca_cnt, theRepo->entry_cnt, theRepo->chunk_cnt);
  // listDir(MyFS, "/", 2); // FEED_DIR, 2);

  theUI->show_boot_msg("load app data ...");
#if defined(TINYSSB_BOARD_TWATCH) || defined(TINYSSB_BOARD_T5S3PRO)
  Serial.println("# restream all logs (no persisted TAV state, yet)");
  theTAV = new App_TAV_Class();
  theTAV->restream();
#endif
  theUI->spinner(false);
  theUI->buzz();

  msg = "end of setup\n";
  theUI->show_boot_msg(msg);
  Serial.println(msg);
  delay(1000);

  theUI->boot_ended();
  theUI->refresh();
}

char mute_io = 0;
uint64_t next_refresh;

void loop()
{
    if (!mute_io) {
        io_loop();        // check for received packets
        io_proc();        // process received packets
        theSched->tick(); // send pending packets
    }

    theUI->loop();

    if (Serial.available())
        cmd_rx(Serial.readString());

    time_stamp();
    delay(5);

    /*
    if (millis() > next_refresh) {
        epd_clear();
        theUI->refresh();
        next_refresh = millis() + 5000;
    }
    */
}

// eof
