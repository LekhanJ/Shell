#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>

std::vector<std::string> parseInput(const std::string& input) {
    std::istringstream iss(input);
    std::vector<std::string> tokens;
    std::string token;
    while (iss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

void handleBuiltInCommands(const std::vector<std::string>& args) {
    if (args[0] == "cd") {
        if (args.size() < 2) {
            std::cerr << "cd: Missing argument" << std::endl;
        } else if (chdir(args[1].c_str()) != 0) {
            perror("cd failed");
        }
    }
}

void executeCommand(std::vector<std::string> args) {
    bool isBackground = false;
    if (!args.empty() && args.back() == "&") {
        isBackground = true;
        args.pop_back();
    }

    pid_t pid = fork();
    if (pid == 0) {
        std::vector<char*> c_args;
        for (const auto& arg : args) {
            c_args.push_back(const_cast<char*>(arg.c_str()));
        }
        c_args.push_back(nullptr);
        if (execvp(c_args[0], c_args.data()) == -1) {
            perror("Error executing command");
        }
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        if (!isBackground) {
            int status;
            waitpid(pid, &status, 0);
        } else {
            std::cout << "Process running in background, PID: " << pid << std::endl;
        }
    } else {
        perror("Fork failed");
    }
}

int main() {
    std::string input;

    while (true) {
        std::cout << "shell> ";
        std::getline(std::cin, input);

        auto args = parseInput(input);
        if (args.empty()) continue;

        if (args[0] == "exit") break;
        else if (args[0] == "cd") handleBuiltInCommands(args);
        else executeCommand(args);
    }

    return 0;
}
