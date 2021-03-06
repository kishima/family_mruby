include Narya
class WalkMap
  def initialize
    @pos_x = 0
    @pos_y = 0
    @map = [
      [2,2,2,0,0,2,2,0,0,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,],
      [2,2,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,],
      [2,0,0,3,1,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,],
      [0,0,0,0,1,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,],
      [0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,],
      [0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,],
      [2,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,],
      [2,2,2,0,0,0,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,],
      [2,2,2,2,0,0,0,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,],
      [2,2,2,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,],
      [2,2,0,0,0,0,0,0,0,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,],
      [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,],
      [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,],
      [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,],
      [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,],
      [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,],
      [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,],
      [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,],
      [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,],
      [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,]
     ]
     @max_x = @map[0].size
     @max_y = @map.size
     puts "max>#{@max_x},#{@max_y}"
     @bitmap_list = []
     @bitmap_list << Bitmap.new.load("/spiffs/sample/grass.img")  # 0
     @bitmap_list << Bitmap.new.load("/spiffs/sample/road.img")   # 1
     @bitmap_list << Bitmap.new.load("/spiffs/sample/water.img")  # 2
     @bitmap_list << Bitmap.new.load("/spiffs/sample/castle.img") # 3
     @character_list = []
     @character_list << Bitmap.new.load("/spiffs/sample/man.img")

     @offset_x = 40
     @offset_y = 20
  end
  
  def set(x,y)
    @pos_x = x
    @pos_y = y
  end

  def move(k)
    case k
    when 1 #UP
      @pos_y = @pos_y - 1
    when 2 #DOWN
      @pos_y = @pos_y + 1
    when 3 #LEFT
      @pos_x = @pos_x - 1
    when 4 #RIGHT
      @pos_x = @pos_x + 1
    end
    @pos_x = 0 if @pos_x < 0
    @pos_x = @max_x - 1 if @pos_x >= @max_x
    @pos_y = 0 if @pos_y < 0
    @pos_y = @max_y - 1 if @pos_y >= @max_y
    puts " pos( #{@pos_x},#{@pos_y} ) "
  end

  def draw
    (0...9).each do |y|
      (0...9).each do |x|
        sx = @pos_x + x - 6
        sy = @pos_y + y - 6
        chip = 0
        if(sx<0 or sx>=@max_x or sy<0 or sy>=@max_y)
          chip = 2
        else
          chip = @map[sy][sx]
        end
        @bitmap_list[chip].draw(x*16+@offset_x ,y*16+@offset_y)
      end
    end
    @character_list[0].draw(4*16+@offset_x ,4*16+@offset_y)
  end

end

world_map = WalkMap.new

def get_key
  #return 0 unless Input::available
  k = 0
  #k = 1  if Input::keydown?(Key::K_UP)
  #k = 2  if Input::keydown?(Key::K_DOWN)
  #k = 3  if Input::keydown?(Key::K_LEFT)
  #k = 4  if Input::keydown?(Key::K_RIGHT)
  #k = -1 if Input::keydown?(Key::K_ESCAPE)

  k = 1  if Input::paddown?(Pad::UP)
  k = 2  if Input::paddown?(Pad::DOWN)
  k = 3  if Input::paddown?(Pad::LEFT)
  k = 4  if Input::paddown?(Pad::RIGHT)
  k = -1 if Input::paddown?(Pad::SELECT)

  return k
end


world_map.set(5,5) 
Display::clear
world_map.draw
Display::swap

loop do
  k = get_key
  if k>0
    world_map.move(k) 
    Display::clear
    world_map.draw
    Display::swap
    ESP32::System::delay 50
  end
  break if k<0
  ESP32::System::delay 10
end
puts "End of demo"
