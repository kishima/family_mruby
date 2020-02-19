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

#include <cstdlib>
#include <string>

#include "fmruby.h"
#include "fmruby_editor.h"
#include "mruby.h"

#include "FS.h"

/**
 * File service
 **/
enum class FmrbStorageType{
  NONE,
  SPIFFS,
  SD,
};

#define FMRB_MAX_PATH_LEN (256)
#define FMRB_MAX_DIRLIST_LEN (256)
class FmrbDir{
public:
  FmrbStorageType type;
  int length;
  char* dir_path;

  OVERLOAD_SPI_ALLOCATOR
  FmrbDir(FmrbStorageType);
  ~FmrbDir();
  void set(File *file,const char* dir_name);
  const char* fetch_path(int n);
 
  static const char* type_to_path(FmrbStorageType);
  static bool is_file(const char* path);
  static bool is_dir(const char* path);
  static const char* remove_base_dir(const char* in,const char* base);

private:
  char* path_list[FMRB_MAX_DIRLIST_LEN];
};

class FmrbFileService {
public:
  FmrbFileService();
  FMRB_RCODE init();
  FMRB_RCODE mount_sd();
  void umount_sd();

  char* load(const char* path,uint32_t &fsize,bool is_text=false,bool localmem=false);
  char* load_bitmap(const char* path,uint16_t &width,uint16_t &height,uint32_t &type);
  FmrbDir* get_dir_obj(const char* dir_path);

  FMRB_RCODE save(const uint8_t *buff,const char* path,size_t data_size);

  static FmrbStorageType check_stype_path(const char*);
  static const char* to_data_path(const char* path);

private:
  bool m_spiffs_opened;
  bool m_sd_opened;

  uint64_t m_sd_size; 

  void init_sd_spi();
  FMRB_RCODE mount_spiffs();
  FMRB_RCODE precheck_path(const char* path);

  
};

/**
 * Config
 **/
#define FMRB_CONFIG_KEY_MAX (64)
#define FMRB_MODE_LINE_MAX (256)
#define FMRB_CONFIG_FILE_PATH "/spiffs/config/config.txt"
class FmrbConfig  {
public:
  char main_mode_line[FMRB_MODE_LINE_MAX];
  char mruby_mode_line[FMRB_MODE_LINE_MAX];
  int32_t main_screen_shift_x;
  int32_t main_screen_shift_y;
  int32_t mruby_screen_shift_x;
  int32_t mruby_screen_shift_y;
  int32_t mruby_double_buffer;
  int32_t demo_mode;

  FmrbFileService* m_storage;

  //Code Highlight

  OVERLOAD_SPI_ALLOCATOR
  FmrbConfig();
  void init(FmrbFileService* fs);
  FMRB_RCODE load(void);
  FMRB_RCODE save(void);

private:
  FMRB_RCODE read_value_str(const char* txt,const char* key,char* out);
  FMRB_RCODE read_value_int(const char* txt,const char* key,int32_t* out);
  void set_default_value(void);

};

/**
 * Dialog 
 **/
class FmrbDialog : public FmrbTerminalInput{
public:
  fabgl::VGAController *m_vga;
  fabgl::Canvas *m_canvas;

  OVERLOAD_SPI_ALLOCATOR
  FmrbDialog(fabgl::VGAController *v,fabgl::Canvas* canvas,fabgl::Terminal*,FmrbCanvasConfig*);
  ~FmrbDialog();
  void open_message_dialog(const char* message,int timeout_sec=0);
  bool open_confirmation_dialog(const char* message);
  const char* open_text_select_dialog(const char* list[],int list_length);
  FMRB_RCODE open_file_select_dialog(FmrbFileService*,const char* path,std::string* input);
  FMRB_RCODE open_text_input_dialog(const char* message,std::string* input);

private:
  uint8_t *m_swap_buff;
  uint16_t m_screen_width;
  uint16_t m_screen_height;
  uint16_t m_dialog_width;
  uint16_t m_dialog_height;
  uint16_t m_x;
  uint16_t m_y;
  uint8_t m_line_height;
  uint8_t m_font_width;
  uint8_t m_font_height;
  RGB888 m_fg_color;
  RGB888 m_bg_color1;
  RGB888 m_bg_color2;
  FmrbCanvasConfig *m_canva_config_origin;

  int draw_window(int line);
  void draw_cursor(int pos);

};


/**
 * Menu 
 **/

class FmrbMenuModule;
typedef FMRB_RCODE (*FmrbMenuCallback)(uint32_t fid,FmrbMenuModule* menu);  

enum class FmrbMenuItemType{
  NONE,
  TITLE,
  SELECTABLE,
  UNSELECTABLE,
};

class FmrbMenuItem{
public:
  char* description;
  uint32_t fid;
  FmrbMenuCallback func;
  FmrbMenuItemType type;

  FmrbMenuItem* m_prev;
  FmrbMenuItem* m_next;
  FmrbMenuItem* m_parent;
  FmrbMenuItem* m_child;

//func
  OVERLOAD_SPI_ALLOCATOR
  FmrbMenuItem();
  FmrbMenuItem(char* desc, uint32_t fid,FmrbMenuCallback cfunc,FmrbMenuItemType type);
  ~FmrbMenuItem();

