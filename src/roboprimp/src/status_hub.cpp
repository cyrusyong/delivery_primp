#include "rclcpp/rclcpp.hpp"
#include "roboprimp/srv/set_status.hpp"

class Status : public rclcpp::Node
{
public:
    Status() : Node("status"), _status(status_codes::IDLE)
    {
        auto set_status = [this](const std::shared_ptr<roboprimp::srv::SetStatus::Request> request, std::shared_ptr<roboprimp::srv::SetStatus::Response> response) -> void
        {
            if (request->status_code < 5)
            {
                this->_status = static_cast<Status::status_codes> (request->status_code);
                response->is_valid = true;
            } else {
                response->is_valid = false;
            }
            
        };
        _set_status = this->create_service<roboprimp::srv::SetStatus>("set_status", set_status);
    }

private:
    rclcpp::Service<roboprimp::srv::SetStatus>::SharedPtr _set_status;

    enum class status_codes : uint8_t
    {
        IDLE,
        STOPPING,
        RUNNING,
        BLOCKED,
        ERROR
    };

    status_codes _status;
};

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<Status>());
    rclcpp::shutdown();
    return 0;
}