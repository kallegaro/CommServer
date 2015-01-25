#include "inc/tcpsocketserver.h"

tcpSocketServer::tcpSocketServer(QObject *parent) :
    QObject(parent)
{
    connect(&m_server, SIGNAL(newConnection()), this, SLOT(gotNewClientConnection()));

    //inicia a espera por conexões de clientes na porta 8888
    m_server.listen(QHostAddress::Any, 8888);
}

tcpSocketServer::~tcpSocketServer()
{
    foreach (QTcpSocket *client, m_clientsReceivedDataState.keys()) {
        client->disconnect();

        m_clientsReceivedDataState.remove(client);
        delete client;
    }

    m_server.close();
}

void tcpSocketServer::gotNewClientConnection()
{
    //Adiciona a nova conexão na lista de clients.
    QTcpSocket *newClient = m_server.nextPendingConnection();

    ReceivedDataState newClientDataState;
    newClientDataState.pendingBytes = 0;        //Não há mais dados para serem recebidos.http://en.wikipedia.org/wiki/X10_%28industry_standard%29

    //Aloca QByte ARray para armazenar os dados recebidos.
    newClientDataState.receivedData = new QByteArray(255, '\0');

    m_clientsReceivedDataState.insert(newClient, newClientDataState);

    //Conecta o sinal do novo client de novos dados ao método do servidor
    //que trata os dados recebidos.
    connect(newClient, SIGNAL(readyRead()), this, SLOT(gotClientData()));
    connect(newClient, SIGNAL(disconnected()), this, SLOT(gotNewClientDisconnection()));

    //Define o tamanho do buffer de entrada.
    newClient->setReadBufferSize(255);

}

void tcpSocketServer::gotNewClientDisconnection()
{
    QTcpSocket *client = (QTcpSocket *)QObject::sender();

    //Deleta os possíveis pacotes de dados pendentes.
    m_clientsReceivedDataState.value(client).receivedData->clear();
    delete m_clientsReceivedDataState.value(client).receivedData;

    //Limpa a memória do HEAD alocada para o objeto do client.
    //TODO: Checar se é necessário deletar este objeto -> Não foi alocado por mim.
    //delete client;
}

void tcpSocketServer::gotClientData()
{
    char buffer[255];
    QTcpSocket *client = (QTcpSocket *)QObject::sender();

    //Checa se a lista de clientes contém o seu socket.
    if(m_clientsReceivedDataState.keys().contains(client)) {
        //Se pendingBytes = 0, este é um novo pacote de dados, e não outro pedaço
        //de um pacote anterior-

        qint16 readDataSize = client->read(buffer, 255);

        //Se ocorreou algum erro na transmissão (Checar help para casos de erro).
        if(readDataSize == -1) {
            return;
        }

        //SE existem dados para serem lidos do Socket.
        if(readDataSize != 0) {
            if(m_clientsReceivedDataState.value(client).pendingBytes == 0) {
                //Desconta o primeiro byte.
                readDataSize--;


                //Como o primeiro byte contém o número de bytes do pacote, atualiza
                //a variável pending Bytes com o valor informado do tamanho do pacote
                // menos o tamanho da mensagem atualmente recebida.
                m_clientsReceivedDataState[client].pendingBytes = (buffer[0] - (readDataSize));

                //Adiciona os bytes lidos no buffer de dados recebidos do client.
                m_clientsReceivedDataState[client].receivedData->append(buffer+1, readDataSize);
            }else {
                //Atualiza  a variáve que decrementa o número de bytes que ainda
                //restar para ser lidos.
                m_clientsReceivedDataState[client].pendingBytes = (buffer[0] - (readDataSize));

                //Se entra nesse else, a contagem de bytes é diferente de zeros, e assim,
                // ainda há dados para serem lidos do socket. Le os dados e os
                //adiciona ao socket
                m_clientsReceivedDataState[client].receivedData->append(buffer, readDataSize);
            }

            //Após a leitura dos dados do buffer, checa se terminou de receber
            //o número de dados informado pelo client.
            if(m_clientsReceivedDataState[client].pendingBytes == 0) {
                emit receivedDataPack(m_clientsReceivedDataState[client].receivedData);

                //Limpa o byte array  que acumula os bytes recebidos.
                m_clientsReceivedDataState[client].receivedData->clear();
            }
        }
    }
}
