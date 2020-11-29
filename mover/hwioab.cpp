#include <iostream>
#include <fstream>
#include <atomic>
#include <chrono>
#include <thread>

#include "hwioab.h"

namespace iohwab
{
	class gpio {
		std::string m_pin;
		public:
			gpio(const char* pin) {
				std::ofstream f;
				f.open("/sys/class/gpio/export");
				f << pin;
				f.flush();
				f.close();

				std::ofstream g;
				g.open(std::string{"/sys/class/gpio/gpio"} + std::string{pin} + std::string{"/direction"});
				g << "out";
				g.flush();
				g.close();

				m_pin = std::string{pin};
			}

			void set(int output) {
				std::ofstream f;
				f.open(std::string{"/sys/class/gpio/gpio"} + m_pin + std::string{"/value"});
				if (output != 0) {
					f << "1";
				} else {
					f << "0";
				}
				f.flush();
				f.close();
			}
	};

	class pwm {
		gpio m_pin;
		std::atomic<int> m_dc;
		int m_period = 100;
		int m_counter = 0;
		public:
			pwm(const char* pin) :
					m_pin{pin}
			{ }

			void set(int dc) {
				this->m_dc = dc;
			}

			void tick() {
				m_counter++;
				if (m_counter == m_dc) {
					m_pin.set(0);
				}
				if (m_counter == m_period) {
					m_pin.set(1);
					m_counter = 0;
				}
			}

			~pwm() {
				m_pin.set(0);
			}
	};

	class driver {
		pwm o1{"22"};
		pwm o2{"23"};
		pwm o3{"24"};
		pwm o4{"25"};
		std::atomic<bool> m_stop{false};
		std::thread m_thread;

		void work() {
			for ( ; ; ) {
				o1.tick();
				o2.tick();
				o3.tick();
				o4.tick();
				std::this_thread::sleep_for(std::chrono::microseconds(100));
				if (m_stop) {
					break;
				}
			}
		}

		public:
			driver() :
				m_thread{&driver::work, this}
			{ }

			~driver() {
				m_stop = true;
				m_thread.join();
			}

			void set_dc(int dc1, int dc2, int dc3, int dc4) {
				o1.set(dc1);
				o2.set(dc2);
				o3.set(dc3);
				o4.set(dc4);
			}
	};

	struct cmd_t {
		enum move_t move;
		int speed;
	};

	class handle final {
		public:
		handle() {
			this->stop_required.store(false);
			this->start();
		}
		~handle() {
			this->stop_required.store(true);
			this->thr.join();
		}
		void set_cmd(cmd_t cmd) {
			this->request.store(cmd);
		}

		private:
		std::thread thr{};
		void start() {
			cmd_t cmd;
			cmd.speed = 0;
			cmd.move = STOP;
			request.store(cmd);
			thr = std::thread{&handle::do_handling, this};
		}

		std::atomic<cmd_t> request{};
		std::atomic<bool> stop_required{};
		void do_handling() {
			driver drv;
			while (!stop_required) {
				int dc = 0;
				auto req = this->request.load();
				std::cout << "speed=" << req.speed  << " move=" << (int)req.move << std::endl;
				switch (req.move) {
					case FORWARD:
						dc = static_cast<int>(static_cast<double>(req.speed) * 100 / 15);
						drv.set_dc(dc, dc, 0, 0);
						break;
					default:
						drv.set_dc(0, 0, 0, 0);
						break;
				}
				req.move = STOP;
				req.speed = 0;
				this->request.store(req);
				std::this_thread::sleep_for(std::chrono::milliseconds(3000));
			}
		}
	};

	static handle s_handler;
}

void hwioab_output(int speed, enum move_t move)
{
	iohwab::cmd_t cmd;
	cmd.move = move;
	cmd.speed = speed;
	iohwab::s_handler.set_cmd(cmd);
}
