#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#define FS_SIGNATURE "\xeb\xety"  /* File system signature.                   */
#define FS_SIGLEN 4               /* Signature length.                        */

#define NUM_HEADS 2
#define NUM_CILINDERS 80
#define NUM_SECTORS 63

#define SECTOR_LEN 512

#define FILE_NAME_LEN 32

#define FS_HEADER_MEMORY_LOCATION 0x7C02

typedef struct fs_header_t
{
  unsigned char  signature[FS_SIGLEN];    /* The file system signature.              */
  unsigned short total_number_of_sectors; /* Number of 512-byte disk blocks.         */
  unsigned short number_of_boot_sectors;  /* Sectors reserved for boot code.         */
  unsigned short number_of_file_entries;  /* Maximum number of files in the disk.    */
  unsigned short max_file_size;		  /* Maximum size of a file in blocks.       */
  unsigned int unused_space;              /* Remaining space less than max_file_size.*/
} __attribute__((packed)) fs_header;      /* Disable alignment to preserve offsets.  */

fs_header* get_fs_header(unsigned int *address);

void load_disk_into_memory(int starting_sector, int sectors_to_read, void *target_address);

#endif