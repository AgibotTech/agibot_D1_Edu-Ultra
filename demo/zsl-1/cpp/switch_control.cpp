#include "zsibot_api.h"
#include <bitset>
#include <fcntl.h>
#include <iostream>
#include <termios.h>
#include <thread>
#include <unistd.h>

using namespace zsibot;

// 设置终端为非阻塞模式
void set_conio_terminal_mode()
{
    struct termios new_termios;
    tcgetattr(STDIN_FILENO, &new_termios);
    new_termios.c_lflag &= ~(ICANON | ECHO); // 关闭规范模式和回显
    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
}

// 检查是否有输入
int kbhit()
{
    struct termios oldt, newt;
    int oldf;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    int ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF)
    {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}

int main()
{
    set_conio_terminal_mode(); // 设置终端为非阻塞模式

    // 使用默认的机器狗IP 192.168.234.1  使用默认的发送端口 8081 , 默认的接收端口8080, 此程序的的角色 ROLE_SDK
    ZsibotExecutor zsibot_exec(Role::ROLE_SDK);

    // 显示设备基本信息
    std::cout << "设备序列号: " << zsibot_exec.GetSn() << std::endl;
    std::cout << "设备名称: " << zsibot_exec.GetDevName() << std::endl;
    // std::cout << "当前电量: " << zsibot_exec.GetPower() << "%" << std::endl;
    // std::cout << "设备温度: " << zsibot_exec.GetTemperature() << "°C" << std::endl;
    // std::cout << "WiFi SSID: " << zsibot_exec.GetWifiSsid() << std::endl;

    VersionInfo version = zsibot_exec.GetVersion();
    std::cout << "主控版本: " << version.mc_version << std::endl;
    std::cout << "任务版本: " << version.dog_task_version << std::endl;

    // 按键状态跟踪
    std::bitset<256> pressed_keys;

    while (true)
    {
        // 检测键盘输入
        if (kbhit())
        {
            char ch = getchar(); // 获取按键

            // 记录按键状态
            pressed_keys.set(static_cast<unsigned char>(ch), true);

            switch (ch)
            {
            case 'w': // 向前移动
                zsibot_exec.SetRemote({0.5, 0, 0, 0}, std::array<float32_t, 14>{0});
                break;
            case 's': // 向后移动
                zsibot_exec.SetRemote({-0.5, 0, 0, 0}, std::array<float32_t, 14>{0});
                break;
            case 'a': // 向左移动
                zsibot_exec.SetRemote({0, 0, 0.5, 0}, std::array<float32_t, 14>{0});
                break;
            case 'd': // 向右移动
                zsibot_exec.SetRemote({0, 0, -0.5, 0}, std::array<float32_t, 14>{0});
                break;
            case 'q': // 左转
                zsibot_exec.SetRemote({0, 0.5, 0, 0}, std::array<float32_t, 14>{0});
                break;
            case 'e': // 右转
                zsibot_exec.SetRemote({0, -0.5, 0, 0}, std::array<float32_t, 14>{0});
                break;
            case 'c': // 停止移动
                zsibot_exec.SetRemote({0, 0, 0, 0}, std::array<float32_t, 14>{0});
                break;
            case '0': // 切换到失能状态，机器狗软急停,完全爬下
                zsibot_exec.SetCmd(CmdCode::CMD_EMERGENCY_STOP);
                break;
            case '1': // 切换到匍匐
                zsibot_exec.SetCmd(CmdCode::CMD_SIT_DOWN);
                break;
            case '2': // 切换到站立
                zsibot_exec.SetCmd(CmdCode::CMD_STAND_UP);
                break;
            case '3': // 切换到跳跃
                zsibot_exec.SetCmd(CmdCode::CMD_JUMP);
                break;
            case '4': // 切换到向前跳跃
                zsibot_exec.SetCmd(CmdCode::CMD_FORWARD_JUMP);
                break;
            case '5': // 切换到打招呼
                zsibot_exec.SetCmd(CmdCode::CMD_GREET);
                break;
            case '6': // 切换到后空翻
                zsibot_exec.SetCmd(CmdCode::CMD_BACK_FLIP);
                break;
            case '7': // 切换到实验室模式
                zsibot_exec.SetCmd(CmdCode::CMD_ENTER_LAB_MODE);
                break;
            case '8': // 切换到退出实验室模式
                zsibot_exec.SetCmd(CmdCode::CMD_EXIT_LAB_MODE);
                break;
            case '9': // 切换到双腿站立 再按一次是取消双腿站立
                zsibot_exec.SetCmd(CmdCode::CMD_TWO_LEG_STAND);
                break;
            case 'k': // 进入到sdk模式
                zsibot_exec.SetCmd(CmdCode::CMD_SDK_CONTROL_RIGHT);
                break;
            case 'r': // 进入到remote模式
                zsibot_exec.SetCmd(CmdCode::CMD_REMOTE_CONTROL_RIGHT);
                break;
            case 'b': // 进入平衡站立模式
                zsibot_exec.SetCmd(CmdCode::CMD_BALANCE_STAND_MODE);
                break;
            case 'i': // 显示设备当前状态信息
            {
                std::cout << "\n========== 设备状态信息 ==========" << std::endl;
                std::cout << "电量: " << zsibot_exec.GetPower() << "%" << std::endl;
                std::cout << "温度: " << zsibot_exec.GetTemperature() << "°C" << std::endl;
                std::cout << "速度等级: " << static_cast<int>(zsibot_exec.GetSpeedLevel()) << std::endl;
                std::cout << "功能模式: " << static_cast<int>(zsibot_exec.GetFunctionMode()) << std::endl;
                std::cout << "控制模式: " << static_cast<int>(zsibot_exec.GetControlMode()) << std::endl;
                std::cout << "运动模式: " << static_cast<int>(zsibot_exec.GetMotionMode()) << std::endl;
                std::cout << "运动类型: " << static_cast<int>(zsibot_exec.GetMotionType()) << std::endl;

                SpeedInfo speed = zsibot_exec.GetSpeed();
                std::cout << "速度: " << speed.speed << " m/s" << std::endl;
                std::cout << "角速度: " << speed.angle_speed << " rad/s" << std::endl;
                std::cout << "平移速度: " << speed.shift_speed << " m/s" << std::endl;
                std::cout << "角度: " << speed.angle << " rad" << std::endl;

                std::array<float32_t, 16> motor_temps = zsibot_exec.GetMotorTemp();
                std::cout << "电机温度: ";
                for (size_t i = 0; i < motor_temps.size() && i < 4; ++i)
                {
                    std::cout << "M" << i << ": " << motor_temps[i] << "°C ";
                }
                std::cout << std::endl;

                std::vector<FaultInfo> faults = zsibot_exec.GetFaultInfo();
                if (faults.empty())
                {
                    std::cout << "无故障信息" << std::endl;
                }
                else
                {
                    std::cout << "故障信息:" << std::endl;
                    for (const auto &fault : faults)
                    {
                        std::cout << "  模块: " << fault.module
                                  << ", 子模块: " << fault.submodule
                                  << ", 错误码: " << fault.error_code
                                  << ", 等级: " << fault.level
                                  << ", 信息: " << fault.info << std::endl;
                    }
                }
                std::cout << "================================\n"
                          << std::endl;
                break;
            }

            case 'o': // 设置WiFi信息示例（不会实际执行，因为需要真实的WiFi信息）
                std::cout << "设置WiFi功能演示 (不会实际执行)" << std::endl;
                // WifiInfo wifi_info;
                // wifi_info.ssid = "example_ssid";
                // wifi_info.password = "example_password";
                // zsibot_exec.SetWifi(wifi_info);
                break;

            default:
                // 重置按键状态
                break;
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(20)); // 限制发送频率
    }

    return 0;
}