  static FmrbMenuItem* add_item_tail(FmrbMenuItem* target, char* desc, uint32_t fid,FmrbMenuCallback cfunc,FmrbMenuItemType type);
  static FmrbMenuItem* add_child_item(FmrbMenuItem* target, char* desc, uint32_t fid,FmrbMenuCallback cfunc,FmrbMenuItemType type);
  static FmrbMenuItem* retrieve_item(FmrbMenuItem* head_item,int line);
};

class FmrbMenuModule : public FmrbTerminalInput {
public:
  fabgl::VGAController *m_vga;
  fabgl::Canvas *m_canvas;
  fabgl::Terminal *m_terminal;
  FmrbCanvasConfig *m_canvas_config;
  void *m_param;

  OVERLOAD_SPI_ALLOCATOR
  FmrbMenuModule(fabgl::VGAController*, fabgl::Canvas* canvas,fabgl::Terminal* terminal,FmrbMenuItem *item);
  ~FmrbMenuModule();
  void begin(void *param_p);
  void set_storage(FmrbFileService *storage);

private:
  FmrbMenuItem* m_top;
  int m_offset_x;
  int m_offset_y;
  int m_mergin;
  FmrbFileService *m_storage;

  void draw_item(FmrbMenuItem* head_item,int line,bool invert);
  int draw_menu(FmrbMenuItem* head_item);
  void clear_draw_area(void);
  void exec_menu(FmrbMenuItem* head_item);
};


/**
 * FmrbAudio
 **/

class FmrbAudio{
public:
  SoundGenerator* m_generator;
  WaveformGenerator* m_wavegen;
  volatile int play_stat;

  OVERLOAD_SPI_ALLOCATOR
  FmrbAudio();
  ~FmrbAudio();
  void load_mml(const char*);
  void play_mml();
  void stop();

private:
  char* m_mml_str;
  TaskHandle_t m_musicTaskHandle;

  static void musicTask(void * arg);
};


/**
 * mruby engine
 **/
#define FMRB_JOYPAD_MAP_LENGTH (13)
enum class FMRB_JPAD_KEY{
  NONE=0,A,B,X,Y,L,R,START,SELECT,
  UP,DOWN,LEFT,RIGHT,
};

#define FMRB_DBG_MSG_MAX_LEN (1024)
class FmrbMrubyEngine {
public:
  OVERLOAD_SPI_ALLOCATOR
  FmrbMrubyEngine();
  ~FmrbMrubyEngine();
  void run(char* code_string);
  uint8_t *get_joypad_map();
  const char *get_error_msg();
  int get_error_line();
  FMRB_RCODE get_result();
  FmrbAudio *m_sound_engine;

private:
  FMRB_RCODE m_exec_result;
  char* m_error_msg;
  int m_error_line;
  uint8_t *m_joypad_map;
 
  static void* mrb_esp32_psram_allocf(mrb_state *mrb, void *p, size_t size, void *ud);
  void check_backtrace(mrb_state *mrb,mrb_value v);
  void prepare_env();
  void cleanup_env();
};

/**
 * System app
 **/

enum class FMRB_SYS_STATE{
  INIT=0,
  SHOW_MENU,
  DO_EDIT,
  EXEC_FROM_EDIT,
  EXEC_FROM_FILE,
};

class FmrbSystemApp {
public:
  fabgl::VGAController *m_vga;
  fabgl::PS2Controller *m_ps2;
  fabgl::Canvas *m_canvas;
  FmrbConfig *m_config;
  FmrbMrubyEngine *m_mruby_engine;
  FmrbFileService *m_storage;
  bool m_resolution_updated;

  FmrbSystemApp(fabgl::VGAController*,fabgl::PS2Controller*,fabgl::Canvas*);
  void init(FmrbFileService* st);
  FMRB_RCODE run();

  static void draw_img(fabgl::VGAController *vga,uint16_t x0,uint16_t y0,uint8_t* data,int mode);

private:
  char* m_script;
  FMRB_SYS_STATE m_state;
  FmrbEditor *m_editor;
  bool m_terminal_available;
  fabgl::Terminal m_terminal;
  FmrbMenuModule *m_main_menu;

  FMRB_RCODE init_terminal();
  FMRB_RCODE close_terminal();
  FMRB_RCODE print_system_info();
  FMRB_RCODE show_splash();
  FMRB_RCODE clear_splash();
  FmrbMenuItem* prepare_top_menu();
  FMRB_RCODE run_main_menu();
  FMRB_RCODE run_editor();
  FMRB_RCODE run_mruby();

};

FMRB_RCODE fmrb_subapp_resolution_test(FmrbMenuModule* menu);
FMRB_RCODE fmrb_subapp_select_main_resolution(FmrbMenuModule* menu);
FMRB_RCODE fmrb_subapp_select_mruby_resolution(FmrbMenuModule* menu);
FMRB_RCODE fmrb_subapp_set_mruby_buffering(FmrbMenuModule* menu);
int16_t fmrb_subapp_select_file(FmrbDir* dir_obj,FmrbDialog *dialog);
FMRB_RCODE fmrb_subapp_save_config(FmrbMenuModule* menu);

// public function
FmrbSystemApp* fmrb_get_system_obj(void);
char* alloc_text_mem(const char* input);
FmrbConfig* get_system_config(void);

