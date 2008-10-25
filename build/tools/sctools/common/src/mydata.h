#ifndef	_MY_DATA_H_
#define	_MY_DATA_H_


#define MY_DATA_VERSION_MAJOR 0
#define MY_DATA_VERSION_MINOR 1

typedef struct {
  u8 version_major;
  u8 version_minor;
  RTCDate rtc_date;
  RTCTime rtc_time;
  BOOL shop_record_flag;
  int num_of_user_download_app;
  int num_of_app_save_data;
  int num_of_photo_files;
  int num_of_shared2_files;
  u8 movableUniqueID[ LCFG_TWL_HWINFO_MOVABLE_UNIQUE_ID_LEN ]; // 移行可能なユニークID 16byte
  u32 deviceId;
  u8 region;
  u64 fuseId;
  char bmsDeviceId[32];
  u32 reserve[256];
} MyData;


#ifdef __cplusplus
extern "C" {
#endif



#ifdef __cplusplus
}
#endif


#endif /* _MY_DATA_H_ */
