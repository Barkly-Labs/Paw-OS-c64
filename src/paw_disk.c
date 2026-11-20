#include <ctype.h>
#include <string.h>
#include <cbm.h>
#include "paw_disk.h"

#define PAW_DISK_COMMAND_LFN 15
#define PAW_DISK_DATA_LFN 2
#define PAW_DISK_COMMAND_CHANNEL 15

static unsigned char active_device = PAW_DISK_DEFAULT_DEVICE;
static unsigned char last_code = 0;
static char last_status[40] = "73, DRIVE STATUS NOT READ";

static unsigned char parse_status_code(const char *text)
{
    if (text[0] < '0' || text[0] > '9' ||
        text[1] < '0' || text[1] > '9') return 0xFE;
    return (unsigned char)((text[0] - '0') * 10 + (text[1] - '0'));
}

static unsigned char read_status_channel(unsigned char lfn,
                                         char *buffer,
                                         unsigned char capacity)
{
    int count;

    if (capacity == 0) return 0xFF;
    count = cbm_read(lfn, buffer, (unsigned int)(capacity - 1));
    if (count < 0) {
        buffer[0] = '\0';
        return 0xFF;
    }
    if (count >= capacity) count = capacity - 1;
    while (count > 0 && (buffer[count - 1] == 13 || buffer[count - 1] == 10))
        --count;
    buffer[count] = '\0';
    return parse_status_code(buffer);
}

static unsigned char report_open_error(unsigned char error)
{
    last_code = (unsigned char)(0x80 | error);
    if (error == 5) strcpy(last_status, "DEVICE NOT PRESENT");
    else if (error == 4) strcpy(last_status, "FILE NOT FOUND");
    else if (error == 9) strcpy(last_status, "BAD DEVICE NUMBER");
    else strcpy(last_status, "DISK I/O OPEN ERROR");
    return last_code;
}

unsigned char paw_disk_device(void)
{
    return active_device;
}

unsigned char paw_disk_set_device(unsigned char device)
{
    if (device < 8 || device > 11) return 0;
    active_device = device;
    return 1;
}

unsigned char paw_disk_read_status(char *buffer, unsigned char capacity)
{
    unsigned char error;
    unsigned char code;

    error = cbm_open(PAW_DISK_COMMAND_LFN, active_device,
                     PAW_DISK_COMMAND_CHANNEL, "");
    if (error != 0) {
        report_open_error(error);
        if (capacity > 0) {
            strncpy(buffer, last_status, capacity - 1);
            buffer[capacity - 1] = '\0';
        }
        return last_code;
    }

    code = read_status_channel(PAW_DISK_COMMAND_LFN, last_status,
                               sizeof(last_status));
    cbm_close(PAW_DISK_COMMAND_LFN);
    last_code = code;

    if (capacity > 0) {
        strncpy(buffer, last_status, capacity - 1);
        buffer[capacity - 1] = '\0';
    }
    return code;
}

unsigned char paw_disk_read_directory(struct cbm_dirent *entries,
                                      unsigned char capacity,
                                      unsigned char *count,
                                      unsigned int *free_blocks,
                                      unsigned char *truncated)
{
    struct cbm_dirent entry;
    unsigned char error;
    unsigned char result;
    unsigned char total;

    *count = 0;
    *free_blocks = 0;
    *truncated = 0;
    error = cbm_opendir(PAW_DISK_DATA_LFN, active_device);
    if (error != 0) return report_open_error(error);

    total = 0;
    result = 0;
    while (1) {
        error = cbm_readdir(PAW_DISK_DATA_LFN, &entry);
        if (error == 0) {
            if (entry.type != CBM_T_HEADER) {
                if (total < capacity) {
                    entries[total] = entry;
                    ++total;
                } else {
                    *truncated = 1;
                }
            }
        } else if (error == 2) {
            *free_blocks = entry.size;
            break;
        } else {
            result = (unsigned char)(0x40 | error);
            break;
        }
    }
    cbm_closedir(PAW_DISK_DATA_LFN);
    *count = total;

    if (result != 0) {
        last_code = result;
        strcpy(last_status, "DIRECTORY READ ERROR");
        return result;
    }
    last_code = 0;
    strcpy(last_status, "DIRECTORY READY");
    return 0;
}

