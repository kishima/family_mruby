require 'rubygems'
require 'RMagick'

#convert image.png -colors 64 -depth 2 +dither image3.png
image = Magick::Image.read('image3.jpg').first

height = image.rows
width  = image.columns
puts "w:#{width} h:#{height}"
exit if height > 256*256
exit if width > 256*256

header = 0xFFFFFFFF

File.open("./test.img","wb") do |fp|
  fp.write([ header ].pack("V")) #pack uint32_t
  fp.write([ width  ].pack("v")) #pack uint16_t little:v  big:n
  fp.write([ height ].pack("v"))

  (0...height).each do |y| 
    (0...width).each do |x| 
      pixel = image.pixel_color(x, y) 

      a = 0 #alpha
      r = (pixel.red*3 / 65535.0).round
      g = (pixel.green*3 / 65535.0).round
      b = (pixel.blue*3 / 65535.0).round
      #val = (b << 4) | (g << 2) | r

      val = (a << 6) | (b << 4) | (g << 2) | r
      #puts "#{pixel.red},#{pixel.green},#{pixel.blue}"
      puts "#{r},#{g},#{b}"
      fp.write([val].pack("C"))
    end 
  end

end

puts "Done"

