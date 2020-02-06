import processing.serial.*;       

int MouseClk = 'b'; 
int valueL = 0;
int valueR = 0;
int L1 = 145;
int L2 = 185;
int L3 = 100;
boolean Rclick = false;
boolean Lclick = false;
float e;
float f;
int wheel = 200;
int wheel_twist = 200; 
float omega2rad = 0;
float F;
float safety = 0; //Safety for not overshooting (Recommended minim 20)
float Min_Range;
float Max_Range;
int L;
int H;
int lastX;
int lastY;
int scrntype = 1;
int base_spin = 90;
int lastbs = base_spin;
Serial port; // The serial port 

void setup()
{
  L = 450;
  H = 450;
  size(450, 450);
  frameRate(100);
  println(Serial.list()); // List COM-ports
  //select appropriate COM
  port = new Serial(this, Serial.list()[0], 19200);
  //change between [0], [1]
}

void draw()
{
  clicks();
}



void mouseWheel(MouseEvent event) 
{
  e = event.getCount();
  while (wheel > 0 && wheel < H)
  {
    if (e > 0)
    {
      wheel += 10;
      //println(wheel);
      break;
    }
    else
    {
      wheel -= 10;
      //println(wheel);
      break;
    }
  }
  if (wheel <= 0)
  {
    wheel = 10;
  }
  else if (wheel >= H)
  {
    wheel = H-10;
  }
}

void screen1()
{
  scrntype = 1;
  fill(255, 0, 0);
  rect(0,0,L,H);
  fill(130, 230, 117);
  circle(0,L, Max_Range);
  fill(255, 0, 0);
  circle(0, L, Min_Range);
  fill(valueL);
  rect(25, 25, 50, 50);
  fill(0, 0, 255);
  rect(L-15, H, 10, - H + wheel);
}

void screen2()
{
  scrntype = 2;
  fill(125);
  rect(0,0,L,H);
  fill(0, 255, 0);
  circle(L/2, H/2, L);
  fill(125);
  rect(0, H,L, -H/2);
  fill(0, 0, 255);
  circle(L/2, H/2, 20);
  fill(0, 255, 0);
  line(L/2, H/2, L/2, -H);
  line(L/2, H/2, mouseX, mouseY);
}
  
void clicks()
{
  if (mousePressed && mouseButton == LEFT)
  {
    valueL = 0;
    MouseClk = 'a';
  } 
  else
  {
    valueL = 100;
    MouseClk = 'b';
  }
  if (mousePressed && mouseButton == RIGHT) 
  {
    screen2();
    base_angle(mouseX - H/2, L/2 - mouseY);
    println(lastX, lastY, wheel, base_spin);
    updateXY(lastX, lastY, wheel, base_spin);
  } 
  else 
  {
    screen1();
    float omega2converted = convert1(wheel);
    float omega2rad = d2r(omega2converted);
    F = sqrt(sq(L2) + sq(L3) + (2 * L2 * L3 * cos(omega2rad)));
    safety = 20; //Safety for not overshooting (Recommended minim 20)
    Min_Range = 300;
    Max_Range = 2*(L1 + F - safety);
    println(mouseX, -mouseY+L, wheel, lastbs);
    updateXY(mouseX, -mouseY+L, wheel, lastbs);
  }
}

int convert1(int wheel)
{
  int wheelrad = round(-0.45 * wheel + 90);
  return wheelrad;
}

float r2d(float rad)
{
  float deg = (rad * 4068) / 71;
  return deg;  
}

float d2r(float deg)
{
  float rad = (deg * 71) / 4068;
  return rad; 
}


void updateXY(int x, int y, int wheel, int base_spin)
{
  lastX = x;
  lastY = y;
  lastbs = base_spin;
  port.write(x + "x");
  port.write(y + "y");
  port.write(wheel + "z");
  port.write(base_spin + "w");
  
  if (MouseClk == 'a')
  {
    port.write('a');
  }
  else
  {
    port.write('b');
  }
}

void base_angle(int x, int y)
{
  float DistX = x;
  float DistY = y;
  float bs1 = atan(DistX/DistY);
  base_spin = round(r2d(bs1)) + 90;
}
