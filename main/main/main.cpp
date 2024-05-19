#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <unistd.h>
#include <map>

struct NetStats {
    unsigned long long rxBytes;
    unsigned long long txBytes;
};

std::map<std::string, NetStats> getNetworkStats() {
    std::ifstream file("/proc/net/dev");
    std::string line;
    std::map<std::string, NetStats> stats;

    // Пропускаємо перші два рядки (заголовки)
    std::getline(file, line);
    std::getline(file, line);

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string interfaceName;
        NetStats interfaceStats = { 0, 0 };
        unsigned long long rxPackets, rxErrors, rxDropped, rxFifo, rxFrame, rxCompressed, rxMulticast;
        unsigned long long txPackets, txErrors, txDropped, txFifo, txColls, txCarrier, txCompressed;

        iss >> interfaceName;
        interfaceName = interfaceName.substr(0, interfaceName.find(':'));
        iss >> interfaceStats.rxBytes >> rxPackets >> rxErrors >> rxDropped >> rxFifo >> rxFrame >> rxCompressed >> rxMulticast;
        iss >> interfaceStats.txBytes >> txPackets >> txErrors >> txDropped >> txFifo >> txColls >> txCarrier >> txCompressed;

        stats[interfaceName] = interfaceStats;
    }

    return stats;
}

bool isAbnormalActivity(const NetStats& prevStats, const NetStats& currentStats) {
    unsigned long long deltaRxBytes = currentStats.rxBytes - prevStats.rxBytes;
    unsigned long long deltaTxBytes = currentStats.txBytes - prevStats.txBytes;

    
    const unsigned long long threshold = 10000;

    return (deltaRxBytes > threshold || deltaTxBytes > threshold);
}

int main() {
    std::map<std::string, NetStats> prevStats = getNetworkStats();

    std::ofstream logFile("traffic_stats.log", std::ios_base::app);
    if (!logFile.is_open()) {
        std::cerr << "Error opening log file" << std::endl;
        return 1;
    }

    // Періодичний збір та відображення статистики мережі
    while (true) {
        sleep(1);  // Очікування 1 секунду
        auto currentStats = getNetworkStats();

        std::cout << "Current network statistics:" << std::endl;
        logFile << "Current network statistics:" << std::endl;

        for (const auto& entry : currentStats) {
            auto it = prevStats.find(entry.first);
            if (it != prevStats.end()) {
                unsigned long long deltaRxBytes = entry.second.rxBytes - it->second.rxBytes;
                unsigned long long deltaTxBytes = entry.second.txBytes - it->second.txBytes;

                std::cout << entry.first << ": RX bytes=" << entry.second.rxBytes << " (Delta=" << deltaRxBytes << "), "
                    << "TX bytes=" << entry.second.txBytes << " (Delta=" << deltaTxBytes << ")" << std::endl;

                logFile << entry.first << ": RX bytes=" << entry.second.rxBytes << " (Delta=" << deltaRxBytes << "), "
                    << "TX bytes=" << entry.second.txBytes << " (Delta=" << deltaTxBytes << ")" << std::endl;

                // Виявлення аномальної активності
                if (isAbnormalActivity(it->second, entry.second)) {
                    std::cout << "Attention: Abnormal activity on interface " << entry.first << std::endl;
                    logFile << "Attention: Abnormal activity on interface " << entry.first << std::endl;
                }
            }
        }

        std::cout << std::endl;
        logFile << std::endl;
        logFile.flush(); // Зберігаємо дані в файл після кожної ітерації

        prevStats = currentStats; // Оновлення попередніх статистикдддддддддддддддд
    }

    logFile.close();
    return 0;
}
