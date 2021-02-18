#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <thread>

class TheSocketTCPIP4{
public:
    //Creating Socket
    int portnumber;
    int listening;
    int result;

    //binding to ip.port
    sockaddr_in hint;

    TheSocketTCPIP4(int port){
        //Create socket
        this->portnumber = port;
        this->create(port);
        this->theBind();
        this->theListenMultiple();
        //this->theListen();
        //this->Accept();
    }
    void create(int port){
        this->listening = socket(AF_INET, SOCK_STREAM, 0);

        if (this->listening <= 0 ){
            std::cerr <<"Can't create a socket " << std:: endl;
        }
        this->hint.sin_family = AF_INET;
        this->hint.sin_port = htons(this->portnumber);

        inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);
    }
    void theBind(){
        if (bind(this->listening, (sockaddr*)&this->hint, sizeof(this->hint)) == -1){
            std::cerr << "Can't Create A Socket " << std::endl; // make a reutrn error
        }
    }
    void theListen(){
        if (listen(this->listening, SOMAXCONN) == - 1){
            std::cerr << "Cannot listen on socket" << std::endl;
        }
    }
    void theListenMultiple(){
        if (listen(this->listening, SOMAXCONN) == - 1){
            std::cerr << "Cannot listen on socket" << std::endl;
        }
        sockaddr_in theClient;
        socklen_t theClientSize = sizeof (theClient);
        char host[NI_MAXHOST];
        char svc [NI_MAXSERV];
        memset(host, 0, NI_MAXHOST);
        memset(svc, 0, NI_MAXSERV);
        int newclientSocket;

        while (true){
            newclientSocket = accept(this->listening, (sockaddr*)&theClient, &theClientSize);
            if (newclientSocket == -1){
                std::cerr << "Problem with client Connecting " << std::endl;
            }

            std::thread thread1(theProcess, newclientSocket);
            thread1.detach();
        }
    }

    static void theProcess(int socket){
        while (true){
            char buffer[4096];
            memset(buffer, 0, 4096);
            int bytesRecevied = recv(socket, buffer, 4096, 0);
            send(socket, buffer, bytesRecevied+1, 0);
        }
    }

    void Accept(){

        sockaddr_in client;
        socklen_t  clientSize = sizeof (client);
        char host[NI_MAXHOST];
        char svc [NI_MAXSERV];

        memset(host, 0, NI_MAXHOST);
        memset(svc, 0, NI_MAXSERV);

        int clientSocket = accept(this->listening, (sockaddr*)&client, &clientSize);
        if (clientSocket == - 1){
            std::cerr << "Problems with client connecting " << std::endl;
        }
        int result = getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, svc, NI_MAXSERV, 0);
        if (result){
            std::cout << host << " Connected on " << svc << std::endl;
        }
        else {
            inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
            std::cout << host << " Connected on " << ntohs(client.sin_port) << std::endl;
        }
        char buf[4096];
        while (true){
            //Clear Buffer
            memset(buf, 0, 4096);

            //Wait for a message
            int bytesRec = recv(clientSocket, buf, 4096, 0);
            if (bytesRec == -1){
                std::cerr<<"There was a connection issue" << std::endl;
                break;
            }
            if (bytesRec == 0){
                std::cout << "The Client disconncted " << std::endl;
                break;
            }
            //Display Message
            std::cout << "Recevied String: " << std::string(buf, 0, bytesRec) << std::endl;
            //Resend Message
            send(clientSocket, buf, bytesRec+1, 0);
        }
    }
    void closeSocket(){
        close(listening);
    }

};
int main1(){
    int portNumber = 3000;

    //Creation of the socket
    int listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening <= 0){
        std::cerr << "Can't Create a socket" << std::endl;
        return -1;
    }
    //Binding to IP/port
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(portNumber);
    inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);

    if (bind(listening, (sockaddr*)&hint, sizeof(hint)) == -1){
        std::cerr << "Cant bind to iP/port" << std::endl;
        return -2;
    }
    //Make socket listen
    if (listen(listening, SOMAXCONN)== -1){
        std::cerr <<"Can't listen" << std::endl;
        return -3;
    }
    //Accept the call
    sockaddr_in client;
    socklen_t  clientSize = sizeof (client);
    char host[NI_MAXHOST];
    char svc [NI_MAXSERV];

    int clientSocket = accept(listening, (sockaddr*)&client, &clientSize);

    if (clientSocket == -1){
        std::cerr << "Problem with client connecting" << std::endl;
        return -4;
    }
    //Close the listening socket;
    close(listening);
    memset(host, 0, NI_MAXHOST);
    memset(svc, 0, NI_MAXSERV);

    int result = getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, svc, NI_MAXSERV, 0);
    if (result){
        std::cout << host << " Connected on " << svc << std::endl;
    }
    else {
        inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
        std::cout << host << " Connected on " << ntohs(client.sin_port) << std::endl;
    }
    //Display message that is reveieved and echo message
    char buf[4096];
    while (true){
        //Clear Buffer
        memset(buf, 0, 4096);

        //Wait for a message
        int bytesRec = recv(clientSocket, buf, 4096, 0);
        if (bytesRec == -1){
            std::cerr<<"There was a connection issue" << std::endl;
            break;
        }
        if (bytesRec == 0){
            std::cout << "The Client disconncted " << std::endl;
            break;
        }
        //Display Message
        std::cout << "Recevied String: " << std::string(buf, 0, bytesRec) << std::endl;
        //Resend Message
        send(clientSocket, buf, bytesRec+1, 0);
    }
    close(clientSocket);
}
template<typename T>
void println(T message){
    std::cout << message << std::endl;

}
int main(){
    //println(73);
    //println("God's name be blessed. Lord Jesus Christ is Lord");
    TheSocketTCPIP4 theTcp(3000);
}