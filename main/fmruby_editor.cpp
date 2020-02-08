
#include <stdio.h>
#include "fabgl.h"
#include "fmruby.h"
#include "fmruby_app.h"
#include "fmruby_fabgl.h"
#include "fmruby_editor.h"


const char* null_script = "\n";

const char* sample_script = 
//#include "./mrb/entry_mrb.rb"
#include "./mrb/tetris.rb"
//#include "./mrb/bitmap_test.rb"
;

const char* sample_script2 = 
#include "./mrb/rpg_test.rb"
;


#define EDT_DEBUG(...)  printf(__VA_ARGS__)
//#define EDT_DEBUG(...)

EditLine::EditLine()
{
  text = (char*)fmrb_spi_malloc(EDITLINE_BLOCK_SIZE);
  if(text == nullptr) throw std::bad_alloc();;
  memset(text,0,EDITLINE_BLOCK_SIZE);
  text[0] = '\0';
  length = 0;
  buff_size = EDITLINE_BLOCK_SIZE;

  flag = 0;
  lineno = 0;
  prev = nullptr;
  next = nullptr;

}
EditLine::EditLine(char* input){
  int input_len = strlen(input);
  int block_size = (input_len+1)/EDITLINE_BLOCK_SIZE + 1;
  buff_size = EDITLINE_BLOCK_SIZE * block_size;
  text = (char*)fmrb_spi_malloc(buff_size);
  if(text == nullptr) throw std::bad_alloc();;
  memset(text,0,buff_size);
  strcpy(text,input);
  length = input_len;
  FMRB_DEBUG(FMRB_LOG::DEBUG,"input_len:%d, buff_size:%d\n",input_len,buff_size);

  flag = 0;
  lineno = 0;
  prev = nullptr;
  next = nullptr;
}

EditLine::~EditLine(){
  this->next = nullptr;
  this->prev = nullptr;
  if(this->text){
    fmrb_free(this->text);
  }
}

int EditLine::insert(uint16_t pos,char c)
{
  if(text==NULL) return -1;

  EDT_DEBUG("p:%d length=%d buffsize=%d\n",pos,length,buff_size);
  if( length+1+1 > buff_size) // Text lenght + null char + new char >= cuurent buff size
  {
    EDT_DEBUG("realloc block(text_p:%p new buff size:%d)\n",text,buff_size+EDITLINE_BLOCK_SIZE);
    text = (char*)fmrb_spi_realloc(text,buff_size+EDITLINE_BLOCK_SIZE);
    if (NULL==text)
    {
      return -1;
    }
    memset(text+buff_size,0,EDITLINE_BLOCK_SIZE);
    buff_size += EDITLINE_BLOCK_SIZE;
  }
  // 012345
  // ABC@      pos=2, length=3, buffsize=6
  // ABXC@            length=4, buffsize=6

  EDT_DEBUG("%p, %p, %d\n",text+pos+1,text+pos,length-pos+1);
  memmove(text+pos+1,text+pos,length-pos+1);
  text[pos]=c;
  length+=1;
  return 0;
}

int EditLine::backdelete(uint16_t pos)
{
  if(text==NULL) return -1;
  if(pos==0) return -1;
  // 012345
  // ABC@      pos=1, length=3, buffsize=6
  // BC@              length=2, buffsize=6

  EDT_DEBUG("%p, %p, %d\n",text+pos+1,text+pos,length-pos+1);
  memmove(text+pos-1,text+pos,length-pos+1);
  length-=1;

  EDT_DEBUG("p:%d length=%d buffsize=%d\n",pos,length,buff_size);
  if( length+1 <= buff_size - EDITLINE_BLOCK_SIZE) // Text lenght + null char < cuurent buff size - BLOCK
  {
    EDT_DEBUG("realloc block(text_p:%p new buff size:%d)\n",text,buff_size-EDITLINE_BLOCK_SIZE);
    text = (char*)fmrb_spi_realloc(text,buff_size-EDITLINE_BLOCK_SIZE);
    if (NULL==text)
    {
      return -1;
    }
    buff_size -= EDITLINE_BLOCK_SIZE;
  }

  return 0;
}


int EditLine::insert(char c)
{
  return 1;
}

