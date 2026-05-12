#include "mesh_engine.hpp"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <thread>
#include <vector>

namespace MadaMesh {

    MeshEngine::MeshEngine() : running(false), serverSocket(-1) {
        if (sodium_init() < 0) {
            std::cerr << "[MadaMesh] Erreur fatale : libsodium n'a pas pu être initialisée." << std::endl;
        }
        generateKeys();
    }

    MeshEngine::~MeshEngine() {
        stop();
    }

    void MeshEngine::generateKeys() {
        crypto_box_keypair(publicKey, secretKey);
        std::cout << "[MadaMesh] Clés cryptographiques générées." << std::endl;
    }

    bool MeshEngine::start(int p) {
        if (running) return true;
        port = p;

        serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
        if (serverSocket < 0) {
            std::cerr << "[MadaMesh] Erreur socket." << std::endl;
            return false;
        }

        struct sockaddr_in serverAddr;
        memset(&serverAddr, 0, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_port = htons(port);

        if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
            std::cerr << "[MadaMesh] Erreur bind sur le port " << port << std::endl;
            close(serverSocket);
            return false;
        }

        running = true;
        std::thread(&MeshEngine::listenLoop, this).detach();
        std::cout << "[MadaMesh] Moteur écoute sur le port " << port << std::endl;
        return true;
    }

    void MeshEngine::stop() {
        if (!running) return;
        running = false;
        if (serverSocket != -1) {
            close(serverSocket);
            serverSocket = -1;
        }
        std::cout << "[MadaMesh] Moteur arrêté." << std::endl;
    }

    void MeshEngine::listenLoop() {
        char buffer[4096];
        struct sockaddr_in clientAddr;
        socklen_t addrLen = sizeof(clientAddr);

        while (running) {
            int len = recvfrom(serverSocket, buffer, sizeof(buffer), 0, (struct sockaddr*)&clientAddr, &addrLen);
            if (len > 0) {
                // Pour l'instant, on se contente de logger la réception
                std::cout << "[MadaMesh] Reçu de " << inet_ntoa(clientAddr.sin_addr) << " : " << len << " octets." << std::endl;
            }
        }
    }

    void MeshEngine::discoverNodes() {
        std::cout << "[MadaMesh] Phase de découverte active (UDP/P2P)..." << std::endl;
        // Dans une version réelle, on envoie un paquet UDP Broadcast "HELLO" avec notre clé publique.
    }

    void MeshEngine::updateRoutingTable(const std::string& nodeId, const std::string& ip, int hops, const unsigned char* pubKey) {
        Node newNode;
        newNode.id = nodeId;
        newNode.ip = ip;
        newNode.distance = hops;
        memcpy(newNode.publicKey, pubKey, crypto_box_PUBLICKEYBYTES);
        routingTable[nodeId] = newNode;
        std::cout << "[MadaMesh] Table mise à jour : " << nodeId << " (" << ip << ")" << std::endl;
    }

    std::string MeshEngine::encrypt(const std::string& message, const unsigned char* targetPubKey) {
        unsigned char nonce[crypto_box_NONCEBYTES];
        randombytes_buf(nonce, sizeof nonce);

        size_t ciphertext_len = crypto_box_MACBYTES + message.length();
        std::vector<unsigned char> ciphertext(ciphertext_len);

        if (crypto_box_easy(ciphertext.data(), (const unsigned char*)message.c_str(), message.length(), nonce, targetPubKey, secretKey) != 0) {
            return "";
        }

        std::string result((char*)nonce, crypto_box_NONCEBYTES);
        result.append((char*)ciphertext.data(), ciphertext_len);
        return result;
    }

    bool MeshEngine::sendMessage(const std::string& targetId, const std::string& message) {
        if (routingTable.find(targetId) == routingTable.end()) return false;

        std::string encrypted = encrypt(message, routingTable[targetId].publicKey);
        if (encrypted.empty()) return false;

        struct sockaddr_in targetAddr;
        memset(&targetAddr, 0, sizeof(targetAddr));
        targetAddr.sin_family = AF_INET;
        targetAddr.sin_port = htons(port);
        inet_pton(AF_INET, routingTable[targetId].ip.c_str(), &targetAddr.sin_addr);

        sendto(serverSocket, encrypted.c_str(), encrypted.length(), 0, (struct sockaddr*)&targetAddr, sizeof(targetAddr));
        std::cout << "[MadaMesh] Message chiffré envoyé à " << targetId << " (" << routingTable[targetId].ip << ")" << std::endl;
        return true;
    }

    bool MeshEngine::broadcast(const std::string& message) {
        for (auto const& [id, node] : routingTable) {
            sendMessage(id, message);
        }
        return true;
    }
}
