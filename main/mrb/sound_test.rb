R"( #"
include Narya

Display::clear
Display::swap
Display::clear
Display::swap

Sound::load
Sound::play

loop do
  k = Input::get_key
  if k==Key::K_RETURN
    break
  end
end

)"