char* EditLine::cut(uint16_t start_pos, uint16_t end_pos)
{
  // 0123456789
  // abcde@     // start:2 end:5
  // cde@       // buff
  // ab@        // text

  if(NULL==text)return NULL;
  if(end_pos < start_pos)return NULL;
  int copy_size = end_pos - start_pos + 1;
  char* buff = (char*)fmrb_spi_malloc(copy_size);
  memcpy(buff,&text[start_pos],copy_size);
  memmove(&text[start_pos],&text[end_pos],length-end_pos+1);
  length -= copy_size-1;
  
  EDT_DEBUG("start:%d end:%d length=%d buffsize=%d\n",start_pos,end_pos,length,buff_size);
  if( length+1 <= buff_size - EDITLINE_BLOCK_SIZE) // Text lenght + null char < cuurent buff size - BLOCK
  {
    EDT_DEBUG("realloc block(text_p:%p new buff size:%d)\n",text,buff_size-EDITLINE_BLOCK_SIZE);
    text = (char*)fmrb_spi_realloc(text,buff_size-EDITLINE_BLOCK_SIZE);
    if (NULL==text)
    {
      fmrb_free(buff);
      return NULL;
    }
    buff_size -= EDITLINE_BLOCK_SIZE;
  }
  return buff;
}

void EditLine::clear(void)
{
  text = (char*)fmrb_spi_malloc(EDITLINE_BLOCK_SIZE);
  memset(text,0,EDITLINE_BLOCK_SIZE);
  strcpy(text,"\n");
  length = strlen(text);
  buff_size = EDITLINE_BLOCK_SIZE;
}

/***********************************
 * FmrbEditor
 *   An editor for mruby code
 ***********************************/
FmrbEditor::FmrbEditor(fabgl::Terminal* terminal):
  FmrbTerminalInput(terminal),
  m_buff_head(nullptr),
  m_height(0),
  m_disp_height(0),
  m_width(0),
  m_disp_width(0),
  m_lineno_shift(6),
  m_x(0),
  m_y(0),
  m_disp_head_line(1),
  m_total_line(0),
  m_line_lexer_p(nullptr),
  m_term(terminal),
  m_error(EDIT_STATUS::EDIT_NO_ERROR)
{

}
FmrbEditor::~FmrbEditor(){

}

FMRB_RCODE FmrbEditor::begin()
{
  m_line_lexer_p = nullptr;
#if 0
  m_line_lexer_p = (FmrbSimpleLineLexer*)fmrb_spi_malloc(sizeof(FmrbSimpleLineLexer));
  if(m_line_lexer_p) m_line_lexer_p->init();
#endif
  return FMRB_RCODE::OK;
}

FMRB_RCODE FmrbEditor::release()
{
  if(m_line_lexer_p) fmrb_free( m_line_lexer_p );
  clear_buffer();
  m_buff_head = nullptr;
  return FMRB_RCODE::OK;
}


FMRB_RCODE FmrbEditor::run(char* input_script){
  if(!m_term) return FMRB_RCODE::ERROR;
  m_height = m_term->getRows();
  m_width  = m_term->getColumns();
  m_disp_height = m_height - 1;
  m_disp_width = m_width;

  FMRB_DEBUG(FMRB_LOG::INFO,"Editor begin(%d,%d)\n",m_width,m_height);

  m_term->clear();
  move_cursor(m_lineno_shift+1,1);

  if(input_script)
  {
    load(input_script);
    fmrb_free(input_script);
  }else{
    load(null_script);
    //load_demo_file();
  }

  update();

  while(true){
    FmrbVkey vkey = read_vkey();
    if(FmrbTerminalInput::is_visible(vkey)){
      int akey = FmrbTerminalInput::to_ascii(vkey);
      printf("V> %d\n",(int)vkey);
      printf("A> 0x%02x\n",(int)akey);
      insert_ch(akey);
    }else{
      printf("V> %d\n",(int)vkey);
      switch(vkey){
        case FmrbVkey::VK_RETURN:
        case FmrbVkey::VK_KP_ENTER:
          insert_ret();
        break;
        case FmrbVkey::VK_DELETE:
        case FmrbVkey::VK_BACKSPACE:
          delete_ch();
        break;
        case FmrbVkey::VK_CTRL_D:
          delete_line();
        break;

        case FmrbVkey::VK_UP:
          move_edit_cursor(0x41);
        break;
        case FmrbVkey::VK_DOWN:
          move_edit_cursor(0x42);
        break;
        case FmrbVkey::VK_RIGHT:
          move_edit_cursor(0x43);
        break;
        case FmrbVkey::VK_LEFT:
          move_edit_cursor(0x44);
        break;

        case FmrbVkey::VK_KP_PAGEUP:
          page_up();
        break;
        case FmrbVkey::VK_KP_PAGEDOWN:
          page_down();
        break;

        case FmrbVkey::VK_F1:
        break;
        case FmrbVkey::VK_F2:
          save_file();
        break;
        case FmrbVkey::VK_F3:
          load_file();
        break;
        case FmrbVkey::VK_F4:
          return FMRB_RCODE::OK;
        break;
        case FmrbVkey::VK_F5:
          load_demo_file(0);
        break;
        case FmrbVkey::VK_F6:
          load_demo_file(1);
        break;

        default:
        break;
      }
    }
  }
  return FMRB_RCODE::ERROR;
}

