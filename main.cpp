#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHMotor.h>
#include <FEHBattery.h>
#include <cmath>
#include <cstdlib>

//Declarations for encoders & motors
DigitalEncoder right_encoder(FEHIO::P1_6);
DigitalEncoder left_encoder(FEHIO::P1_7);
FEHMotor right_motor(FEHMotor::Motor2,9.0);
FEHMotor left_motor(FEHMotor::Motor0,9.0);

AnalogInputPin lightsensor(FEHIO::P1_0);

void forward(int percent, int counts) //using encoders
{
    //Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    //Set both motors to desired percent
    right_motor.SetPercent(-percent*1.01);
    left_motor.SetPercent(percent);

    //While the average of the left and right encoder is less than counts,
    //keep running motors
    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts);

    LCD.Clear();
    //LCD.WriteLine(left_encoder.Counts());
    //LCD.WriteLine(right_encoder.Counts());

    //Turn off motors
    right_motor.Stop();//1507
    left_motor.Stop();//1471
}

void backward(int percent, int counts) //using encoders
{
    //Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    //Set both motors to desired percent
    right_motor.SetPercent(percent);
    left_motor.SetPercent(-percent*1.02);

    //While the average of the left and right encoder is less than counts,
    //keep running motors
    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts);

    //Turn off motors
    right_motor.Stop();
    left_motor.Stop();
}

void turnLeft(int percent, int counts) //using encoders
{
    //Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    //Set both motors to desired percent
    // might have to flip lol
    right_motor.SetPercent(-percent);
    left_motor.SetPercent(-percent);

    //While the average of the left and right encoder is less than counts,
    //keep running motors
    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts);

    //Turn off motors
    right_motor.Stop();
    left_motor.Stop();
}

void turnRight(int percent, int counts) //using encoders
{
    //Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    //Set both motors to desired percent
    // might have to flip lol
    right_motor.SetPercent(percent);
    left_motor.SetPercent(percent);

    //While the average of the left and right encoder is less than counts,
    //keep running motors
    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts);

    //Turn off motors
    right_motor.Stop();
    left_motor.Stop();
}

void lightSensor(double initRed){
    float colorValue = lightsensor.Value();
    colorValue = lightsensor.Value();

    
    
    for(int i = 0; i < 5;i++){
        //red
        if(colorValue <= 0.2){
            colorValue = lightsensor.Value();
        //robot_arm.SetDegree(0);
        }
        //blue
        else{
            colorValue = lightsensor.Value();
        //robot_arm.SetDegree(colorValue*56.25);
        }
        Sleep(1.0);
    }   
}

int starting(){
    double colorValue = lightsensor.Value();
    double startingRed = 100000000000.0;
    while(colorValue >= 0.3){
        colorValue = lightsensor.Value();
        LCD.Clear();
        LCD.WriteLine(colorValue);
        Sleep(0.5);
        startingRed = colorValue;
        LCD.Clear();
        LCD.WriteLine(startingRed);
        //LCD.WriteLine("Starting Run!");
    }
    
    return startingRed;
}


int main(void)
{
    double juice = Battery.Voltage();
    int motor_percent = (11.5/juice)*30; //Input power level here
    double expected_counts_per_inch = 40; //Input theoretical counts here 
    double degrees = (355/90); //360
    float x, y; //for touch screen

    float t_now;

    //Initialize the screen
    LCD.Clear(BLACK);
    LCD.SetFontColor(WHITE);

    
    //waiting for the light
    double red = starting();

    LCD.WriteLine("starting: ");
    LCD.WriteLine(red);

    //lining up for the ramp
    forward(motor_percent, expected_counts_per_inch * 0.25);
    Sleep(1.0);
    turnRight(motor_percent*0.5, degrees * 45);
    Sleep(1.0);   
    forward(motor_percent, expected_counts_per_inch * 3.75);
    Sleep(1.0);   
    turnRight(motor_percent*0.5, degrees * 90);   
    Sleep(1.0);   
    forward(motor_percent, expected_counts_per_inch * 5.5);
    Sleep(1.0);   
    turnLeft(motor_percent*0.5, degrees * 88.5);
    Sleep(1.0);

    // Going up the ramp
    forward(motor_percent, expected_counts_per_inch * 43);
    Sleep(1.0);

    // Lining up for clicking button
    backward(18, expected_counts_per_inch * 7);
    Sleep(1.0);
    turnRight(motor_percent*0.5, degrees * 90);
    Sleep(1.0);
    forward(motor_percent, expected_counts_per_inch * 10);
    Sleep(1.0);

    // Approaching the button
    backward(18, expected_counts_per_inch * 17.75);
    Sleep(1.0);

    double light = lightsensor.Value();

    // red
    if (light < 0.26){
        LCD.Clear();
        LCD.WriteLine(light);
        Sleep(0.5);
        LCD.SetBackgroundColor(RED);
        LCD.WriteLine("Detected Red");

        turnRight(motor_percent, degrees * 90);
        Sleep(1.0);
        backward(motor_percent, expected_counts_per_inch * 2);
        Sleep(1.0);
        turnLeft(motor_percent, degrees * 90);
        backward(motor_percent, expected_counts_per_inch * 5.5);
        
        // forward(motor_percent, expected_counts_per_inch * 7);
        // turnLeft(motor_percent, degrees * 10);

    }
    // Blue
     else if (light >= 0.3){
        LCD.Clear();
        LCD.WriteLine(light);
        Sleep(0.5);
        LCD.SetBackgroundColor(BLUE);
        LCD.WriteLine("Detected blue!");
        
        turnRight(motor_percent, degrees * 88.5);
        Sleep(1.0);
        forward(motor_percent, expected_counts_per_inch * 3);
        Sleep(1.0);
        turnLeft(motor_percent, degrees * 90);
        Sleep(1.0);
        backward(motor_percent, expected_counts_per_inch * 5.5);

        // Sleep(1.0);
        // forward(motor_percent, expected_counts_per_inch * 7);
        // Sleep(1.0);
        // turnRight(motor_percent, degrees * 10);

    }

    // backward(motor_percent, expected_counts_per_inch * 17.5);
    // Sleep(1.0);
    LCD.WriteLine("starting: ");
    LCD.WriteLine(red);






    return 0;
}