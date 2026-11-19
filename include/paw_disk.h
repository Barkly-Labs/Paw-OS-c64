#ifndef PAW_DISK_H
#define PAW_DISK_H

#include <cbm.h>

#define PAW_DISK_DEFAULT_DEVICE 8
#define PAW_DISK_MAX_ENTRIES 18

unsigned char paw_disk_device(void);
unsigned char paw_disk_set_device(unsigned char device);
unsigned char paw_disk_read_status(char *buffer, unsigned char capacity);
unsigned char paw_disk_read_directory(struct cbm_dirent *entries,
                                      unsigned char capacity,
                                      unsigned char *count,
                                      unsigned int *free_blocks,
                                      unsigned char *truncated);
unsigned char paw_disk_command(const char *command);
unsigned char paw_disk_delete(const char *filename);
unsigned char paw_disk_rename(const char *old_name, const char *new_name);
unsigned char paw_disk_open_seq(const char *filename);
int paw_disk_read_seq(void *buffer, unsigned int size);
void paw_disk_close_seq(void);
unsigned char paw_disk_filename_valid(const char *filename);
const char *paw_disk_last_status(void);
unsigned char paw_disk_last_code(void);

#endif