EditLine* FmrbEditor::load_line(const char* in)
{
  EditLine* line_p = new EditLine();
  int csr=0;
  bool end_flag=false;
  while(!end_flag)
  {
    if( (csr+1) % EDITLINE_BLOCK_SIZE == 0){
      line_p->buff_size = line_p->buff_size+EDITLINE_BLOCK_SIZE;
      line_p->text = (char*)fmrb_spi_realloc(line_p->text,line_p->buff_size);
      if(line_p->text == nullptr) throw "load_line:realloc error!";
    }
    line_p->text[csr] = in[csr];

    if(in[csr]=='\r' || in[csr]=='\n')
    {
      if(csr>=line_p->buff_size){
        FMRB_DEBUG(FMRB_LOG::ERR,"load_line:bad alloc csr:%d buffsieze:%d\n",csr,line_p->buff_size);
      }
      line_p->text[csr] = '\0';
      line_p->length = csr;
      end_flag = true;
    }
    csr++;
  }
  return line_p;
}


void FmrbEditor::load(const char* buf)
{
  m_error = EDIT_NO_ERROR;
  int csr=0;
  m_total_line = 0;
  EditLine* fist_line = new EditLine();
  fist_line->prev=nullptr;
  EditLine* last_line = fist_line;
  while(buf[csr]!='\0')
  {
    EditLine* line = load_line(&buf[csr]);
    if(nullptr==line){
      m_error = EDIT_MEM_ERROR;
      return;
    }
    //FMRB_DEBUG(FMRB_LOG::DEBUG,"load size=%04d : %s\n",line->length,line->text);
    m_total_line += 1;
    line->lineno = m_total_line;
    csr += line->length;
    line->prev = last_line;
    last_line->next = line;

    csr++;
    last_line = line;
  }
  last_line->next=nullptr;
  m_buff_head = fist_line;
}

void FmrbEditor::print_csr_info(void)
{
  EDT_DEBUG("(%02d,%02d) head=%d total=%d\n",m_x,m_y,m_disp_head_line,m_total_line);
}

void FmrbEditor::move_edit_cursor(int dir)
{
  int current_line_n = m_disp_head_line-1 + m_y;
  EDT_DEBUG("current_line_n=%d\n",current_line_n);
  EditLine* line = seek_line(current_line_n);
  if(NULL==line){
    return;
  }
  switch(dir){
    case 0x41:  // A : UP
      if(1==m_y){
        //scroll
        if(m_disp_head_line > 1){
          m_disp_head_line -= 1;
          update();
        }
      }else if(m_y > 1){
        m_y = m_y - 1;
        if( m_x > m_lineno_shift + line->prev->length + 1 ){
          m_x = m_lineno_shift + line->prev->length + 1;
        }
        move_cursor(m_x, m_y);
      }
      break;
    case 0x42:  // B : DOWN
      if(m_disp_height == m_y){
        //scroll
        if(m_disp_head_line-1+m_disp_height < m_total_line){
          m_disp_head_line += 1;
          update();
        }
      }else if(m_y < m_disp_height ){
        m_y = m_y + 1;
        if(m_disp_head_line+m_y-1 > m_total_line) m_y -= 1;
        if(line->next){
          if( m_x > m_lineno_shift + line->next->length + 1 ){
            m_x = m_lineno_shift + line->next->length + 1;
          }
        }
        move_cursor(m_x, m_y);
      }
      break;
    case 0x43:  // C : RIGHT
      if(m_x <= m_lineno_shift + line->length){
        move_cursor(m_x + 1, m_y);
      }
      break;
    case 0x44:  // D : LEFT
      if(m_x > m_lineno_shift + 1){
        move_cursor(m_x - 1, m_y);
      }
      break;
    default:
      break;
  }
}

