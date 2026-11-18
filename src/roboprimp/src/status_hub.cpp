#include <string.h>

#include "rclcpp/rclcpp.hpp"
#include "roboprimp/msg/robot_status.hpp"
#include "roboprimp/srv/set_status.hpp"

using namespace std::chrono_literals;

class Status : public rclcpp::Node {
 public:
  Status() : Node("status"), _status() {
    _status.status = "IDLE";
    _status.battery_level = 100;
    _status.x_coord = 0;
    _status.y_coord = 0;

    auto set_status = [this](
                          const std::shared_ptr<roboprimp::srv::SetStatus::Request> request,
                          std::shared_ptr<roboprimp::srv::SetStatus::Response> response) -> void {
      if (request->status_code < 5) {
        response->is_valid = true;

        status_codes code = static_cast<Status::status_codes>(request->status_code);
        switch (code) {
          case (status_codes::IDLE): {
            _status.status = "IDLE";
            RCLCPP_INFO(this->get_logger(), "State set to IDLE");
            break;
          }
          case (status_codes::STOPPING): {
            _status.status = "STOPPING";
            RCLCPP_INFO(this->get_logger(), "State set to STOPPING");
            break;
          }
          case (status_codes::RUNNING): {
            _status.status = "RUNNING";
            RCLCPP_INFO(this->get_logger(), "State set to RUNNING");
            break;
          }
          case (status_codes::BLOCKED): {
            _status.status = "BLOCKED";
            RCLCPP_INFO(this->get_logger(), "State set to BLOCKED");
            break;
          }
          case (status_codes::ERROR): {
            _status.status = "ERROR";
            RCLCPP_ERROR(this->get_logger(), "State is ERROR");
            break;
          }
          default: {
            break;
          }
        }

        _status_pub->publish(_status);
      } else {
        response->is_valid = false;
      }
    };

    _set_status = this->create_service<roboprimp::srv::SetStatus>("set_status", set_status);
    _status_pub = this->create_publisher<roboprimp::msg::RobotStatus>("status", 5);
  }

 private:
  rclcpp::Service<roboprimp::srv::SetStatus>::SharedPtr _set_status;
  rclcpp::Publisher<roboprimp::msg::RobotStatus>::SharedPtr _status_pub;

  enum class status_codes : uint8_t { IDLE, STOPPING, RUNNING, BLOCKED, ERROR };

  roboprimp::msg::RobotStatus _status;
};

int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<Status>());
  rclcpp::shutdown();
  return 0;
}