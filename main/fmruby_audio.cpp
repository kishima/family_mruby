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


#include "fmruby.h"
#include "fmruby_app.h"
#include "fmruby_fabgl.h"

static const int freq_scale[] = {
  // -5 , C0
  // -4 , D0
  // -3 , E0
  // -2 , F0
  // -1 , G0
/* 0 */	28 	, //	A0
/* 1 */	29 	, //	A#0
/* 2 */	31 	, //	B0
/* 3 */	33 	, //	C1
/* 4 */	35 	, //	C#1
/* 5 */	37 	, //	D1
/* 6 */	39 	, //	D#1
/* 7 */	41 	, //	E1
/* 8 */	44 	, //	F1
/* 9 */	46 	, //	F#1
/* 10 */	49 	, //	G1
/* 11 */	52 	, //	G#1
/* 12 */	55 	, //	A1
/* 13 */	58 	, //	A#1
/* 14 */	62 	, //	B1
/* 15 */	65 	, //	C2
/* 16 */	69 	, //	C#2
/* 17 */	73 	, //	D2
/* 18 */	78 	, //	D#2
/* 19 */	82 	, //	E2
/* 20 */	87 	, //	F2
/* 21 */	92 	, //	F#2
/* 22 */	98 	, //	G2
/* 23 */	104 	, //	G#2
/* 24 */	110 	, //	A2
/* 25 */	117 	, //	A#2
/* 26 */	123 	, //	B2
/* 27 */	131 	, //	C3
/* 28 */	139 	, //	C#3
/* 29 */	147 	, //	D3
/* 30 */	156 	, //	D#3
/* 31 */	165 	, //	E3
/* 32 */	175 	, //	F3
/* 33 */	185 	, //	F#3
/* 34 */	196 	, //	G3
/* 35 */	208 	, //	G#3
/* 36 */	220 	, //	A3
/* 37 */	233 	, //	A#3
/* 38 */	247 	, //	B3
/* 39 */	262 	, //	C4
/* 40 */	277 	, //	C#4
/* 41 */	294 	, //	D4
/* 42 */	311 	, //	D#4
/* 43 */	330 	, //	E4
/* 44 */	349 	, //	F
/* 45 */	370 	, //	F#4
/* 46 */	392 	, //	G4
/* 47 */	415 	, //	G#4
/* 48 */	440 	, //	A4
/* 49 */	466 	, //	A#4
/* 50 */	494 	, //	B4
/* 51 */	523 	, //	C5
/* 52 */	554 	, //	C#5
/* 53 */	587 	, //	D5
/* 54 */	622 	, //	D#5
/* 55 */	659 	, //	E5
/* 56 */	698 	, //	F5
/* 57 */	740 	, //	F#5
/* 58 */	784 	, //	G5
/* 59 */	831 	, //	G#5
/* 60 */	880 	, //	A5
/* 61 */	932 	, //	A#5
/* 62 */	988 	, //	B5
/* 63 */	1047 	, //	C6
/* 64 */	1109 	, //	C#6
/* 65 */	1175 	, //	D6
/* 66 */	1245 	, //	D#6
/* 67 */	1319 	, //	E6
/* 68 */	1397 	, //	F6
/* 69 */	1480 	, //	F#6
/* 70 */	1568 	, //	G6
/* 71 */	1661 	, //	G#6
/* 72 */	1760 	, //	A6
/* 73 */	1865 	, //	A#6
/* 74 */	1976 	, //	B6
/* 75 */	2093 	, //	C7
/* 76 */	2217 	, //	C#7
/* 77 */	2349 	, //	D7
/* 78 */	2489 	, //	D#7
/* 79 */	2637 	, //	E7
/* 80 */	2794 	, //	F7
/* 81 */	2960 	, //	F#7
/* 82 */	3136 	, //	G7
/* 83 */	3322 	, //	G#7
/* 84 */	3520 	, //	A7
/* 85 */	3729 	, //	A#7
/* 86 */	3951 	, //	B7
/* 87 */	4186 	, //	C8
};