void FmrbEditor::move_cursor(int x,int y)
{
  if(x>m_width)x=m_width;
  if(x<1)x=1;
  if(y>m_height)y=m_height;
  if(y<1)y=1;
  m_x = x;
  m_y = y;
  move(m_x,m_y);
}

void FmrbEditor::move(int x,int y)
{
  if(x>m_width)x=m_width;
  if(x<1)x=1;
  if(y>m_height)y=m_height;
  if(y<1)y=1;
  char buf[20];
  memset(buf,0,sizeof(buf));
  sprintf(buf,"\e[%d;%dH",y,x);
  m_term->write(buf);
}


EditLine* FmrbEditor::seek_line(int n)
{
  if(n<1 || n>m_total_line)
  {
    FMRB_DEBUG(FMRB_LOG::ERR,"seek line > total line\n");
    return NULL;
  }
  if(NULL==m_buff_head)
  {
    FMRB_DEBUG(FMRB_LOG::ERR,"m_buff_head is NULL\n");
    return NULL;
  }
  if(NULL == m_buff_head->next){
    return NULL; 
  } 
  EditLine* line = m_buff_head;
  for(int i=1;i<=n;i++){
    line = line->next;
    if(NULL==line){
      FMRB_DEBUG(FMRB_LOG::ERR,"line %d doesn't exist\n",n);
      return NULL;
    }
  }
  return line;
}

void FmrbEditor::draw_line(int disp_y,EditLine* line)
{
  move(1,disp_y);
  m_term->write("\e[2K"); // delete line
  if(!line) return;
  m_term->printf("\e[34m%04d: \e[0m",line->lineno);// line number

  if(m_line_lexer_p){
    m_line_lexer_p->set_line(line->text);
    while(true){
      const char* buff;
      int len=0;
      m_line_lexer_p->itr_dump(&buff,&len);
      if(buff==NULL)break;
      //printf("[%.*s]",len,buff);
      //printf("%.*s",len,buff);
      m_term->write(buff,(size_t)len);
    }
  }else{
    m_term->write(line->text);
  }

  //m_term->write("\r\n");
}


void FmrbEditor::update()
{
  EditLine* line = seek_line(m_disp_head_line);
  for(int cnt=1; cnt<=m_disp_height; cnt++){
    draw_line(cnt,line);
    if(line){
      if(m_y == cnt){
        if( m_x >= line->length ){
          move(line->length, m_y);
        }
      }
      line = line->next;
    }
  }

  //Draw Functions
  int bottom = m_height;
  move(1,bottom);
  m_term->write("\e[30m\e[46m UPDATE \e[0m");
  move(11,bottom);
  m_term->write("\e[30m\e[46m  SAVE  \e[0m");
  move(21,bottom);
  m_term->write("\e[30m\e[46m  LOAD  \e[0m");
  move(31,bottom);
  m_term->write("\e[30m\e[46m  RUN   \e[0m");
  move(41,bottom);
  m_term->write("\e[30m\e[46m  DEMO  \e[0m");
  move(51,bottom);
  m_term->write("\e[30m\e[46m  DEMO2 \e[0m");
  move(61,bottom);
  size_t ramsize = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);
  m_term->printf("%d",ramsize);

  move(m_x,m_y);

  m_term->flush(false);//Required to avoid freeze
}

