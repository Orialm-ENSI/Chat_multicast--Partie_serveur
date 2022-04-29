#include "fenetre_serveur.h"
#include <QString>

// Constructeur par défaut
fenetre_serveur::fenetre_serveur() : QWidget()
{


    // Parametres du bouton
    champ_port = new QLineEdit(this);
    {
        champ_port->setPlaceholderText("port serveur");
        champ_port->setInputMask("9999000000");
        champ_port->setToolTip("Entrez un port supérieur ou égal à 1024 pour pouvoir démarrer le serveur dessus");
    }

    bouton_ouverture_serveur = new QPushButton("Ouverture", this);
    {
        bouton_ouverture_serveur->setToolTip("Ouvre le serveur sur le port choisi");
        bouton_ouverture_serveur->setEnabled(true);
        QObject::connect(bouton_ouverture_serveur, &QPushButton::clicked, this, &fenetre_serveur::ouvrir_serveur);
    }

    bouton_fermeture_serveur = new QPushButton("Fermeture", this);
    {
        bouton_fermeture_serveur->setToolTip("Ferme le serveur et déconnecte tout le monde");
        bouton_fermeture_serveur->setEnabled(false); // On ne peut pas cliquer tant que le serveur n'est pas ouvert
        QObject::connect(bouton_fermeture_serveur, &QPushButton::clicked, this, &fenetre_serveur::fermer_serveur);
    }

    // Texte d'information du serveur
    etatServeur = new QLabel(this);
    {
        etatServeur->setText("Le serveur est fermé");
    }

    boutonQuitter = new QPushButton("Quitter", this);
    {
        boutonQuitter->setToolTip("Ferme le serveur et la fenêtre");
        QObject::connect(boutonQuitter, SIGNAL(clicked()), qApp, SLOT(quit())); /* Je laisse cette syntaxe car je ne sais pas de quoi quit est membre*/
    }

    // Disposition
    layout_principal = new QFormLayout(this); //Pour ne pas s'embêter avec les ->setGeometry
    {
        layout_principal->addRow(champ_port);
        layout_principal->addRow(bouton_ouverture_serveur, bouton_fermeture_serveur);
        layout_principal->addRow(etatServeur);
        layout_principal->addRow(boutonQuitter);
    }
    setWindowTitle("Fenetre du serveur");



    // Demarrage du serveur
}

void fenetre_serveur::ouvrir_serveur()
{
    bouton_ouverture_serveur->setEnabled(false);
    quint16 port = champ_port->text().toUInt();
    serveur = new QTcpServer(this);
    if( !serveur->listen(QHostAddress::Any, port)){ // listen renvoi 0 si le port est disponible
        etatServeur->setText("Le serveur n'a pas pu demarrer : <br />" + serveur->errorString());
        bouton_ouverture_serveur->setEnabled(true);
    }
    else{
        bouton_fermeture_serveur->setEnabled(true);
        etatServeur->setText("Le serveur a démarré sur le port <strong>" + QString::number(serveur->serverPort()) + "</strong>.");
        QObject::connect(serveur, &QTcpServer::newConnection, this, &fenetre_serveur::nouvelleConnexion);
    }

}

void fenetre_serveur::fermer_serveur()
{
    envoyerATous("Le serveur va fermer");
    for(int i = 0; i < clients.size(); i++)
    {
        clients[i]->close(); //On parcours les clients et on les déconnectes un à un
    }
    serveur->close(); // Fermeture du serveur
    serveur = nullptr; // Suppression du serveur
    clients.clear(); // On vide la liste des clients
    bouton_fermeture_serveur->setEnabled(false);
    bouton_ouverture_serveur->setEnabled(true);
    etatServeur->setText("Le serveur est fermé");
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
    QDataStream in(socket);
    do{
        if(tailleMessage == 0){ // Si on a pas déjà la taille du message
            if(socket->bytesAvailable() >= (int)sizeof(quint16)){ // Si on au moins un entier, alors on a la taille du message
                in >> tailleMessage;
            }
            else{// Si on a pas au moins un int, alors on a pas encore reçu le message en entier
                return;
            }
        }

        if(socket->bytesAvailable() >= tailleMessage){
            QString date = QDateTime::currentDateTime().toString("dd/MM/yy hh:mm");
            QString type = "TEXT"; //Pour indiquer ce qu'on envoie, le client appelera différente fonction selon la valeur de cet élément, /* Liens voire images */
            QString message;
            in >> message; // On vide entièrement in dans message;
            message = type + "|" + date + "|" + message; // La fonction de réception du client prend | comme séparateur
            envoyerATous(message); // On l'envoi à tout le monde;
            tailleMessage = 0; // On se rend prêt à recevoir un nouveau message;
        }
        else{ // Si on a pas encore le message entier, on attend
            return;
        }
    }while(!in.atEnd());
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
    if( socket != 0){
        clients.removeOne(socket); // On retire le client
        socket->deleteLater();
    }
    else{
        return;
    }
}
