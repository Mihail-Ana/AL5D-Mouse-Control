#include <Servo.h>
#include <Math.h>

Servo Base;
Servo Shoulder;
Servo Elbow;
Servo Wrist;
Servo Gripper;
Servo WristRot;

int pos, pos1, pos2, i, f, NewPos;
char KStroke;
int kps, kpe, kpw, kpb, kpr, kpg; 
int ypos = 0;
int xpos= 0;
int CoordX;
int CoordY;
double L1 = 145;  //Link 1 in mm
double L2 = 185;  //Link 2 in mm
double L3 = 100;  //Link 3 in mm
double omega2;    //Angle of CLW
double omega2rad = 0;
double omega2con;
double base_twist = 90;
double R;
double F;
double safety = 20; //Safety for not overshooting (Recommended minim 20)
double minimum;
double maximum;
bool InsideCircle = true; 
bool BaseRot = true;

double r2d(double rad)
{
  double deg = (rad * 4068) / 71;
  return deg;  
}

double d2r(double deg)
{
  double rad = (deg * 71) / 4068;
  return rad; 
}


void Mouse_control(Servo X = Base, Servo Y = Shoulder)
{
  if ( Serial.available())
  {
    char ch = Serial.read(); // read in a character from the serial port and assign to ch
  }
}

double convert1(int omega2)
{
  double var1 = round(-0.45 * omega2 + 90);
  return var1;
}

void IK(double x, double y, double omega2, double base_twist)
{
  Servo S = Shoulder;
  Servo E = Elbow;
  Servo W = Wrist;
  Servo B = Base;
  int omega2con = convert1(omega2);
  double omega2rad = d2r(omega2con);
  double beta;
  double F = sqrt(sq(L2) + sq(L3) + (2 * L2 * L3 * cos(omega2rad)));
  double alpha = 90 + r2d(acos((sq(L1)-sq(F)+sq(x)+sq(y)) / (2 * L1 * sqrt(sq(x)+sq(y)))) - atan(x/y));
  double megabeta = 90 + r2d(asin((sq(L1)+sq(F)-sq(x)-sq(y)) / (2 * L1 * F)));
  double delta = r2d(acos((sq(L2)+sq(F)-sq(L3))/(2 * L2 * F)));
  if (omega2con <= 0)
  {
    beta = megabeta - delta;
  }
  else
  {
    beta = megabeta + delta;
  }
  
  S.write(alpha);
  E.write(beta);
  W.write(90+omega2con);
  B.write(base_twist);
  
}

void inside_circle(double x, double y, double omega2rad)
{
  omega2con = convert1(omega2);
  omega2rad = d2r(omega2con);
  R = sqrt(sq(x) + sq(y));
  F = sqrt(sq(L2) + sq(L3) + (2 * L2 * L3 * cos(omega2rad)));
  safety = 20; //Safety for not overshooting (Recommended minim 20)
  minimum = 150;
  maximum = L1 + F - safety;
  
  if (R > maximum || R < minimum)
  {
    InsideCircle = false; 
  }
  else
  {
    InsideCircle = true;
  }
}

void base_rot(double base_twist)
{
  if (base_twist > 180 || base_twist < 0)
  {
    BaseRot = false;
  }
  else
  {
    BaseRot = true;
  }
}

void Mouse_Control(Servo X = Base, Servo Y = Shoulder)
{
  static int v = 0; // value to be sent to the servo (0-180)
  if ( Serial.available())
  {
    char ch = Serial.read(); // read in a character from the serial port and assign to ch
    switch(ch)
    { 
      case '0'...'9': // if it's numeric
        v = v * 10 + ch - '0';
        break;
        
      case 'x':
        CoordX = v;
        v = 0;
        break;
        
      case 'y':
        CoordY = v;
        v = 0;
        break;       

      case 'w':
        base_twist = v;
        v = 0;
        break;
        
      case 'z':
        omega2 = v;
        inside_circle(CoordX, CoordY, omega2);
        base_rot(base_twist);
        if (InsideCircle && BaseRot)
        {
          IK(CoordX, CoordY, omega2, base_twist);
          v = 0;
          CoordX = 0;
          CoordY = 0;
          omega2 = 0;
          base_twist = 0;
          break;
        }
        else
        {
          v = 0;
          IK(200, 200, 200, 90);
          CoordX = 0;
          CoordY = 0;
          omega2 = 0;
          base_twist = 0;
          break;
        }
        
      case 'a':
        Gripper.write(20);
        break;

      case 'b':
        Gripper.write(180);
        break;
    }
  }
}


void setup()                                                        //Setup of Code. This happens while LOGIC "Loads"
{
  Serial.begin(19200);                                                 // Serial comm begin at XXXXXX bps
  Base.write(90);
  Shoulder.write(90);
  Elbow.write(90);
  Wrist.write(90);
  Gripper.write(180);
  WristRot.write(90);
  

  
 
  Shoulder.attach(2);                                                    //Shoulder >90 is BACK, <90 is FRONT  of Center                                                  
  Base.attach(3);                                                       //Base >90 is RIGHT, <90 is LEFT of Center                                                      
  Elbow.attach(4);                                                      //Elbow >90 is DOWN, <90 is UP of Center                                                     
  Wrist.attach(10);                                                     //Wrist >90 is UP, <90 is DOWN  of Center                                                                                                
  WristRot.attach(11);                                                   //WristRot >90 is COUNTER CLOCKWISE FROM ARM POV, <90 is CLOCKWISE FROM ARM POV  of Center
  Gripper.attach(12);                                                   //Gripper >90 is CLOSE, <90 is OPEN of Center
                                                                          
}

void loop()                                                       //What the Arm will do once Logic and Servo are powered. If Logic is powered without Servo, the code will still iterate and the once Servo gains power the servos will shoot to current position in itteration. 
{
  Mouse_Control();  
}
