#include <cstdio>
#include "rclcpp/rclcpp.hpp"
#include "roboprimp/msg/robot_status.hpp"

class StatusMonitor : public rclcpp::Node
{
public:
  StatusMonitor() : Node("status_monitor")
  {
    auto receive_message = [this](const roboprimp::msg::RobotStatus::UniquePtr msg) -> void
    {
      RCLCPP_INFO(this->get_logger(), "State: %s", msg->state.c_str());
      RCLCPP_INFO(this->get_logger(), "Battery: %ld", msg->battery_level);
      RCLCPP_INFO(this->get_logger(), "x: %ld", msg->x_coord);
      RCLCPP_INFO(this->get_logger(), "y: %ld", msg->y_coord);
    };

    _status_monitor = this->create_subscription<roboprimp::msg::RobotStatus>("status_topic", 10, receive_message);
  }

private:
  rclcpp::Subscription<roboprimp::msg::RobotStatus>::SharedPtr _status_monitor;
};

int main(int argc, char *argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<StatusMonitor>());
  rclcpp::shutdown();
  return 0;
}
