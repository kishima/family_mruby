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

FmrbAudio::FmrbAudio()
{
  fmrb_dump_mem_stat(1);
  m_generator = new SoundGenerator(DEFAULT_SAMPLE_RATE);
  fmrb_dump_mem_stat(2);
  m_wavegen = nullptr;
}

FmrbAudio::~FmrbAudio()
{
  if(m_generator){
    delete(m_generator);
  }
  if(m_wavegen){
    delete(m_wavegen);
  }
}

void FmrbAudio::load()
{
  fmrb_dump_mem_stat(1);
  m_wavegen = new SquareWaveformGenerator();
  m_wavegen->enable(true);
  m_wavegen->setSampleRate(DEFAULT_SAMPLE_RATE);
  m_wavegen->setFrequency(500);
  m_wavegen->setVolume(100);
  m_generator->attach(m_wavegen);
  fmrb_dump_mem_stat(2);
}

void FmrbAudio::play()
{
  m_generator->play(true);
  m_generator->setVolume(100);

}

void FmrbAudio::stop()
{
  m_generator->play(false);
  //if(m_wavegen){
  //  m_generator->detach(m_wavegen);
  //}
}