#ifndef	_ERROR_REPORT_H_
#define	_ERROR_REPORT_H_


#ifdef __cplusplus
extern "C" {
#endif

void Error_Report_Init(void);
void Error_Report_End(void);

BOOL Error_Report_Add(char *str);
BOOL Error_Report_Printf(const char *fmt, ...);

BOOL Error_Report_Display(TEXT_CTRL *tc);

#ifdef __cplusplus
}
#endif


#endif /* _ERROR_REPORT_H_ */
