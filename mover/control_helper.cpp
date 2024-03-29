#include <string>
#include <fstream>
#include <atomic>
#include <thread>

#include "control_helper.hpp"

namespace control_helper
{
	gpio::gpio(const char* pin) : m_pin{pin} {
		std::ofstream f;
		f.open("/sys/class/gpio/export");
		f << pin;
		f.flush();
		f.close();
		std::this_thread::sleep_for(std::chrono::milliseconds(300));

		std::ofstream g;
		g.open(std::string{"/sys/class/gpio/gpio"} + std::string{pin} + std::string{"/direction"});
		g << "out";
		g.flush();
		g.close();
		std::this_thread::sleep_for(std::chrono::milliseconds(300));
	}

	void gpio::set(int output) {
		std::ofstream f;
		f.open(std::string{"/sys/class/gpio/gpio"} + m_pin + std::string{"/value"});
		if (output == 1) {
			f << "0";
		} else {
			f << "1";
		}
		f.flush();
		f.close();
	}

	pwm::pwm(const char* pin) : m_pin{pin} { }

	void pwm::set(int dutycycle) {
		this->m_dutycycle = dutycycle;
	}

	void pwm::tick() {
		++m_counter;
		if (m_counter >= period) {
			m_counter = 0;
		}
		if ((m_counter <= m_dutycycle) && (m_dutycycle > 0)) {
			m_pin.set(1);
		} else {
			m_pin.set(0);
		}
	}

	pwm::~pwm() {
		m_pin.set(0);
	}

	void driver::work() {
		for ( ; ; ) {
			o1.tick();
			o2.tick();
			o3.tick();
			o4.tick();
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			if (m_stop) {
				break;
			}
		}
	}

	driver::driver() :
		m_thread{&driver::work, this}
	{ }

	driver::~driver() {
		m_stop = true;
		m_thread.join();
	}

	void driver::set_dc(int dc1, int dc2, int dc3, int dc4) {
		o1.set(dc1);
		o2.set(dc2);
		o3.set(dc3);
		o4.set(dc4);
	}

	handle::handle() {
		this->stop_required.store(false);
		this->start();
	}

	handle::~handle() {
		this->stop_required.store(true);
		this->thr.join();
	}

	void handle::set_cmd(cmd_t cmd) {
		this->request.store(cmd);
		this->request_cnt.store(0);
	}

	void handle::start() {
		cmd_t cmd;
		cmd.speed = 0;
		cmd.move = STOP;
		request.store(cmd);
		thr = std::thread{&handle::do_handling, this};
	}

	void handle::do_handling() {
		constexpr double max_speed = 15.0;
		constexpr double turn_limit = static_cast<double>(pwm::period) / 1.0;
		driver drv{};
		while (!stop_required) {
			int dc = 0;
			auto req = this->request.load();
			switch (req.move) {
				case FORWARD:
					dc = static_cast<int>(static_cast<double>(req.speed) * pwm::period / max_speed);
					drv.set_dc(dc, dc, 0, 0);
					break;
				case BACKWARD:
					dc = static_cast<int>(static_cast<double>(req.speed) * turn_limit / max_speed);
					drv.set_dc(0, 0, dc, dc);
					break;
				case LEFT:
					dc = static_cast<int>(static_cast<double>(req.speed) * turn_limit / max_speed);
					drv.set_dc(0, dc, 0, 0);
					break;
				case RIGHT:
					dc = static_cast<int>(static_cast<double>(req.speed) * turn_limit / max_speed);
					drv.set_dc(dc, 0, 0, 0);
					break;
				case FAST_RIGHT:
					dc = static_cast<int>(static_cast<double>(req.speed) * turn_limit / max_speed);
					drv.set_dc(dc, 0, 0, dc);
					break;
				case FAST_LEFT:
					dc = static_cast<int>(static_cast<double>(req.speed) * turn_limit / max_speed);
					drv.set_dc(0, dc, dc, 0);
					break;
				default:
					drv.set_dc(0, 0, 0, 0);
					break;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			constexpr int request_timeout {1000};
			request_cnt++;
			if (request_cnt > request_timeout) {
				request_cnt = request_timeout;
				req.move = STOP;
				req.speed = 0;
				this->request.store(req);
			}
		}
	}
}
