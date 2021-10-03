#include <vector>
#include <string>

namespace reop {
    enum container_type {FUEL, COOLANT};
    class reactor {
        public:
        bool isRunning = false;
        int temp = 0;
        int fuel = 0;
        int coolant = 0;
        reactor();
    };
    class robot {
        public:
        int coolant = 0;
        int fuel = 0;
        int pc = 0;
        int x;
        int y;
        std::vector<std::string> program;
        std::vector<std::string> interrupt_handler;
        bool isInterruptControllerInstalled = false;
        robot();
    };
}