#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "pico/cyw43_arch.h"
#include "lwip/udp.h"
#include "lwip/pbuf.h"

#include "hwioab.h"
#include "move_cmd.h"


#define BLINK_PIN	15u
#define SSID		"wifi"
#define PASSWORD	"password"


void udp_receive_callback(
	void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
	static_cast<void>(arg);
	static_cast<void>(upcb);
	static_cast<void>(addr);
	static_cast<void>(port);

	if (p->len > 0) {
		const move_t move = get_move(static_cast<const char *>(p->payload)[0]);
		if (move != move_t::STOP) {
			static bool led_on = false;
			led_on = !led_on;
			cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_on);
		} else {
			cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
		}
	}
	pbuf_free(p);
}

int main()
{
	stdio_init_all();
	hwioab_init();
	if (cyw43_arch_init()) {
		printf("WiFi init failed");
		return -1;
	}

	cyw43_arch_enable_sta_mode();
	if (cyw43_arch_wifi_connect_timeout_ms(SSID, PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
		printf("failed to connect.\n");
		return 1;
	} else {
		printf("Connected.\n");
	}

	struct udp_pcb *upcb;
	upcb = udp_new();
	udp_bind(upcb, IP_ADDR_ANY, UDP_SERVER_PORT);
	udp_recv(upcb, udp_receive_callback, nullptr);

	// Tell the LED pin that the PWM is in charge of its value.
	::gpio_set_function(BLINK_PIN, GPIO_FUNC_PWM);
	// Figure out which slice we just connected to the LED pin
	uint slice_num = ::pwm_gpio_to_slice_num(BLINK_PIN);

	// Get some sensible defaults for the slice configuration. By default, the
	// counter is allowed to wrap over its maximum range (0 to 2**16-1)
	pwm_config config = ::pwm_get_default_config();
	// Set divider, reduces counter clock to sysclock/this value
	::pwm_config_set_clkdiv(&config, 4.f);
	// Load the configuration into our PWM slice, and set it running.
	::pwm_init(slice_num, &config, true);

	while (true) {
		static absolute_time_t led_time;
		if (absolute_time_diff_us(get_absolute_time(), led_time) < 0) {
			static bool led_on = true;
			led_on = !led_on;
			if (led_on) {
				::pwm_set_gpio_level(BLINK_PIN, 255 * 5);
			} else {
				::pwm_set_gpio_level(BLINK_PIN, 0);
			}
			led_time = make_timeout_time_ms(500);
		}
		cyw43_arch_poll();
		sleep_ms(1);
	}
}
