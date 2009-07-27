#ifndef	_MY_DATA_H_
#define	_MY_DATA_H_


#define MY_DATA_VERSION_MAJOR 1
#define MY_DATA_VERSION_MINOR 6

typedef struct {
  u8 version_major;
  u8 version_minor;
  BOOL sys_ver_flag;
  u16 sys_ver_major;
  u16 sys_ver_minor;
  BOOL rtc_date_flag;
  BOOL rtc_time_flag;
  RTCDate rtc_date;
  RTCTime rtc_time;
  BOOL wireless_lan_param_flag;
  BOOL user_settings_flag;
  BOOL shop_record_flag;
  int num_of_user_download_app;
  int num_of_user_pre_installed_app;
  int num_of_user_pre_installed_eticket_only;
  int num_of_app_save_data;
  int num_of_photo_files;
  int num_of_shared2_files;

  int num_of_error_user_download_app; /* unused */
  int num_of_error_app_save_data;
  int num_of_error_photo_files;
  int num_of_error_shared2_files;

  BOOL uniqueid_flag;
  u8 movableUniqueID[ LCFG_TWL_HWINFO_MOVABLE_UNIQUE_ID_LEN ]; // 移行可能なユニークID 16byte
  u32 deviceId;
  u8 region;
  u64 fuseId;
  char bmsDeviceId[32];
  s32 volume;
  s32 backlight_brightness;
  u32 reserve[256];
} MyData;


#ifdef __cplusplus
extern "C" {
#endif



#ifdef __cplusplus
}
#endif


#endif /* _MY_DATA_H_ */
