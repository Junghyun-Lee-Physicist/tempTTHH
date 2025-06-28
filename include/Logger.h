// Logger.h
#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <string>
#include <algorithm> // for std::transform

namespace Logger {

    // Helper function to convert string to lowercase
    std::string toLower(const std::string& str) {
        std::string lowerStr = str;
        std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
        return lowerStr;
    }

    // Base function to handle the actual printing with color and log level
    void printMessage(const std::string& message, const std::string& color, const std::string& logLevel) {
        std::string prefix = "[ Log ]: ";   // Default is LOG
        std::string colorCode = "\033[39m"; // Default terminal color
        std::string lowerColor = toLower(color); // Convert color option to lowercase
        std::string lowerLogLevel = toLower(logLevel); // Convert log level option to lowercase

        // Step 1: Set log level (error, warning)
        if (lowerLogLevel == "error" || lowerLogLevel == "e") {
            prefix = "[ Error ]: ";
        } else if (lowerLogLevel == "warning" || lowerLogLevel == "w") {
            prefix = "[ Warning ]: ";
        }

        // Step 2: Set color based on input
        if (lowerColor == "r" || lowerColor == "red") {
            colorCode = "\033[31m"; // Red
        } else if (lowerColor == "g" || lowerColor == "green") {
            colorCode = "\033[32m"; // Green
        } else if (lowerColor == "b" || lowerColor == "blue") {
            colorCode = "\033[34m"; // Blue
        } else if (lowerColor == "c" || lowerColor == "cyan") {
            colorCode = "\033[36m"; // Cyan
        } else if (lowerColor == "y" || lowerColor == "yellow") {
            colorCode = "\033[33m"; // Yellow
        } else if (lowerColor == "m" || lowerColor == "magenta") {
            colorCode = "\033[35m"; // Magenta
        }

        // Print the message with the selected color and prefix
        std::cout << "\t" << colorCode << prefix << message << "\033[0m" << std::endl; // Reset color after printing
    }

    // Main print function that takes message, color, and log level as separate arguments
    void print(const std::string& message, const std::string& color = "", const std::string& logLevel = "") {
        // Directly call printMessage with the provided message, color, and log level
        printMessage(message, color, logLevel);
    }

}

#endif // LOGGER_H

