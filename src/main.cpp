#include <cerrno>
#include <cmath>
#include <cstdlib>
#include <errno.h>
#include <stdexcept>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <print>

constexpr char* SV_SOCK_PATH = "/tmp/mini.sock";
constexpr uint BUF_SIZE = 1024;
constexpr uint CONNECTIONS = 5;

void run_server() {
    struct sockaddr_un addr;
    int sfd = socket(AF_UNIX, SOCK_STREAM, 0);
    std::println("Server socket fd = {}\n", sfd);

    if (sfd == -1) {
        throw std::runtime_error("Socket error");
    }

    if (strlen(SV_SOCK_PATH) > sizeof(addr.sun_path) - 1) {
        throw std::runtime_error(
            std::format("Server socket path is too long: {}", SV_SOCK_PATH)
        );
    }

    if (remove(SV_SOCK_PATH) == -1 && errno != ENOENT) {
        throw std::runtime_error(
            std::format("Remove: {}", SV_SOCK_PATH)
        );
    }

    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SV_SOCK_PATH, sizeof(addr.sun_path) - 1);

    if (bind(sfd, (struct sockaddr *) &addr, sizeof(struct sockaddr_un)) == -1) {
        throw std::runtime_error("Binding error");
    }

    if (listen(sfd, CONNECTIONS) == -1) {
        throw std::runtime_error("Listening error");
    }

    ssize_t num_read;
    char buf[BUF_SIZE];

    for(;;) {
        std::println("Waiting to accept a connection...");
        int cfd = accept(sfd, nullptr, nullptr);
        std::println("Accepted socket fd = {}", cfd);

        while((num_read = read(cfd, buf, BUF_SIZE)) > 0) {
            if (write(STDOUT_FILENO, buf, num_read) != num_read) {
                throw std::runtime_error("Failed write");
            }
        }

        if (num_read == -1) {
            throw std::runtime_error("Read");
        }

        if (close(cfd) == -1) {
            throw std::runtime_error("Close");
        }
    }
}

int run_client() {
    struct sockaddr_un addr;
    ssize_t num_read;
    char buf[BUF_SIZE];
    
    int sfd = socket(AF_UNIX, SOCK_STREAM, 0);
    std::println("Client socket fd = {}", sfd);    

    if (sfd == -1) {
        throw std::runtime_error("Socket error");
    }

    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SV_SOCK_PATH, sizeof(addr.sun_path) - 1);

    if (connect(sfd, (struct sockaddr*) &addr, sizeof(struct sockaddr_un)) == -1) {
        throw std::runtime_error("Connect");
    }

    while ((num_read = read(STDIN_FILENO, buf, BUF_SIZE)) > 0) {
        if (write(sfd, buf, num_read) != num_read) {
            throw std::runtime_error("Write error");
        }
    }

    if (num_read == -1) {
        throw std::runtime_error("Read");
    }

    exit(EXIT_SUCCESS);
}

int main(int argc, char **argv) {
    pid_t pid = fork();

    if(pid == 0) {
        run_server();
        _exit(0);
    } else {
        run_client();
        waitpid(pid, nullptr, 0);
    }
}