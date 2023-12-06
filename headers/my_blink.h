#define PERIOD 1000

void wait_microseconds(int32_t us);
void smooth_blink(int32_t led, int32_t duration_ms, volatile bool * enable);
void smooth_blink_any_times(int32_t led , int32_t num, int32_t period_ms, volatile bool *enable);
