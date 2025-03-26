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

// Declarations for analog optosensors
AnalogInputPin right_opto(FEHIO::P2_0);
AnalogInputPin middle_opto(FEHIO::P2_1);
AnalogInputPin left_opto(FEHIO::P2_2);
FEHMotor right_motor(FEHMotor::Motor0,9.0);
FEHMotor left_motor(FEHMotor::Motor2,9.0);

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

enum LineStates {
    MIDDLE,
    RIGHT,
    LEFT, NONE
   };

void lineFollowing(int percent){
    int state = MIDDLE;

    while (true) { // I will follow this line forever!
        switch(state) {
        // If I am in the middle of the line...
        case MIDDLE:
        // Set motor powers for driving straight
        right_motor.SetPercent(percent);
        left_motor.SetPercent(-percent);
        LCD.WriteLine("Right opto");
        LCD.WriteLine(right_opto.Value());
        LCD.WriteLine("Middle Opto");
        LCD.WriteLine(middle_opto.Value());
        LCD.WriteLine("Left opto");
        LCD.WriteLine(left_opto.Value());
        /* Drive */
        if (right_opto.Value() <= 0.85) {
            state = RIGHT; // update a new state
        }
        /* Code for if left sensor is on the line */
        if (left_opto.Value() <= 0.85) {
            state = LEFT; // update a new state
        }
        if(middle_opto.Value() >= 0.3){
            state = NONE;
        }
        break;
        // If the right sensor is on the line...
        case RIGHT:
        // Set motor powers for right turn
        right_motor.SetPercent(0);
        left_motor.SetPercent(25);
        /* Drive */
        if(right_opto.Value() <= 1.6) {
        /* update a new state */
            state = MIDDLE;
        }
        break;
        // If the left sensor is on the line...
        case LEFT:
        /* Mirror operation of RIGHT state */
        // Set motor powers for right turn
        left_motor.SetPercent(0);
        right_motor.SetPercent(-25);
        /* Drive */
        if(left_opto.Value() >= 1) {
        /* update a new state */
            state = MIDDLE;
        }
        break;
        /*When there's no line */
        case NONE:
        /* Mirror operation of RIGHT state */
        // Set motor powers for right turn
        left_motor.SetPercent(0);
        right_motor.SetPercent(0);
        /* Drive */
        if(right_opto.Value() <= 0.2) {
            /* update a new state */
                state = MIDDLE;
            }
        break;
        }
    }
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
    //double red = starting();

    //LCD.WriteLine("starting: ");
    //LCD.WriteLine(red);
    lineFollowing(motor_percent);



    // backward(motor_percent, expected_counts_per_inch * 17.5);
    // Sleep(1.0);
    //LCD.WriteLine("starting: ");
    //LCD.WriteLine(red);
    return 0;
}