unsigned char paw_disk_command(const char *command)
{
    unsigned char error;
    unsigned char code;

    error = cbm_open(PAW_DISK_COMMAND_LFN, active_device,
                     PAW_DISK_COMMAND_CHANNEL, command);
    if (error != 0) return report_open_error(error);

    code = read_status_channel(PAW_DISK_COMMAND_LFN, last_status,
                               sizeof(last_status));
    cbm_close(PAW_DISK_COMMAND_LFN);
    last_code = code;
    return code;
}

unsigned char paw_disk_filename_valid(const char *filename)
{
    unsigned char length;
    unsigned char ch;
    unsigned char last_was_space;

    if (filename == 0 || *filename == '\0') return 0;
    length = 0;
    last_was_space = 0;
    while (*filename != '\0') {
        ch = (unsigned char)toupper((unsigned char)*filename);
        if (!((ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9') ||
              ch == '_' || ch == '-' || ch == '.' || ch == ' ')) return 0;
        if (ch == ' ' && length == 0) return 0;
        if (++length > 16) return 0;
        last_was_space = (unsigned char)(ch == ' ');
        ++filename;
    }
    return (unsigned char)!last_was_space;
}

unsigned char paw_disk_delete(const char *filename)
{
    char command[23];
    unsigned char length;

    if (!paw_disk_filename_valid(filename)) {
        last_code = 0xFD;
        strcpy(last_status, "UNSUPPORTED FILE NAME");
        return last_code;
    }
    command[0] = 'S';
    command[1] = '0';
    command[2] = ':';
    length = 3;
    while (*filename != '\0') command[length++] = *filename++;
    command[length] = '\0';
    return paw_disk_command(command);
}

unsigned char paw_disk_rename(const char *old_name, const char *new_name)
{
    char command[40];
    unsigned char length;

    if (!paw_disk_filename_valid(old_name) ||
        !paw_disk_filename_valid(new_name)) {
        last_code = 0xFD;
        strcpy(last_status, "UNSUPPORTED FILE NAME");
        return last_code;
    }
    command[0] = 'R';
    command[1] = '0';
    command[2] = ':';
    length = 3;
    while (*new_name != '\0') command[length++] = *new_name++;
    command[length++] = '=';
    while (*old_name != '\0') command[length++] = *old_name++;
    command[length] = '\0';
    return paw_disk_command(command);
}

unsigned char paw_disk_open_seq(const char *filename)
{
    unsigned char error;
    if (!paw_disk_filename_valid(filename)) {
        last_code = 0xFD;
        strcpy(last_status, "UNSUPPORTED FILE NAME");
        return last_code;
    }
    error = cbm_open(PAW_DISK_DATA_LFN, active_device, CBM_SEQ, filename);
    if (error != 0) return report_open_error(error);
    return 0;
}

int paw_disk_read_seq(void *buffer, unsigned int size)
{
    return cbm_read(PAW_DISK_DATA_LFN, buffer, size);
}

void paw_disk_close_seq(void)
{
    cbm_close(PAW_DISK_DATA_LFN);
}

unsigned char paw_disk_open_prg(const char *filename)
{
    unsigned char error;

    if (!paw_disk_filename_valid(filename)) {
        last_code = 0xFD;
        strcpy(last_status, "UNSUPPORTED FILE NAME");
        return last_code;
    }

    error = cbm_open(PAW_DISK_DATA_LFN, active_device, CBM_READ, filename);
    if (error != 0) return report_open_error(error);
    return 0;
}

int paw_disk_read_prg(void *buffer, unsigned int size)
{
    return cbm_read(PAW_DISK_DATA_LFN, buffer, size);
}

void paw_disk_close_prg(void)
{
    cbm_close(PAW_DISK_DATA_LFN);
}

const char *paw_disk_last_status(void)
{
    return last_status;
}

unsigned char paw_disk_last_code(void)
{
    return last_code;
}
