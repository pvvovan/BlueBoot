#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/udp.h"
#include "lwip/pbuf.h"

#include "hwioab.h"
#include "move_cmd.h"


#define BLINK_PIN	15u
#define SSID		"wifi"
#define PASSWORD	"password"

static long s_timecnt {0};
static constexpr long TIMEOUT {1000};

void udp_receive_callback(
	void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
	static_cast<void>(arg);
	static_cast<void>(upcb);
	static_cast<void>(addr);
	static_cast<void>(port);

	if (p->len > 0) {
		const cmd_t cmd = static_cast<const unsigned char *>(p->payload)[0];
		const move_t move = get_move(cmd);
		const int speed = get_speed(cmd);
		hwioab_output(speed, move);
		s_timecnt = 0;
	}
	pbuf_free(p);
}

int main()
{
	asm volatile ("bkpt #1");

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

	while (true) {
		static absolute_time_t led_time;
		if (absolute_time_diff_us(get_absolute_time(), led_time) < 0) {
			static bool led_on = true;
			led_on = !led_on;
			cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_on);
			led_time = make_timeout_time_ms(500);
		}
		cyw43_arch_poll();
		sleep_ms(1);
		s_timecnt++;
		if (s_timecnt > TIMEOUT) {
			hwioab_output(0, move_t::STOP);
		}
	}
}
