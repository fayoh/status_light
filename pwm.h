#ifndef PWM_H
#define PWM_H

void update_pwm(unsigned char channel, unsigned char value);

void enable_pulse(unsigned char led, unsigned char period);

void init_pwm();

#endif //PWM_H
