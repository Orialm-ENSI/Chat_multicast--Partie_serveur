#include "fenetre_serveur.h"
#include <QString>

// Constructeur par défaut
fenetre_serveur::fenetre_serveur() : QWidget()
{
    // Texte d'information du serveur
    etatServeur = new QLabel(this);

    // Parametres du bouton
    boutonQuitter = new QPushButton("Quitter", this);
    QObject::connect(boutonQuitter, SIGNAL(clicked()), qApp, SLOT(quit()));

    // Disposition
    layout = new QVBoxLayout(this); //Pour ne pas s'embêter avec les ->setGeometry
    layout->addWidget(etatServeur);
    layout->addWidget(boutonQuitter);
    setWindowTitle("Fenetre du serveur");

    // Demarrage du serveur
    /* Songer a mettre ceci hors du constructeur plus tard : J'aimerai pouvoir faire plusieurs serveurs en simultanné */
    serveur = new QTcpServer(this);
    // Attente de la connexion de n'importe qui sur le port 50885.
    if( !serveur->listen(QHostAddress::Any, 50885)){ /* Si possible laisser l'utilisateur choisir son port ou un port automatique*/
        etatServeur->setText("Le serveur n'a pas pu demarrer : <br />" + serveur->errorString());
    }
    else{
        etatServeur->setText("Le serveur a démarré sur le port <strong>" + QString::number(serveur->serverPort()) + "</strong>.<br /> Des clients peuvent maintenant se connecter");
        QObject::connect(serveur, &QTcpServer::newConnection, this, &fenetre_serveur::nouvelleConnexion);
    }
}


void fenetre_serveur::nouvelleConnexion()
{

    // Nouveau client
    QTcpSocket *nouveauClient = serveur->nextPendingConnection();
    clients << nouveauClient;

    // Signaux
    QObject::connect(nouveauClient, &QTcpSocket::readyRead, this, &fenetre_serveur::donneesRecues); // signal readyRead introuvable dans la doc de QTcpSocket
    QObject::connect(nouveauClient, &QTcpSocket::disconnected, this, &fenetre_serveur::deconnexionClient); //idem

    // Annonces
    fenetre_serveur::envoyerATous("Un nouveau client vient de se connecter 1"); // Annonce 1
    fenetre_serveur::envoyerATous("Un nouveau client vient de se connecter 2"); // Annonce 2
    fenetre_serveur::envoyerATous("Un nouveau client vient de se connecter 3"); // Annonce 3

}

void fenetre_serveur::donneesRecues()
{
    // Recherche du QTcpSoccket du client qui envoi
    //fenetre_serveur::envoyerATous("test de réponse\n");

    QTcpSocket *socket = qobject_cast< QTcpSocket *>(sender());
    if(socket == 0){ return; }

    // Recuperation du message
    QDataStream in(socket); /*rendre les if plus propres */
    if(tailleMessage == 0){ // Si on a pas déjà la taille du message

        if(socket->bytesAvailable() < (int)sizeof(quint16)){ // Si on a pas au moins un int, alors on a pas encore reçu le message en entier
            return;
        }
        in >> tailleMessage; // Si on au moins un entier, alors on a la taille du message
    }

    if(socket->bytesAvailable() < tailleMessage) {return;} // Si on a pas encore le message entier, on attend

    QString message;
    in >> message; // On vide entièrement in dans message;
    envoyerATous(message); // On l'envoi à tout le monde;
    tailleMessage = 0; // On se rend prêt à recevoir un nouveau message;
}

void fenetre_serveur::envoyerATous(const QString& message)
{
    QByteArray paquet; // Le paquet a envoyer est un vecteur d'octets
    QDataStream out(&paquet, QIODevice::WriteOnly); // Simplement pour pouvoir ecrire dans le paquet avec <<. C'est du sucre syntaxique
    out << (quint16) 0; // Réservation de l'emplacement pour la taille du message
    out << message; // On place le message a proprement parler
    /* Ajouter un traitement du message, pour mettre le pseudo de qui envoi, l'heure... */
    out.device()->seek(0); // On se repalce au début du paquet
    out << (quint16) (paquet.size() - sizeof(quint16)); // La taille du message compte pas dans la taille du message...

    // Envoi
    for(int i = 0; i < clients.size(); i++)
    {
        clients[i]->write(paquet); //On parcours les clients et on leur envoie un à un
    }
}

void fenetre_serveur::deconnexionClient()
{
    envoyerATous("Un client vient de se déconnecter");

    // Qui ?
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if( socket == 0) { return; }

    clients.removeOne(socket); // On retire le client
    socket->deleteLater();
    /* Mettre des else !!! */

}