static int solfa_to_i(const char* solfa)
{
  //printf("cmp:%s\n",solfa);
  if(0==strcmp("C",solfa)){
    return 0;
  }else if(0==strcmp("C+",solfa)){
    return 1;
  }else if(0==strcmp("D-",solfa)){
    return 1;
  }else if(0==strcmp("D+",solfa)){
    return 3;
  }else if(0==strcmp("D",solfa)){
    return 2;
  }else if(0==strcmp("E-",solfa)){
    return 3;
  }else if(0==strcmp("E",solfa)){
    return 4;
  }else if(0==strcmp("F+",solfa)){
    return 6;
  }else if(0==strcmp("F",solfa)){
    return 5;
  }else if(0==strcmp("G-",solfa)){
    return 6;
  }else if(0==strcmp("G+",solfa)){
    return 8;
  }else if(0==strcmp("G",solfa)){
    return 7;
  }else if(0==strcmp("A-",solfa)){
    return 8;
  }else if(0==strcmp("A+",solfa)){
    return 10;
  }else if(0==strcmp("A",solfa)){
    return 9;
  }else if(0==strcmp("B-",solfa)){
    return 10;
  }else if(0==strcmp("B",solfa)){
    return 11;
  }
  return 0;
}

static int octave_str_to_i(const char* oct){
  if(oct[0]=='O'){
    int ret = atoi(&oct[1]);
    if(ret<0)ret=0;
    if(ret>8)ret=8;
    return ret;
  }
  return 4;
}

static int octave_to_i(int oct){
  if(oct<0)oct=0;
  if(oct>8)oct=8;
  return -9+oct*12;
}

static int read_num(const char* in,int* num_len){
  int csr=0;
  char out[5];
  while(true){
    if( in[csr]>='0' && in[csr]<='9'){
      out[csr]=in[csr];
      csr++;
    }else if(in[csr]=='\0'){
      break;
    }else{
      break;
    }
    if(csr>=4)break;
  }
  if(csr==0)return -1;
  out[csr]='\0';
  *num_len=csr;
  return atoi(out);
}

FmrbMmlChannel::FmrbMmlChannel():
m_csr(0),
m_mml_len(0),
m_mml_str(nullptr)
{
  reset();
}
FmrbMmlChannel::~FmrbMmlChannel()
{
  if(m_mml_str) fmrb_free(m_mml_str);
}

void FmrbMmlChannel::reset()
{
  m_csr = 0;
  m_tempo = 120;
  m_freq = 400;
  m_instrument = 0;
  m_duration = 1000*60/m_tempo; //ms
  m_octave = octave_str_to_i("O4");
  m_def_slen = 4;
  m_play_pos = 0;
  m_next_play_pos = 0;
}
void FmrbMmlChannel::load(const char* mml)
{
  if(m_mml_str){
    fmrb_free(m_mml_str);
  }
  m_mml_len = strlen(mml);
  m_mml_str = (char*)fmrb_spi_malloc(m_mml_len+1);
  strcpy(m_mml_str,mml);

  reset();
}

