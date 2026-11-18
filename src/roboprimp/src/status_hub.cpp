#include <string.h>

#include "rclcpp/rclcpp.hpp"
#include "roboprimp/msg/robot_status.hpp"
#include "roboprimp/srv/set_position.hpp"
#include "roboprimp/srv/set_status.hpp"

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

    auto set_position =
        [this](const std::shared_ptr<roboprimp::srv::SetPosition::Request> request,
               std::shared_ptr<roboprimp::srv::SetPosition::Response> response) -> void {
      // BOUNDING to -100 to 100 FOR NOW
      int64_t x_coord = request->x_coord;
      int64_t y_coord = request->y_coord;

      if (x_coord >= -100 && x_coord <= 100 && y_coord >= -100 && y_coord <= 100) {
        response->is_valid = true;

        _status.x_coord = x_coord;
        _status.y_coord = y_coord;
        _status_pub->publish(_status);

        RCLCPP_INFO(this->get_logger(), "Set x: %ld, Set y:%ld", x_coord, y_coord);
      } else {
        response->is_valid = false;
        RCLCPP_ERROR(this->get_logger(), "Position out of bounds");
      }
    };

    _set_status = this->create_service<roboprimp::srv::SetStatus>("set_status", set_status);
    _set_position = this->create_service<roboprimp::srv::SetPosition>("set_position", set_position);
    _status_pub = this->create_publisher<roboprimp::msg::RobotStatus>("status", 5);
  }

 private:
  rclcpp::Service<roboprimp::srv::SetStatus>::SharedPtr _set_status;
  rclcpp::Service<roboprimp::srv::SetPosition>::SharedPtr _set_position;
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