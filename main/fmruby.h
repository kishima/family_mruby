/*
 Created by Katsuhiko KAGEYAMA(@kishima) - <https://silentworlds.info>
 Copyright (c) 2019-2020 Katsuhiko KAGEYAMA.
 All rights reserved.

 This file is part of Family mruby.

 Family mruby is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Family mruby is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with Family mruby.  If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once

#include "fabgl.h"
#include "fabutils.h"
#include "fmruby_virtual_key.h"

#define FMRB_VERSION "0.6.1"
#define FMRB_RELEASE "2020/02/12"

#define FMRB_MAIN_TASK_PRIORITY 1
#define FMRB_UART_TASK_PRIORITY 2

#define FMRB_MAIN_TASK_STACK_SIZE (1024*16)
#define FMRB_UART_TASK_STACK_SIZE (1024*2)

// public function
void* fmrb_spi_malloc(size_t size);
void* fmrb_spi_realloc(void* ptr, size_t size);
void fmrb_free(void* ptr);

#define OVERLOAD_SPI_ALLOCATOR \
  void* operator new(std::size_t size){\
  void* ptr = fmrb_spi_malloc(size);\
    if(ptr == nullptr) { throw std::bad_alloc(); }\
    return ptr; }\
  void operator delete(void* ptr){ fmrb_free(ptr); }

#define FMRB_BITMAP_HEADER_SIZE (4)

#define ENABLE_FMRB_LOG

enum class FMRB_LOG{
  DEBUG=0,
  MSG,
  INFO,
  ERR,
  RAW,
};

enum class FMRB_RCODE:int{
  DEVICE_ERROR=-100,
  NOTFOUND_ERROR,
  NOTREADY_ERROR,
  MEMALLOC_ERROR,
  ERROR=-1,
  OK=0,
  OK_DONE,
  OK_CONTINUE,
  OK_EMPTY,
};


class FmrbTerminalInput{
public:
  FmrbTerminalInput(fabgl::Terminal *t);
  ~FmrbTerminalInput();
  fabgl::Terminal *m_terminal;

  OVERLOAD_SPI_ALLOCATOR
  FmrbVkey read_vkey(int timeout=-1);
  void wait_vkey(FmrbVkey k);
  void wait_vkey(FmrbVkey target,int timeout);

  static bool is_visible(FmrbVkey k);
  static char to_ascii(FmrbVkey k);
  static FmrbVkey ascii_to_vkey(unsigned char c);

};

struct FmrbCanvasConfig {
  RGB888 fg; //pen color
  RGB888 bg; //brush color
  bool fill_bg;
  int font_width;
  int font_height;

  OVERLOAD_SPI_ALLOCATOR
  FmrbCanvasConfig(RGB888 fg,RGB888 bg,bool fillbg, int font_width,int font_height);
  ~FmrbCanvasConfig();
  void set(fabgl::Canvas*);
};

//debug
void fmrb_debug_print(FMRB_LOG lv,const char *fmt,const char* func,int line,...);
void fmrb_dump_mem_stat(int diff=0);

#ifdef ENABLE_FMRB_LOG
  #define FMRB_DEBUG(lv,fmt, ...) fmrb_debug_print(lv,fmt, __func__ , __LINE__ , ##__VA_ARGS__ )
#else
  #define FMRB_DEBUG(...)
#endif
