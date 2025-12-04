#ifndef PAWNOTECLIENT_H
#define PAWNOTECLIENT_H

class PawnoteClient {
public:
    PawnoteClient(); // Constructor
    ~PawnoteClient(); // Destructor

    void connect(); // Method to connect to Pawnote service
    void disconnect(); // Method to disconnect from Pawnote service
    void sendNote(const std::string& note); // Method to send a note
    std::string receiveNote(); // Method to receive a note
};

#endif // PAWNOTECLIENT_H