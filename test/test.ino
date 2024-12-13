#include "esp_encoder_motor.h"
#include "esp_servo.h"

static em::EncoderMotor* const g_encoder_motors[] = {
    /**
     * E0:
     * +: GPIO 27
     * -: GPIO 13
     * A: GPIO 18
     * B: GPIO 19
     */
    new em::EncoderMotor(
        GPIO_NUM_27, GPIO_NUM_13, GPIO_NUM_18, GPIO_NUM_19, 12, 90, em::EncoderMotor::PhaseRelation::kAPhaseLeads),
    /**
     * E1:
     * +: GPIO 4
     * -: GPIO 2
     * A: GPIO 5
     * B: GPIO 23
     */
    new em::EncoderMotor(
        GPIO_NUM_4, GPIO_NUM_2, GPIO_NUM_5, GPIO_NUM_23, 12, 90, em::EncoderMotor::PhaseRelation::kAPhaseLeads),

    /**
     * E2:
     * +: GPIO 17
     * -: GPIO 12
     * A: GPIO 35
     * B: GPIO 16
     */
    new em::EncoderMotor(
        GPIO_NUM_17, GPIO_NUM_12, GPIO_NUM_35, GPIO_NUM_36, 12, 90, em::EncoderMotor::PhaseRelation::kAPhaseLeads),

    /**
     * E3:
     * +: GPIO 15
     * -: GPIO 14
     * A: GPIO 34
     * B: GPIO 39
     */
    new em::EncoderMotor(
        GPIO_NUM_15, GPIO_NUM_14, GPIO_NUM_34, GPIO_NUM_39, 12, 90, em::EncoderMotor::PhaseRelation::kAPhaseLeads),
};

static em::EspServo* const g_servos[] = {
    new em::EspServo(25),
    new em::EspServo(26),
    new em::EspServo(32),
    new em::EspServo(33),
};

void setup() {
  Serial.begin(115200);
  printf("setting up\n");
  for (auto servo : g_servos) {
    servo->Init();
    servo->Write(0);
  }

  for (auto encoder_motor : g_encoder_motors) {
    encoder_motor->Init();
    encoder_motor->RunPwmDuty(1023);
  }

  printf("setup completed\n");
}

void loop() {
  static auto s_pwm_duty_time = millis();

  static uint32_t s_servo_angle = 0;
  static auto s_servo_trigger_time = millis();

  if (millis() - s_servo_trigger_time > 1000) {
    s_servo_trigger_time = millis();
    s_servo_angle += 90;
    if (s_servo_angle > 180) {
      s_servo_angle = 0;
    }
    for (auto servo : g_servos) {
      servo->Write(s_servo_angle);
    }
  }

  if (millis() - s_pwm_duty_time > 2000) {
    s_pwm_duty_time = millis();

    for (auto encoder_motor : g_encoder_motors) {
      encoder_motor->RunPwmDuty(-encoder_motor->PwmDuty());
    }
  }

  printf("speed(RPM):[%4" PRId32 ", %4" PRId32 ", %4" PRId32 ", %4" PRId32 "], pwm duty:[%5 " PRIi16 ", %5" PRIi16 ", %5" PRIi16
         ", %5" PRIi16 "], pulse counts:[%" PRId64 ", %" PRId64 ", %" PRId64 ", %" PRId64 "]\n",
         g_encoder_motors[0]->Rpm(),
         g_encoder_motors[1]->Rpm(),
         g_encoder_motors[2]->Rpm(),
         g_encoder_motors[3]->Rpm(),
         g_encoder_motors[0]->PwmDuty(),
         g_encoder_motors[1]->PwmDuty(),
         g_encoder_motors[2]->PwmDuty(),
         g_encoder_motors[3]->PwmDuty(),
         g_encoder_motors[0]->EncoderPulseCount(),
         g_encoder_motors[1]->EncoderPulseCount(),
         g_encoder_motors[2]->EncoderPulseCount(),
         g_encoder_motors[3]->EncoderPulseCount());
  delay(100);
}