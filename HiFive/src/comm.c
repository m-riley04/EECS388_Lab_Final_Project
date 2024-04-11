#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "eecs388_lib.h"

void auto_brake(int devid)
{
    // Task-1: 
    // Your code here (Use Lab 02 - Lab 04 for reference)
    // Use the directions given in the project document

    /*
    Distance                Action              LED Color
    ======================================================
    >200cm                  No breaking         Green
    100cm < d <= 200cm      Break lightly       Red and green
    60cm < d <= 100cm       Break hard          Red
    d <= 60cm               Must stop           Flashing red (100ms blinks)
    */

   // Initialize the varaibles
   uint8_t dist_L   = 0;
   uint8_t dist_H   = 0;
   uint16_t dist    = 0;
   int gpio         = RED_LED;

   // Check first 2 bytes to ensure frame headers exist and are valid
   if ('Y' == ser_read(devid) && 'Y' == ser_read(devid)) {
        // Read dist L byte
        dist_L = ser_read(devid);
        // Read dist_H byte
        dist_H = ser_read(devid);

        // Build the distance value
        dist = (dist_H << 8) | dist_L;


        //=== READING DISTANCE
        if (dist > 200) {
            //== No breaking - turn green on and others off
            gpio_write(GREEN_LED, ON);
            gpio_write(RED_LED, OFF);

        } else if (100 < dist && dist <= 200) {
            //== Break lightly - turn red and green on and others off
            gpio_write(GREEN_LED, ON);
            gpio_write(RED_LED, ON);

        } else if (60 < dist && dist <= 100) {
            //== Break hard - turn red on and others off
            gpio_write(GREEN_LED, OFF);
            gpio_write(RED_LED, ON);

        } else {
            //== Must stop - flash red and turn other colors off
            gpio_write(GREEN_LED, OFF);
            gpio_write(RED_LED, ON);
            delay(100);
            gpio_write(RED_LED, ON);
            delay(100);
        }

        // OPTIONAL - Print the distance
        printf("Distance: %u cm \n", dist);
   }
}

int read_from_pi(int devid)
{
    // Task-2: 
    // You code goes here (Use Lab 09 for reference)
    // After performing Task-2 at dnn.py code, modify this part to read angle values from Raspberry Pi.

}

void steering(int gpio, int pos)
{
    // Task-3: 
    // Your code goes here (Use Lab 05 for reference)
    // Check the project document to understand the task

    // Basically, you need to take the input angle "pos" and generate the
    // To generate the pwm signals, use gpio_write() and delay_usec()
    

    // Define the servo pulses
    int SERVO_PULSE_MAX = 2400; // 2400 us
    int SERVO_PULSE_MIN = 544; // 544 us
    int SERVO_PERIOD = 20000; // 20000 us (or 20ms)


    // Gets the pulse rate in pulses per nanosecond
    int pulse_rate = (SERVO_PULSE_MAX - SERVO_PULSE_MIN) / 180;
    // Get the current position's pulse rate
    int position_rate = (pulse_rate) * pos;
    // Must account for the SERVO_PULSE_MIN that was subtracted initially. The total duty delay.
    int duty_delay = position_rate + SERVO_PULSE_MIN;
    // Turn the GPIO pin on
    gpio_write(gpio, ON);
    // Delay by the duty amount
    delay_usec(duty_delay);
    // Turn the GPIO pin off
    gpio_write(gpio, OFF);
    // Delay for the servo period (1 minute = 1000000 microseconds, 1000000 microseconds / 50 loops = 20000 microseconds, or 20 milliseconds)
    delay_usec(SERVO_PERIOD);
}


int main()
{
    // initialize UART channels
    ser_setup(0); // uart0
    ser_setup(1); // uart1
    int pi_to_hifive = 1; //The connection with Pi uses uart 1
    int lidar_to_hifive = 0; //the lidar uses uart 0
    
    printf("\nUsing UART %d for Pi -> HiFive", pi_to_hifive);
    printf("\nUsing UART %d for Lidar -> HiFive", lidar_to_hifive);
    
    //Initializing PINs
    gpio_mode(PIN_19, OUTPUT);
    gpio_mode(RED_LED, OUTPUT);
    gpio_mode(BLUE_LED, OUTPUT);
    gpio_mode(GREEN_LED, OUTPUT);

    printf("Setup completed.\n");
    printf("Begin the main loop.\n");

    while (1) {

        auto_brake(lidar_to_hifive); // measuring distance using lidar and braking
        int angle = read_from_pi(pi_to_hifive); //getting turn direction from pi
        printf("\nangle=%d", angle) 
        int gpio = PIN_19; 
        for (int i = 0; i < 10; i++){
            // Here, we set the angle to 180 if the prediction from the DNN is a positive angle
            // and 0 if the prediction is a negative angle.
            // This is so that it is easier to see the movement of the servo.
            // You are welcome to pass the angle values directly to the steering function.
            // If the servo function is written correctly, it should still work,
            // only the movements of the servo will be more subtle
            if(angle>0){
                steering(gpio, 180);
            }
            else {
                steering(gpio,0);
            }
            
            // Uncomment the line below to see the actual angles on the servo.
            // Remember to comment out the if-else statement above!
            // steering(gpio, angle);
        }

    }
    return 0;
}
