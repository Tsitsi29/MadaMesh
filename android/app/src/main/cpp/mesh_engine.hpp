#ifndef MESH_ENGINE_HPP
#define MESH_ENGINE_HPP

#include <string>
#include <vector>
#include <map>
#include <sodium.h>

namespace MadaMesh {

    struct Node {
        std::string id;
        std::string ip;
        int distance; // hops
        unsigned char publicKey[crypto_box_PUBLICKEYBYTES];
    };

    class MeshEngine {
    public:
        MeshEngine();
        ~MeshEngine();

        bool start(int port = 8888);
        void stop();
        
        // Routage et Découverte
        void discoverNodes();
        void updateRoutingTable(const std::string& nodeId, const std::string& ip, int hops, const unsigned char* pubKey);
        
        // Communication
        bool sendMessage(const std::string& targetId, const std::string& message);
        bool broadcast(const std::string& message);

        // Crypto
        void generateKeys();
        std::string encrypt(const std::string& message, const unsigned char* targetPubKey);
        std::string decrypt(const std::string& encryptedMessage, const unsigned char* senderPubKey);

    private:
        std::map<std::string, Node> routingTable;
        bool running;
        int serverSocket;
        int port;
        
        unsigned char publicKey[crypto_box_PUBLICKEYBYTES];
        unsigned char secretKey[crypto_box_SECRETKEYBYTES];

        void listenLoop();
    };

}

#endif // MESH_ENGINE_HPP
