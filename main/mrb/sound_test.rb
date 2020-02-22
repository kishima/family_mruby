R"( #"
include Narya

  #Sound::load_mml("T170O4F2<F2C1 >B-2<F2>F1 F2A2<A2.B")
  #Sound::load_mml "T130L8O5 FR16DF16ECD FR16DF16ECD FR16DF16EDC DC>B-A<DF"
Sound::load_mml 0,"T72L4O4V100 A32<C32E32G+32 A<ED8C8>B8A8 G8.G16A8B8A4.R8"
Sound::load_mml 1,"T72L16O4V60 R8             EC>A<CEAE>A<AFC>A<FD>B<D"
Sound::play_all(true)

Display::draw_text(40,100,"Play music!","WHITE")
Display::swap

Narya::wait_key Key::K_RETURN

)"
