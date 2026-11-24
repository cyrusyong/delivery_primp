#include <cstdio>

#include "rcl_interfaces/msg/log.hpp"
#include "rclcpp/rclcpp.hpp"
#include "roboprimp/msg/robot_status.hpp"

#define WARN 30
#define ERROR 40
#define FATAL 50

class StatusMonitor : public rclcpp::Node {
 public:
  StatusMonitor() : Node("status_monitor") {
    auto receive_message = [this](const roboprimp::msg::RobotStatus::UniquePtr msg) -> void {
      RCLCPP_INFO(this->get_logger(), "State: %s", msg->status.c_str());
      RCLCPP_INFO(this->get_logger(), "Battery: %d", msg->battery_level);
      RCLCPP_INFO(this->get_logger(), "x: %ld", msg->x_coord);
      RCLCPP_INFO(this->get_logger(), "y: %ld", msg->y_coord);
    };

    auto receive_logs = [this](const rcl_interfaces::msg::Log log) -> void {
      if (log.name == this->get_logger().get_name()) {
        return;
      }
      switch (log.level) {
        case (WARN): {
          RCLCPP_WARN(this->get_logger(), "WARN: %s", log.msg.c_str());
          break;
        }
        case (ERROR): {
          RCLCPP_ERROR(this->get_logger(), "ERROR: %s", log.msg.c_str());
          break;
        }
        case (FATAL): {
          RCLCPP_FATAL(this->get_logger(), "FATAL: %s", log.msg.c_str());
          break;
        }
        default: {
          break;
        }
      }
    };

    _status_monitor =
        this->create_subscription<roboprimp::msg::RobotStatus>("status", 10, receive_message);
    _log_monitor = this->create_subscription<rcl_interfaces::msg::Log>("/rosout", 10, receive_logs);
  }

 private:
  rclcpp::Subscription<roboprimp::msg::RobotStatus>::SharedPtr _status_monitor;
  rclcpp::Subscription<rcl_interfaces::msg::Log>::SharedPtr _log_monitor;
};

int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<StatusMonitor>());
  rclcpp::shutdown();
  return 0;
}