bool FmrbMmlChannel::fetch(uint32_t play_pos)
{
  const char* mml = m_mml_str;

  if (m_mml_len <= 0) return false;

  m_play_pos = play_pos;
  //FMRB_DEBUG(FMRB_LOG::DEBUG,"pos %lu - %lu\n",m_play_pos,m_next_play_pos);
  if(play_pos < m_next_play_pos){
    return false;
  }
  if(play_pos > m_next_play_pos){
    return false;
  }

  char cmd[10];
  //FMRB_DEBUG(FMRB_LOG::DEBUG,"%s\n",&mml[m_csr]);
  bool update = false;
  while (true)
  {
    if (m_csr >= m_mml_len)
    {
      m_freq = 1;
      m_instrument = -1;
      m_duration = 0xFFFFFFFF;
      update = true;
      break;
    }
    if (mml[m_csr] >= 'A' && mml[m_csr] <= 'G')
    { //Solfa
      cmd[0] = mml[m_csr];
      if (mml[m_csr + 1] == '-' || mml[m_csr + 1] == '+')
      {
        cmd[1] = mml[m_csr + 1];
        cmd[2] = '\0';
        m_csr += 2;
      }
      else
      {
        cmd[1] = '\0';
        m_csr += 1;
      }
      int i = solfa_to_i(cmd);
      m_freq = freq_scale[octave_to_i(m_octave) + i];

      int point = 0;
      if (m_mml_str[m_csr] == '.')
      {
        point = 1;
        m_csr++;
      }

      int num_len = 0;
      int slen = read_num(mml + m_csr, &num_len);
      if (slen > 0)
      {
        m_csr += num_len;
        if (mml[m_csr] == '.')
        {
          point = 1;
          m_csr++;
        }
        m_duration = 1000 * 60 * 4 / slen / m_tempo; //ms
      }
      else
      {
        m_duration = 1000 * 60 * 4 / m_def_slen / m_tempo; //ms
      }
      if (point)
      {
        m_duration = m_duration * 3 / 2;
      }
      m_instrument = 0;
      m_next_play_pos = m_play_pos + m_duration;
      update = true;
      break;
    }
    else if (mml[m_csr] == 'R')
    {
      m_csr++;
      int num_len = 0;
      int slen = read_num(mml + m_csr, &num_len);
      int point = 0;
      if (slen > 0)
      {
        m_csr += num_len;
        if (mml[m_csr] == '.')
        {
          point = 1;
          m_csr++;
        }
        m_duration = 1000 * 60 * 4 / slen / m_tempo; //ms
        if (point)
        {
          m_duration = m_duration * 3 / 2;
        }
      }
      else
      {
        m_duration = 1000 * 60 * 4 / m_def_slen / m_tempo; //ms
      }
      //FMRB_DEBUG(FMRB_LOG::DEBUG, "Rest duration:%d\n", m_duration);
      m_freq = 1;
      m_instrument = -1;
      m_next_play_pos = m_play_pos + m_duration;
      update = true;
      break;
    }
    else if (mml[m_csr] == 'L')
    {
      m_csr++;
      int num_len = 0;
      int tmp = read_num(mml + m_csr, &num_len);
      if (tmp > 0)
      {
        m_csr += num_len;
        m_def_slen = tmp;
      }
    }
    else if (mml[m_csr] == 'T')
    {
      m_csr++;
      int num_len = 0;
      int tmp = read_num(mml + m_csr, &num_len);
      //FMRB_DEBUG(FMRB_LOG::DEBUG,"Tempo num:%d num_len:%d\n",tmp,num_len);
      if (tmp > 0)
      {
        m_csr += num_len;
        m_tempo = tmp;
      }
    }
    else if (mml[m_csr] == 'V')
    {
      m_csr++;
      int num_len = 0;
      int vol = read_num(mml + m_csr, &num_len);
      //FMRB_DEBUG(FMRB_LOG::DEBUG,"Volume vol:%d num_len:%d\n",vol,num_len);
      if (vol >= 0)
      {
        m_volume = vol;
        m_csr += num_len;
      }
    }
    else if (mml[m_csr] == ' ')
    {
      m_csr++;
    }
    else if (mml[m_csr] == '&')
    {
      m_csr++;
    }
    else if (mml[m_csr] == 'O')
    {
      //Octave
      cmd[0] = mml[m_csr];
      cmd[1] = mml[m_csr + 1];
      cmd[2] = '\0';
      m_octave = octave_str_to_i(cmd);
      //FMRB_DEBUG(FMRB_LOG::DEBUG, "octave:%d\n", m_octave);
      m_csr += 2;
    }
    else if (mml[m_csr] == '<')
    {
      m_csr++;
      m_octave++;
      //FMRB_DEBUG(FMRB_LOG::DEBUG, "octave+:%d\n", m_octave);
      if (m_octave > 8)
        m_octave = 8;
    }
    else if (mml[m_csr] == '>')
    {
      m_csr++;
      m_octave--;
      //FMRB_DEBUG(FMRB_LOG::DEBUG, "octave-:%d\n", m_octave);
      if (m_octave < 0)
        m_octave = 0;
    }
  }
  return update;
}

uint32_t FmrbMmlChannel::remain_duration()
{
  if(m_next_play_pos<m_play_pos) return 0;
  return m_next_play_pos - m_play_pos;
}

/**
 * FmrbAudio
 **/
FmrbAudio::FmrbAudio():
play_stat(0),
m_volume(128/2)
{
  fmrb_dump_mem_stat(1);
  m_generator = new SoundGenerator(DEFAULT_SAMPLE_RATE);

  for(int i=0;i<FMRB_AUDIO_MAX_CHANNEL;i++){
    m_mml_ch[i] = new FmrbMmlChannel();
    if(i==0){
      m_wavegen[i] = new SquareWaveformGenerator();
    }else{
      m_wavegen[i] = new TriangleWaveformGenerator();
    }
    m_wavegen[i]->enable(false);
    m_wavegen[i]->setSampleRate(DEFAULT_SAMPLE_RATE);
    m_wavegen[i]->setFrequency(500);
    m_wavegen[i]->setVolume(100);
    m_generator->attach(m_wavegen[i]);
  }

  xTaskCreate(musicTask, "music", FMRB_MUSIC_TASK_STACK_SIZE, this, FMRB_MUSIC_TASK_PRIORITY, &m_musicTaskHandle);
  fmrb_dump_mem_stat(2);
}