void FmrbEditor::update_lineno(void)
{
  if(NULL==m_buff_head) return;
  if(NULL==m_buff_head->next) return;
  EditLine* line = m_buff_head->next;
  int lineno = 0;
  while(line)
  {
    lineno += 1;
    line->lineno = lineno;
    line = line->next;
  }

}

void FmrbEditor::insert_ch(char c)
{
  EditLine* line = seek_line(m_disp_head_line+m_y-1);
  //FMRB_DEBUG(FMRB_LOG::DEBUG,"INSERT(%c):text:%s\n",c,line->text);
  line->insert(m_x-m_lineno_shift-1,c);
  draw_line(m_y,line);
  m_x += 1;
  move(m_x,m_y);
}

void FmrbEditor::insert_ret()
{
  EditLine* line = seek_line(m_disp_head_line+m_y-1);
  if(NULL==line)return;
  char* cut_test = line->cut(m_x-m_lineno_shift-1,line->length);
  if(NULL==cut_test)return;
  EditLine* new_line = new EditLine(cut_test);
  new_line->prev = line;
  new_line->next = line->next;
  line->next = new_line;
  m_total_line++;
  fmrb_free(cut_test);
  update_lineno();

  m_x = m_lineno_shift + 1;

  if(m_disp_height == m_y){
    //scroll
    if(m_disp_head_line-1+m_disp_height < m_total_line){
      m_disp_head_line += 1;
    }
  }else{
    m_y = m_y + 1;
  }

  update();
}

void FmrbEditor::delete_ch()
{
  EditLine* line = seek_line(m_disp_head_line+m_y-1);
  //FMRB_DEBUG(FMRB_LOG::DEBUG,"BS:text:%s\n",line->text);
  if(m_x-m_lineno_shift-1>0){
    line->backdelete(m_x-m_lineno_shift-1);
    draw_line(m_y,line);
    m_x -= 1;
    move(m_x,m_y);
  }
}

void FmrbEditor::page_up()
{
  m_disp_head_line -= m_disp_height;
  if(m_disp_head_line < 1 ){
    m_disp_head_line = 1;
  }
  update();
  print_csr_info();
}

void FmrbEditor::page_down()
{
  m_disp_head_line += m_disp_height;
  if(m_disp_head_line + m_disp_height >= m_total_line){
    m_disp_head_line = m_total_line - m_disp_height;
    if(m_disp_head_line<1)m_disp_head_line = 1;
  }
  update();
  print_csr_info();
}

void FmrbEditor::delete_line()
{
  EditLine* line = seek_line(m_disp_head_line+m_y-1);
  print_csr_info();
  printf("line:%p\n",line);

  if(NULL==line)return;
  if(m_total_line>1){
    EditLine* prev = line->prev;
    EditLine* next = line->next;
    prev->next = next;
    if(next){
      next->prev = prev;
    }
    delete line;
    m_total_line -= 1;
    if(!next){
      m_y = m_y - 1;
      if(m_y<1) m_y = 1;
      move_cursor(m_x, m_y);
    }
    update_lineno();
  }else{
    line->clear();
    m_x = m_lineno_shift;
    move_cursor(m_x, m_y);
  }
  update();
}


char* FmrbEditor::dump_script(void)
{
  if(NULL==m_buff_head) return NULL;
  if(NULL==m_buff_head->next) return NULL;
  EditLine* line = m_buff_head->next;
  int total_length = 0;
  while(line)
  {
    total_length += line->length + 1;
    line = line->next;
  }
  FMRB_DEBUG(FMRB_LOG::DEBUG,"total_length=%d\n",total_length);
  char* buff = (char*)fmrb_spi_malloc(total_length+1);
  if(NULL==buff){
    FMRB_DEBUG(FMRB_LOG::ERR,"cannot allocate memory!\n");
    return NULL;
  }
  int csr = 0;
  line = m_buff_head->next;
  while(line)
  {
    memcpy(&buff[csr],line->text,line->length);
    buff[csr+line->length]='\n';
    csr += line->length+1;
    line = line->next;
  }
  buff[csr]='\0';
  FMRB_DEBUG(FMRB_LOG::DEBUG,"csr=%d total_length=%d\n",csr,total_length);
  //FMRB_DEBUG(FMRB_LOG::DEBUG,"%s\n",buff);
  return buff;
}

