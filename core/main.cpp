#include "mesh_engine.hpp"
#include <iostream>
#include <unistd.h>
#include <cstring>

int main() {
    MadaMesh::MeshEngine engine;

    // Simulation d'une clé publique distante (pour le test)
    unsigned char dummyPubKey[crypto_box_PUBLICKEYBYTES];
    memset(dummyPubKey, 0x42, crypto_box_PUBLICKEYBYTES);

    if (engine.start(8888)) {
        std::cout << "--- Test MadaMesh Security & Sockets ---" << std::endl;
        
        // Simuler la découverte d'un noeud sur localhost pour le test
        engine.updateRoutingTable("LocalNode", "127.0.0.1", 1, dummyPubKey);

        // Tester l'envoi chiffré vers soi-même (localhost)
        engine.sendMessage("LocalNode", "Secret Malagasy Message");

        // Laisser un peu de temps pour que le thread de réception capture le message
        usleep(100000); 

        engine.stop();
    }

    return 0;
}
