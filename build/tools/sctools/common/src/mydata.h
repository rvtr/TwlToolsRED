#ifndef	_MY_DATA_H_
#define	_MY_DATA_H_


typedef struct {
  RTCDate rtc_date;
  RTCTime rtc_time;
  BOOL shop_record_flag;
  int num_of_user_download_app;
  int num_of_app_save_data;
  int num_of_photo_files;
  int num_of_shared2_files;
  u8 movableUniqueID[ LCFG_TWL_HWINFO_MOVABLE_UNIQUE_ID_LEN ]; // 移行可能なユニークID 16byte
} MyData;

#ifdef __cplusplus
extern "C" {
#endif



#ifdef __cplusplus
}
#endif


#endif /* _MY_DATA_H_ */