void FmrbEditor::clear_buffer(){
  FMRB_DEBUG(FMRB_LOG::DEBUG,"clear_buffer\n");
  EditLine* line = m_buff_head;
  while(line)
  {
    EditLine* old = line;
    line = line->next;
    //FMRB_DEBUG(FMRB_LOG::DEBUG,"delete %p\n",old);
    delete old;
  }
  m_buff_head = NULL;
}

void FmrbEditor::load_file(){
  FMRB_DEBUG(FMRB_LOG::DEBUG,"load_file\n");
  //clear current buffer
  clear_buffer();
  uint32_t fsize;
  char* buff = FMRB_storage.load("/default.rb",fsize,true,false);
  move_cursor(m_lineno_shift+1,1);
  if(buff){
    load(buff);
  }else{
    FMRB_DEBUG(FMRB_LOG::ERR,"failed to load file\n");
    load(null_script);
  }
  update();
  FMRB_DEBUG(FMRB_LOG::DEBUG,"load_file done\n");
}

void FmrbEditor::load_demo_file(int type){
  //clear current buffer
  clear_buffer();
  move_cursor(m_lineno_shift+1,1);
  if(type==0){
    load(sample_script);
  }else{
    load(sample_script2);
  }
  update();
}

void FmrbEditor::save_file(){
  FMRB_DEBUG(FMRB_LOG::DEBUG,"save_file\n");
  char* buff = dump_script();
  if(buff){
    FMRB_storage.save(buff,"/default.rb");
    fmrb_free(buff);
  }else{
    FMRB_DEBUG(FMRB_LOG::ERR,"dump_script error\n");
  }
  FMRB_DEBUG(FMRB_LOG::DEBUG,"save_file done\n");
}

const char* highlight_words[] = 
{"BEGIN","class","ensure","nil","self","when",
"END","def","false","not","super","while",
"alias","defined?","for","or","then","yield",
"and","do","if","redo","true",//"__LINE__",
"begin","else","in","rescue","undef",//"__FILE__",
"break","elsif","module","retry","unless",//"__ENCODING__",
"case","end","next","return","until"};

const uint8_t highlight_words_color[] = 
{1,1,1,1,1,1,
 1,1,1,1,1,1,
 1,1,1,1,1,1,
 1,1,1,1,1,//1,
 1,1,1,1,1,//1,
 1,1,1,1,1,//1,
 1,1,1,1,1};

 /*
\e[30m	Black
\e[31m	Red
\e[32m	Green
\e[33m	Yellow
\e[34m	Blue
\e[35m	Magenta
\e[36m	Cyan
\e[37m	White
 */

const char* color_string_list[] = {"\e[31m","\e[35m","\e[32m","\e[36m"};
const char* gfooter_str = "\e[0m";


void FmrbSimpleLineLexer::init(){
    m_word_pos = 0;
    m_total_words = 0;
    m_dump_stat = 0;
}

void FmrbSimpleLineLexer::free(){

}

void FmrbSimpleLineLexer::move_head(){
    m_word_pos = 0;
    m_dump_stat = 0;
}

const char* FmrbSimpleLineLexer::last_word_tail(){
    if(m_word_pos>0){
        return m_word_p[m_word_pos-1]+m_word_len[m_word_pos-1]-1;
    }
    return m_string;
}

void FmrbSimpleLineLexer::push_word_prim(const char* word_head,int word_size){
    if(m_total_words >= LEXER_WORDS_MAX){
        printf("ERROR\n");
    }
    //printf("B(%d)[%.*s]\n",m_word_pos,word_size,word_head);
    m_word_p[m_word_pos] = word_head;
    m_word_len[m_word_pos] = word_size;
    m_word_pos++;
    m_total_words++;
}

void FmrbSimpleLineLexer::push_word(const char* word_head,int word_size){
    //printf("A(%d)[%.*s]",m_word_pos,word_size,word_head);
    //printf("\n");
    if( last_word_tail()+1 < word_head){
        push_word_prim(last_word_tail()+1, word_head - (last_word_tail()+1));
    }
    push_word_prim(word_head,word_size);
}

