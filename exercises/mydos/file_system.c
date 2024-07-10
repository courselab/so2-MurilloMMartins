#include "file_system.h"

fs_header* get_fs_header(unsigned int *address) {
  return (fs_header*)address;
}

void load_disk_into_memory(int starting_sector, int sectors_to_read, void *target_address) {
  int sector = (starting_sector - 1) % NUM_SECTORS + 1;
  int head = ((starting_sector - 1) / NUM_SECTORS) % NUM_HEADS;
  int cylinder = (starting_sector - 1) / (NUM_HEADS * NUM_SECTORS);

  __asm__ volatile(
      "pusha \n"
      "mov boot_drive, %%dl \n"    /* Select the boot drive (from rt0.o). */
      "mov $0x2, %%ah \n"          /* BIOS disk service: op. read sector. */
      "mov %[sectToRead], %%al \n" /* How many sectors to read          */
      "mov %[cylCoord], %%ch \n"   /* Cylinder coordinate (starts at 0).  */
      "mov %[sectCoord], %%cl \n"  /* Sector coordinate   (starts at 1).  */
      "mov %[headCoord], %%dh \n"  /* Head coordinate     (starts at 0).      */
      "mov %[targetAddr], %%bx \n" /* Where to load the file system (rt0.o).   */
      "int $0x13 \n"               /* Call BIOS disk service 0x13.        */
      "popa \n" ::
      [headCoord] "g"(head),
      [sectCoord] "g"(sector),
      [cylCoord] "g"(cylinder),
      [sectToRead] "g"(sectors_to_read),
      [targetAddr] "g"(target_address));
}