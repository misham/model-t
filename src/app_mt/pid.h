#ifndef PID_H
#define PID_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>
#include "ch.h"
#include "types.h"
#include "sensor.h"


typedef enum {
  POSITIVE,
  NEGATIVE
} pid_output_dir_t;

typedef struct {
  bool enabled;
  bool auto_mode;

  float kp;
  float ki;
  float kd;

  float err_i;
  float err_i_tune;
  float last_err;
  float last_sample;

  float out;
  float out_min;
  float out_max;
  int8_t output_sign;

  /* Time is in system ticks */
  systime_t sample_time;
  systime_t last_time;
} pid_controller_t;


void pid_init(pid_controller_t* pid);
void pid_exec(pid_controller_t* pid, float setpoint, float sample);
void pid_set_gains(pid_controller_t* pid, float Kp, float Ki, float Kd);
void tune_gains(pid_controller_t* pid, float err_p, float err_d);
void pid_enable(pid_controller_t* pid, float sample, bool enabled);
void pid_reinit(pid_controller_t* pid, float sample);
void pid_set_output_sign(pid_controller_t* pid, uint8_t direction);
void pid_set_output_limits(pid_controller_t* pid, float Min, float Max);

#endif
