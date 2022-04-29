#ifndef FENETRE_SERVEUR_H
#define FENETRE_SERVEUR_H

// fichiers par défaut
#include <QWidget>
#include <QtNetwork>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>

class fenetre_serveur : public QWidget {
    Q_OBJECT

public:
    fenetre_serveur();
    void envoyerATous(const QString& message); /*renommer en envoyer*/

private slots:
    void nouvelleConnexion(); /*renommer en client_connexion*/
    void donneesRecues(); /*renommer en recevoir*/
    void deconnexionClient(); /*renommer en client_deconnexion*/

private:
    // Elements de l'interface
    QLabel *etatServeur = nullptr;
    QPushButton *boutonQuitter = nullptr;
    QVBoxLayout *layout = nullptr;

    // Elements de réseau
    QTcpServer *serveur = nullptr;
    QList<QTcpSocket *> clients;
    quint16 tailleMessage = 0;


};

#endif // FENETRE_SERVEUR_H
