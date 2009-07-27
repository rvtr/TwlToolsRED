#ifndef	_MYIMPORT_H_
#define	_MYIMPORT_H_

#ifdef __cplusplus
extern "C" {
#endif

BOOL myImportTad(char* file_name, int org_version, FSFile *log_fd);
BOOL myImportTad_sign(char* full_path, int org_version, FSFile *log_fd);
BOOL myDeleteTitle(u64 tid, BOOL with_ticket, FSFile *log_fd);
BOOL my_NAM_ImportTadTicketOnly(const char* path);
BOOL my_NAM_ImportTadTicketOnly_sign(const char* path);

#ifdef __cplusplus
}
#endif


#endif /* _MYIMPORT_H_ */
