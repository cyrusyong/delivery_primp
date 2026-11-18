#include <string.h>

#include <chrono>

#include "rclcpp/rclcpp.hpp"
#include "roboprimp/srv/set_status.hpp"
#include "std_msgs/msg/string.hpp"

using namespace std::chrono_literals;

class Status : public rclcpp::Node {
 public:
  Status() : Node("status"), _status(status_codes::IDLE) {
    auto set_status = [this](
                          const std::shared_ptr<roboprimp::srv::SetStatus::Request> request,
                          std::shared_ptr<roboprimp::srv::SetStatus::Response> response) -> void {
      if (request->status_code < 5) {
        this->_status = static_cast<Status::status_codes>(request->status_code);
        response->is_valid = true;
      } else {
        response->is_valid = false;
      }
    };

    auto timer_callback = [this]() -> void {
      switch (_status) {
        case (status_codes::IDLE): {
          RCLCPP_INFO(this->get_logger(), "STATE: IDLE");
          break;
        }
        case (status_codes::STOPPING): {
          RCLCPP_INFO(this->get_logger(), "STATE: STOPPING");
          break;
        }
        case (status_codes::RUNNING): {
          RCLCPP_INFO(this->get_logger(), "STATE: RUNNING");
          break;
        }
        case (status_codes::BLOCKED): {
          RCLCPP_INFO(this->get_logger(), "STATE: BLOCKED");
          break;
        }
        case (status_codes::ERROR): {
          RCLCPP_ERROR(this->get_logger(), "STATE: ERROR");
          break;
        }
        default: {
          RCLCPP_ERROR(this->get_logger(), "INVALID STATE");
          break;
        }
      }
    };

    _set_status = this->create_service<roboprimp::srv::SetStatus>("set_status", set_status);
    _status_pub = this->create_publisher<std_msgs::msg::String>("status", 5);
    _timer = this->create_wall_timer(1000ms, timer_callback);
  }

 private:
  rclcpp::Service<roboprimp::srv::SetStatus>::SharedPtr _set_status;
  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr _status_pub;
  rclcpp::TimerBase::SharedPtr _timer;

  enum class status_codes : uint8_t { IDLE, STOPPING, RUNNING, BLOCKED, ERROR };

  status_codes _status;
};

int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<Status>());
  rclcpp::shutdown();
  return 0;
}