FmrbAudio::~FmrbAudio()
{
  printf("delete FmrbAudio\n");
  vTaskDelete(m_musicTaskHandle);
  vTaskDelay(100);

  for(int i=0;i<FMRB_AUDIO_MAX_CHANNEL;i++){
    if(m_wavegen[i]){
      m_generator->detach(m_wavegen[i]);
      delete(m_wavegen[i]);
    }
    if(m_mml_ch[i]){
      delete m_mml_ch[i];
    }
  }
  if(m_generator){
    delete(m_generator);
  }

}



void FmrbAudio::musicTask(void * arg)
{
  FmrbAudio* audio = (FmrbAudio*)arg;
  FMRB_DEBUG(FMRB_LOG::INFO,"musicTask begin\n");
  uint32_t play_pos = 0;
  uint32_t duration = 0;

  while(true){
    if(0==audio->play_stat){
      vTaskDelay(30);
      continue;
    }
    if(1==audio->play_stat){
      FMRB_DEBUG(FMRB_LOG::INFO,"musicTask play start\n");
      audio->m_generator->play(true);
      audio->m_generator->setVolume(audio->m_volume);
      audio->play_stat = 2;
      //len = strlen(audio->m_mml_str);
      //FMRB_DEBUG(FMRB_LOG::DEBUG,"mml:%s\n",audio->m_mml_str);
      //vTaskDelay(100);
    }
    if(2==audio->play_stat){
      duration = 0xFFFFFFFF;
      //FMRB_DEBUG(FMRB_LOG::DEBUG, ">>>>> play_pos:%d\n",play_pos);
      for(int i=0;i<FMRB_AUDIO_MAX_CHANNEL;i++){
        FmrbMmlChannel *ch = audio->m_mml_ch[i];
        bool update = ch->fetch(play_pos);
        if(update){
          int freq = ch->m_freq;
          int instrument = ch->m_instrument;
          if(duration > ch->m_duration) duration = ch->m_duration;

          //FMRB_DEBUG(FMRB_LOG::DEBUG, "[%d]freq:%d inst:%d duration:%lu ms\n",i, freq, ch->m_instrument,ch->m_duration);
          WaveformGenerator* wgen = audio->m_wavegen[i];
          if(instrument>=0){
            wgen->setFrequency(freq);
            wgen->setVolume(127 * ch->m_volume / 100 );
            wgen->setDuration(ch->m_duration * wgen->sampleRate() * 90 / 100 / 1000 );
            wgen->enable(true);
          }else{ //Rest
            wgen->enable(false);
          }
        }else{
          uint32_t rduraion = ch->remain_duration();
          if( rduraion > 0 && duration > rduraion) duration = rduraion;
          //FMRB_DEBUG(FMRB_LOG::DEBUG, "[%d]no update duration=%lu\n",i,rduraion);
        }
      }
      //FMRB_DEBUG(FMRB_LOG::DEBUG, "delay duration:%d\n",duration);
      if(duration<0xFFFFFFFF)
      {
        if(duration>1) vTaskDelay(duration);
        play_pos += duration;
      }else{
        FMRB_DEBUG(FMRB_LOG::DEBUG, "============================\n");
        FMRB_DEBUG(FMRB_LOG::DEBUG, "rest play > repeat\n");
        for(int i=0;i<FMRB_AUDIO_MAX_CHANNEL;i++){
          audio->m_mml_ch[i]->reset();
        }
        play_pos = 0;
        //vTaskDelay(30);
      }
    } 
  }
}


void FmrbAudio::load_mml(int ch,const char* mml)
{
  FMRB_DEBUG(FMRB_LOG::DEBUG,"ch:%d mml:%s\n",ch,mml);
  m_mml_ch[ch]->load(mml);
}

void FmrbAudio::play_mml(int ch,bool loop)
{
  play_stat = 1;
}

void FmrbAudio::play_all(bool loop)
{
  play_stat = 1;
}

void FmrbAudio::stop_all()
{
  play_stat = 0;
  m_generator->play(false);
}


void FmrbAudio::set_volume(int vol)
{
  if(vol<0)vol=0;
  if(vol>127)vol=127;
  m_volume = vol;
}