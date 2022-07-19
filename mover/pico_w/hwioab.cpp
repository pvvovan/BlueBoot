#include "hwioab.h"

#include <pico/stdlib.h>
#include <hardware/pwm.h>
#include <array>


static constexpr int PWM_FL {14};
static constexpr int PWM_RL {15};
static constexpr int PWM_RR {16};
static constexpr int PWM_FR {17};

constexpr double PWM_PERIOD {256 * 256 - 1};

void hwioab_init()
{
	constexpr std::array<int, 4> pwm_pins {PWM_FL, PWM_RL, PWM_RR, PWM_FR};
	for (const auto& pin : pwm_pins) {
		// Tell the LED pin that the PWM is in charge of its value.
		::gpio_set_function(pin, GPIO_FUNC_PWM);
		// Figure out which slice we just connected to the LED pin
		uint slice_num = ::pwm_gpio_to_slice_num(pin);

		// Get some sensible defaults for the slice configuration. By default, the
		// counter is allowed to wrap over its maximum range (0 to 2**16-1)
		pwm_config config = ::pwm_get_default_config();
		// Set divider, reduces counter clock to sysclock/this value
		::pwm_config_set_clkdiv(&config, 4.f);
		// Load the configuration into our PWM slice, and set it running.
		::pwm_init(slice_num, &config, true);
		// Set duty cycle to 100%
		::pwm_set_gpio_level(pin, PWM_PERIOD);
	}
}

static void set_dc(int fl, int fr, int rl, int rr)
{
	::pwm_set_gpio_level(PWM_FL, fl);
	::pwm_set_gpio_level(PWM_FR, fr);
	::pwm_set_gpio_level(PWM_RL, rl);
	::pwm_set_gpio_level(PWM_RR, rr);
}

void hwioab_output(int speed, enum move_t move)
{
	constexpr double max_speed {15};
	uint16_t dc = static_cast<uint16_t>(PWM_PERIOD - (speed * PWM_PERIOD / max_speed));

	switch (move) {
		case FORWARD:
			::set_dc(dc, dc, 0, 0);
			break;
		case BACKWARD:
			::set_dc(0, 0, dc, dc);
			break;
		case LEFT:
			::set_dc(0, dc, 0, 0);
			break;
		case RIGHT:
			::set_dc(dc, 0, 0, 0);
			break;
		case FAST_RIGHT:
			::set_dc(dc, 0, 0, dc);
			break;
		case FAST_LEFT:
			::set_dc(0, dc, dc, 0);
			break;
		default:
			::set_dc(0, 0, 0, 0);
			break;
	}
}
