#ifndef CONTROL_HELPER_HPP_
#define CONTROL_HELPER_HPP_

#include <thread>
#include <atomic>

#include "hwioab.h"


namespace control_helper
{
	class gpio final {
		private:
			std::string m_pin{};

		public:
			gpio(const char* pin);
			void set(int output);
	};

	class pwm final {
		private:
			gpio m_pin;
			std::atomic<int> m_dutycycle{};
			int m_counter{0};

		public:
			static constexpr int period{1};
			pwm(const char* pin);
			void set(int dutycycle);
			void tick();
			~pwm();
	};

	class driver final {
		private:
			pwm o1{"22"};
			pwm o2{"23"};
			pwm o3{"24"};
			pwm o4{"25"};
			std::atomic<bool> m_stop{false};
			std::thread m_thread{};

			void work();

		public:
			driver();
			~driver();
			void set_dc(int dc1, int dc2, int dc3, int dc4);
	};

	struct cmd_t {
		enum move_t move{};
		int speed{};
	};

	class handle final {
		private:
			std::thread thr{};
			std::atomic<cmd_t> request{};
			std::atomic<bool> stop_required{};

			void start();
			void do_handling();

		public:
			handle();
			~handle();
			void set_cmd(cmd_t cmd);
	};
}

#endif /* CONTROL_HELPER_HPP_ */
