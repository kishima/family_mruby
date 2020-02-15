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

#include "fmruby.h"
#include <stdint.h>

#define LEXER_WORDS_MAX (200)
class FmrbSimpleLineLexer{
public:
    void init();
    void free();
    void set_line(const char* line);
    void move_head();
    void itr_dump(const char** buff,int* len);
private:
    const char* m_string;
    const char* m_head;
    int m_total_words;
    int m_word_pos;
    const char* m_word_p[LEXER_WORDS_MAX];
    unsigned char m_word_len[LEXER_WORDS_MAX];
    int m_dump_stat;

    bool is_delimiter(const char);
    void push_word(const char*,int);
    void push_word_prim(const char*,int);
    const char* last_word_tail();
    int is_highlight(const char* ,int);
};

#define EDITLINE_BLOCK_SIZE (16)

struct EditLine {
  char* text;
  uint16_t length;
  uint16_t flag;
  uint16_t lineno;
  uint16_t buff_size;
  EditLine* prev;
  EditLine* next;

  OVERLOAD_SPI_ALLOCATOR
  EditLine(void);
  EditLine(char*);
  ~EditLine();
  int insert(char c);  
  int insert(uint16_t pos,char c);
  int backdelete(uint16_t pos);
  char* cut(uint16_t start_pos, uint16_t end_pos);
  void clear(void);

private:

};

enum EDIT_STATUS{
  EDIT_NO_ERROR=0,
  EDIT_MEM_ERROR
};

class FmrbMrubyEngine;
class FmrbFileService;

class FmrbEditor : public FmrbTerminalInput {
public:
  OVERLOAD_SPI_ALLOCATOR
  FmrbEditor(fabgl::VGAController* vga,fabgl::Canvas* canvas,fabgl::Terminal* terminal,  FmrbFileService *storage);
  ~FmrbEditor();
  FMRB_RCODE begin(FmrbMrubyEngine*);
  FMRB_RCODE run(char* input_script);
  void reset(void);
  void release_buffer(void);
  char* dump_script(void);

  //for callback
  bool m_code_hightlight;
  void toggle_highlight(void);
  void load_demo_file(int);
  void show_message(const char*,int timeout=0);

private:
  fabgl::VGAController *m_vga;
  fabgl::Canvas *m_canvas;
  fabgl::Terminal *m_term;
  FmrbFileService *m_storage;
  EditLine* m_buff_head;
  int m_height;
  int m_disp_height;
  int m_width;
  int m_disp_width;
  int m_lineno_shift;
  int m_x;
  int m_y;
  int m_disp_head_line;
  int m_total_line;
  FmrbSimpleLineLexer* m_line_lexer_p;
  //enum EDIT_STATUS m_error;
  FmrbMrubyEngine *m_mruby_engine;
  FmrbCanvasConfig *m_canvas_config;

  
  void wait_key(char);
  void load_file();
  void save_file();
  void clear_buffer();

  void load(const char* buf);
  EditLine* load_line(const char* in);
  EditLine* seek_line(int n);

  void update(void);
  void update_lineno(void);
  void draw_line(int disp_y,EditLine* line);

  void move_edit_cursor(int dir);
  void move_cursor(int x,int y);
  void move(int x,int y);
  void move_pos(int line);

  void insert_ch(char c);
  void insert_ret();
  void delete_ch();
  void page_up();
  void page_down();
  void delete_line();

  void print_csr_info(void);

  void clear_screen(void);

  bool quit(void);
  void open_menu(void);
};

