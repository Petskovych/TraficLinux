# TrafficLinux

This program is written in C++ and is designed to monitor network traffic on a computer. Here's how the program works:

Function getNetworkStats(): This function opens the file /proc/net/dev, which contains information about network interfaces in a Linux system. It then reads lines with statistics for each network interface, parses these lines, and stores the statistics in a map with the interface name as the key and the value as a NetStats structure, which contains the number of received and transmitted bytes.

Function isAbnormalActivity(): This function takes two NetStats structures (previous and current network statistics) and checks if there is abnormal activity on the network interface. It compares the number of received and transmitted bytes between the previous and current statistics and returns true if the difference exceeds a certain threshold (in this case, the threshold is set to 10,000 bytes).

Function main(): This is the main function of the program. First, it retrieves the initial network statistics using the getNetworkStats() function and opens a file for logging (traffic_stats.log). Then, it enters an infinite loop where it updates the network statistics every second and prints them to the console and the log file. If abnormal activity is detected on any network interface, this information is also printed to the console and added to the log file.

The program continues to run indefinitely, monitoring changes in network traffic and issuing notifications in case of anomalies.






