R"( #"
include Narya

def test
  Display.draw_picture(0,0,"/bk_small.img")
  Display.swap
end

def test2
  test
end

#test2
#puts "Draw Done"

def wait_key
  loop do
    if Input::available
      if Input::keydown?(125)
        break
      end
      ESP32::System::delay 10
    end
  end
end

Display::clear
img = Bitmap.new
img.load("/test.img")
p img.width
p img.height
img.draw(20,20)


puts "Done1"
Display::swap
puts "Done2"

ESP32::System::delay 1000
loop do
  Display::clear
  img.draw(20,20)
  #Display.draw_picture(20,20,"/test.img")
  Display::draw_circle(100,100,8,4)
  Display::swap
  if Input::available
    if Input::keydown?(125)
      break
    end
  end
  ESP32::System::delay 10
end

)"