int FmrbSimpleLineLexer::is_highlight(const char* word_head,int word_size){
    if(word_size<=0)return -1;
    for(int i=0;i<sizeof(highlight_words)/sizeof(char*);i++){
        if(strncmp(word_head,highlight_words[i],strlen(highlight_words[i]))==0){
            //printf("HIT[%.*s] ",word_size,word_head);
            return i;
        }
    }
    if(*word_head == '@') return 100; //@xxx
    if(0x5A >= *word_head && *word_head >= 0x41 ) return 101; //upper case
    return -1;
}

void FmrbSimpleLineLexer::itr_dump(const char** buff,int* len){
    *buff = NULL;
    *len = 0;
    if(m_word_pos>=m_total_words){
        return;
    }
    //printf("(%d:%d)\n",m_word_pos,m_total_words);
    
    if( m_dump_stat > 0 ){
        switch(m_dump_stat){
            case 1:
                *buff = m_word_p[m_word_pos];
                *len  = m_word_len[m_word_pos];
                m_dump_stat++;
            break;
            case 2:
                *buff = gfooter_str;
                *len = strlen(gfooter_str);
                m_dump_stat = 0;
                m_word_pos++;
            break;
            default:
            break;
        }

    }else{
        int type = is_highlight(m_word_p[m_word_pos],m_word_len[m_word_pos]);
        //printf("|%d|",type);
        if(type>=0){ //insert header footer
            if(type<100){
              *buff = color_string_list[highlight_words_color[type]];
            }else{
              if(type==100) *buff = color_string_list[3];
              if(type==101) *buff = color_string_list[2];
            }
            *len = strlen(*buff);
            m_dump_stat = 1;
        }else{
            *buff = m_word_p[m_word_pos];
            *len  = m_word_len[m_word_pos];
            m_word_pos++;
        }
    }
}

bool FmrbSimpleLineLexer::is_delimiter(const char c){
    switch(c){
        case ' ':
        case '(':
        case ')':
        case '\'':
        case '\"':
        case '{':
        case '}':
        case '|':
        case '=':
        case '-':
        case '*':
        case '+':
        case '/':
        case '<':
        case '>':
        case '#':
        case '&':
        case '[':
        case ']':
        case ';':
        case ':':
        case ',':
        case '.':
        case '!':
        case '\0':
        return true;
        default:
        return false;
    }
    return true;
}

void FmrbSimpleLineLexer::set_line(const char* line){
    init();
    m_string = line;

    bool in_string = false;
    int pos=0;
    int stat = 0;//1:in word, 2:out word
    const char* word_top = NULL;
    const char* word_end = NULL;
    char last_c = 0;

    while(true){
        char c = m_string[pos];
        bool delimiter = is_delimiter(c);
        if(!delimiter){
            if(stat==0){
                if(in_string){
                    if(!word_top){
                        word_top = &m_string[pos];
                    }
                }else{
                    word_top = &m_string[pos];
                }
            }
            stat = 1;
        }else{
            if(stat==1){
                bool end_of_word = false;
                if(in_string){
                    if(c=='\"'){
                        if(last_c!='\\'){ //escaped \" in string
                            in_string = false;
                            end_of_word = true;
                        }
                    }else{
                        //continue string
                    }
                }else{
                    if(c=='\"'){
                        in_string = true;
                    }
                    end_of_word = true;
                }

                if(end_of_word){
                    word_end = &m_string[pos];
                    if(word_top){
                        size_t size = word_end-word_top;
                        push_word(word_top,size);
                    }
                    word_top = nullptr;
                }
            }else{
                if(in_string){
                    if(c=='\"'){
                        if(last_c!='\\'){ //escaped \" in string
                            in_string = false;
                        }
                    }else{
                        //continue string
                    }
                }else{
                    if(c=='\"'){
                        in_string = true;
                    }
                }
            }
            stat = 0;
        }
        //printf("%c %d %d %p\n",c,stat,in_string,word_top);
        if(m_string[pos]=='\0'){
            const char* tail = last_word_tail();
            if( tail < &m_string[pos]-1){
                //printf("%p %p\n",tail,&m_string[pos]-1);
                push_word(tail+1,(&m_string[pos]-1-tail));
            }
            break;
        } 
        last_c = c;
        pos++;
    }
    move_head();
}
