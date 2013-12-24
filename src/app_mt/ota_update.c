
#include <ch.h>
#include <hal.h>

#include "ota_update.h"
#include "message.h"
#include "bbmt.pb.h"
#include "web_api.h"
#include "sxfs.h"

#include <stdio.h>


static void
set_state(ota_update_state_t state);

static void
ota_update_dispatch(msg_id_t id, void* msg_data, void* listener_data, void* sub_data);

static void
dispatch_ota_update_start(void);

static void
dispatch_update_check_response(FirmwareUpdateCheckResponse* response);

static void
dispatch_fw_chunk(FirmwareDownloadResponse* update_chunk);


static sxfs_part_t part;
static ota_update_status_t status;


void
ota_update_init()
{
  status.state = OU_IDLE;

  msg_listener_t* l = msg_listener_create("ota_update", 1024, ota_update_dispatch, NULL);

  msg_subscribe(l, MSG_OTAU_CHECK, NULL);
  msg_subscribe(l, MSG_OTAU_START, NULL);
  msg_subscribe(l, MSG_API_FW_UPDATE_CHECK_RESPONSE, NULL);
  msg_subscribe(l, MSG_API_FW_CHUNK, NULL);
}

ota_update_status_t*
ota_update_get_status(void)
{
  return &status;
}

static void
set_state(ota_update_state_t state)
{
  status.state = state;
  msg_send(MSG_OTAU_STATUS, &status);
}

static void
ota_update_dispatch(msg_id_t id, void* msg_data, void* listener_data, void* sub_data)
{
  (void)listener_data;
  (void)sub_data;

  switch (id) {
  case MSG_OTAU_CHECK:
    msg_post(MSG_API_FW_UPDATE_CHECK, NULL);
    set_state(OU_CHECKING);
    break;

  case MSG_OTAU_START:
    dispatch_ota_update_start();
    break;

  case MSG_API_FW_UPDATE_CHECK_RESPONSE:
    dispatch_update_check_response(msg_data);
    break;

  case MSG_API_FW_CHUNK:
    dispatch_fw_chunk(msg_data);
    break;

  default:
    break;
  }
}

static void
dispatch_ota_update_start()
{
  set_state(OU_PREPARING);

  printf("part clear\r\n");

  if (!sxfs_part_clear(SP_OTA_UPDATE_IMG)) {
    printf("part clear failed\r\n");
    set_state(OU_FAILED);
    return;
  }

  if (!sxfs_part_open(&part, SP_OTA_UPDATE_IMG)) {
    printf("part open failed\r\n");
    set_state(OU_FAILED);
    return;
  }

  set_state(OU_STARTING_DOWNLOAD);
  msg_post(MSG_API_FW_DNLD_START, NULL);
}

static void
dispatch_update_check_response(FirmwareUpdateCheckResponse* response)
{
  if (response->update_available) {
    status.update_size = response->binary_size;
    strncpy(status.update_ver, response->version, sizeof(status.update_ver));
    set_state(OU_UPDATE_AVAILABLE);
    printf("update available %s %d\r\n", status.update_ver, response->binary_size);
  }
  else {
    set_state(OU_UPDATE_NOT_AVAILABLE);
    printf("no update available\r\n");
  }
}

static void
dispatch_fw_chunk(FirmwareDownloadResponse* update_chunk)
{
  status.update_downloaded += update_chunk->data.size;

  set_state(OU_DOWNLOADING);

  sxfs_part_write(&part, update_chunk->data.bytes, update_chunk->data.size);

  // if last chunk...
  if (update_chunk->data.size < sizeof(update_chunk->data.bytes)) {
    if (sxfs_part_verify(SP_OTA_UPDATE_IMG)) {
      printf("image verified resetting to apply update...\r\n");

      set_state(OU_COMPLETE);
      msg_send(MSG_SHUTDOWN, NULL);

      chThdSleepSeconds(1);

      NVIC_SystemReset();
    }
    else {
      printf("sxfs verify failed\r\n");
    }
  }
}
