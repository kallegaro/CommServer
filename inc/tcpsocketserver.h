#ifndef TCPSOCKETSERVER_H
#define TCPSOCKETSERVER_H

#include <QHostAddress>
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

typedef struct {
    quint8 pendingBytes;
    QByteArray *receivedData;
} ReceivedDataState;

class tcpSocketServer : public QObject
{
    Q_OBJECT
public:
    explicit tcpSocketServer(QObject *parent = 0);
    ~tcpSocketServer();

signals:
    /**
     * @brief receivedDataPack
     * Sinal emitido quando o servidor termina de receber o pacote de dados
     * completo do client. O ponteiro é emitido como const para impedir que os
     * receptores do sinal alterem o conteúdo do buffer de recebimento no HEAP.
     * @param newDataPack
     */
    void receivedDataPack(const QByteArray *newDataPack);

protected slots:
    /**
     * @brief gotNewConnection
     * Método executado sempre que um client solicita a conexão com o servidor.
     * Este método aloca os recursos necessário para tratar os dados recebidos
     * de cada um dos clientes.
     */
    void gotNewClientConnection(void);

    /**
     * @brief gotNewClientDisconnection
     * Método executado smepre que um client se desconecta do servidor.
     */
    void gotNewClientDisconnection(void);

    /**
     * @brief gotClientData
     * Método executado smepre que um dos clientes envia dados para o servidor.
     *
     */
    void gotClientData(void);

private:
    QTcpServer m_server;
    QMap<QTcpSocket *, ReceivedDataState> m_clientsReceivedDataState;
};

#endif // TCPSOCKETSERVER_H
