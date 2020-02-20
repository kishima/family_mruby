R"( #"
include Narya

#Sound::load_mml("T170O4F2<F2C1 >B-2<F2>F1 F2A2<A2.B")
Sound::load_mml "T130L8O5 FR16DF16ECD FR16DF16ECD FR16DF16EDC DC>B-A<DF"
Sound::play

Display::draw_text(10,30,"Play music!","WHITE")
Display::swap

loop do
  k = Input::get_key
  if k==Key::K_RETURN
    break
  end
end

)"
