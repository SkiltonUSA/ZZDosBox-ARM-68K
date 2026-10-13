/* GPL-2.0-or-later. PC device/frontend integration. */
#pragma once
#include <stdint.h>
void db_pc_init();void db_pc_close();void db_pc_tick();
void db_video_reset();
extern unsigned db_width,db_height,db_frames,db_bpp,db_lines;
extern uint32_t db_pixels[1024*768];

extern unsigned db_memory_mb,db_machine_option,db_cpu_cycles;

extern char db_boot_name[13],db_hdd_name[13];void db_boot_start();void db_boot_close();

extern bool db_vga_console;
extern unsigned db_text_attr;
