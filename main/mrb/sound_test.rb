R"( #"
include Narya

Sound::load_mml("O4FO5FC")
Sound::play

loop do
  k = Input::get_key
  if k==Key::K_RETURN
    break
  end
end

)"
