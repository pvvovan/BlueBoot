#include <iostream>
#include <atomic>
#include <chrono>
#include <thread>

#include "hwioab.h"

namespace iohwab
{
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
			while (!stop_required) {
				auto req = this->request.load();
				std::cout << "speed=" << req.speed  << " move=" << (int)req.move << std::endl;
				req.move = STOP;
				req.speed = 0;
				this->request.store(req);
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
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
