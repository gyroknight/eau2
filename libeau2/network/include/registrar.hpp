#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <shared_mutex>
#include <vector>

class Registrar {
   private:
    int _sockfd;
    std::vector<struct in_addr> _directory;
    std::shared_mutex _dirMutex;

   public:
    Registrar(const char* port, size_t maxConn);
    ~Registrar();

    void start();
    size_t addNode(in_addr_t address);
};