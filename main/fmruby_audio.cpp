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

static int solfa_to_i(char* solfa)
{
  if(0==strcmp("C",solfa)){
    return 0;
  }else if(0==strcmp("C+",solfa)){
    return 1;
  }else if(0==strcmp("D-",solfa)){
    return 1;
  }else if(0==strcmp("D",solfa)){
    return 2;
  }else if(0==strcmp("D+",solfa)){
    return 3;
  }else if(0==strcmp("E-",solfa)){
    return 3;
  }else if(0==strcmp("E",solfa)){
    return 4;
  }else if(0==strcmp("F",solfa)){
    return 5;
  }else if(0==strcmp("F+",solfa)){
    return 6;
  }else if(0==strcmp("G-",solfa)){
    return 6;
  }else if(0==strcmp("G",solfa)){
    return 7;
  }else if(0==strcmp("G+",solfa)){
    return 8;
  }else if(0==strcmp("A-",solfa)){
    return 8;
  }else if(0==strcmp("A",solfa)){
    return 9;
  }else if(0==strcmp("A+",solfa)){
    return 10;
  }else if(0==strcmp("B-",solfa)){
    return 10;
  }else if(0==strcmp("B",solfa)){
    return 11;
  }
  return 0;
}

static int octave_to_i(char* oct){
  if(0==strcmp("O0",oct)){
    return -9;
  }else if(0==strcmp("O1",oct)){
    return 3;
  }else if(0==strcmp("O2",oct)){
    return 15;
  }else if(0==strcmp("O3",oct)){
    return 27;
  }else if(0==strcmp("O4",oct)){
    return 39;
  }else if(0==strcmp("O5",oct)){
    return 51;
  }else if(0==strcmp("O6",oct)){
    return 63;
  }else if(0==strcmp("O7",oct)){
    return 75;
  }else if(0==strcmp("O8",oct)){
    return 87;
  }
  return 39;
}

FmrbAudio::FmrbAudio():
play_stat(0),
m_mml_str(nullptr)
{
  fmrb_dump_mem_stat(1);
  m_generator = new SoundGenerator(DEFAULT_SAMPLE_RATE);

  m_wavegen = new SquareWaveformGenerator();
  m_wavegen->enable(false);
  m_wavegen->setSampleRate(DEFAULT_SAMPLE_RATE);
  m_wavegen->setFrequency(500);
  m_wavegen->setVolume(100);
  m_generator->attach(m_wavegen);

  xTaskCreate(musicTask, "music", FMRB_MUSIC_TASK_STACK_SIZE, this, FMRB_MUSIC_TASK_PRIORITY, &m_musicTaskHandle);
  fmrb_dump_mem_stat(2);
}

FmrbAudio::~FmrbAudio()
{
  vTaskDelete(m_musicTaskHandle);

  if(m_wavegen){
    m_generator->detach(m_wavegen);
    delete(m_wavegen);
  }
  if(m_generator){
    delete(m_generator);
  }
  if(m_mml_str){
    fmrb_free(m_mml_str);
  }

}



void FmrbAudio::musicTask(void * arg)
{
  FmrbAudio* audio = (FmrbAudio*)arg;
  FMRB_DEBUG(FMRB_LOG::INFO,"musicTask begin\n");
  int csr = 0;
  int len = 0;
  int tempo = 120;
  int duration = 1000*60/tempo; //ms
  int octave = octave_to_i("O4");
  while(true){
    if(0==audio->play_stat){
      vTaskDelay(100);
      continue;
    } 
    if(1==audio->play_stat){
      FMRB_DEBUG(FMRB_LOG::INFO,"musicTask play start\n");
      audio->m_generator->play(true);
      audio->m_generator->setVolume(60);
      audio->play_stat = 2;
      len = strlen(audio->m_mml_str);
      FMRB_DEBUG(FMRB_LOG::DEBUG,"mml:%s\n",audio->m_mml_str);
    }
    if(2==audio->play_stat){
      if(len<=0)continue;
      if(csr>=len)csr=0;
      char cmd[10];
      char* mml = audio->m_mml_str;
      if( mml[csr] >= 'A' && mml[csr] <='G'){ //Solfa
        cmd[0]=mml[csr];
        if(mml[csr+1]=='-' || mml[csr+1]=='+'){
          cmd[1]=mml[csr];
          cmd[2]='\0';
          csr += 2;
        }else{
          cmd[1]='\0';
          csr += 1;
        }
        int i = solfa_to_i(cmd);
        int freq = freq_scale[octave+i];
        duration = 1000*60/tempo; //ms
        FMRB_DEBUG(FMRB_LOG::DEBUG,"freq:%d solfa:%d duration:%d\n",freq,i,duration);
        audio->m_wavegen->setFrequency(freq);
        audio->m_wavegen->enable(true);
        vTaskDelay(duration);
      }else if( mml[csr] == 'O' ){
         //Octave
        cmd[0]=mml[csr];
        cmd[1]=mml[csr+1];
        cmd[2]='\0';
        octave = octave_to_i(cmd);
        FMRB_DEBUG(FMRB_LOG::DEBUG,"octave:%d\n",octave);
        csr += 2;
      }else{
        vTaskDelay(10);
      }
      FMRB_DEBUG(FMRB_LOG::DEBUG,"csr:%d\n",csr);

    }

  }
}


void FmrbAudio::load_mml(const char* mml)
{
  if(m_mml_str){
    fmrb_free(m_mml_str);
  }
  m_mml_str = (char*)fmrb_spi_malloc(strlen(mml)+1);
  strcpy(m_mml_str,mml);
}

void FmrbAudio::play_mml()
{
  play_stat = 1;
}

void FmrbAudio::stop()
{
  play_stat = 0;
  m_generator->play(false);
}


