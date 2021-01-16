#ifndef UDP_HANDLER_HPP_
#define UDP_HANDLER_HPP_

namespace cli_bot {
	enum class move {forward, left, right};

	class udp_handler {
		public:
			void run(move cmd, int speed, int seconds);
	};
}

#endif /* UDP_HANDLER_HPP_ */
