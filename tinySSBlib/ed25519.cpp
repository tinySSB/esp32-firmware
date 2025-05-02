// tinySSBlib/ed25519.cpp

// (c) 2025 <christian.tschudin@unibas.ch>

#include <esp_random.h>

#include "ArduinoNvs.h"
#include "ed25519.h"
#include "util.h"

bool ed25519_new_seed(char *nm)
{
  unsigned char seed[ED25519_SEED_LEN];
  esp_fill_random(seed, sizeof(seed));
  return NVS.setBlob(nm, seed, ED25519_SEED_LEN, true);
}

bool ed25519_del_seed(char *nm)
{
  return NVS.erase(nm, true);
}

bool ed25519_get_seed(char *nm, unsigned char *seed)
{
  return NVS.getBlob(nm, seed, ED25519_SEED_LEN);
}

bool ed25519_get_keypair(char *nm, unsigned char *pk, unsigned char *sk)
{
  unsigned char seed[ED25519_SEED_LEN];

  if (!NVS.getBlob(nm, seed, sizeof(seed)))
    return false;
  crypto_sign_ed25519_seed_keypair(pk, sk, seed);
  return true;
}

// eof
