# Chat_multicast--Partie_serveur
Projet de chat multicast demandé pour le cours d'OS et réseau // Côté serveur

--------------------------------------

# Serveur-1.0.4 : corrections de propreté
1) remplacement du goto/label par une boucle do/while dans le slot "donneesRecues"
2) dans les slot "donneesRecues" et "deconnexionClient", réécriture des if : Les conditions sont inversées dans les if, ce qui était fait précédément si on entrait pas dans le if est désormais fait si on entre dans les if, et les return vont dans les else, comme cas par défaut. Les if n'ont plus de pate en l'air.


# Serveur-1.0.3 : Correction du retard
1) La fonction de réception des données du client s'arrêtais une fois qu'elle a lu un message dont la taille est indiquée par le début d'un message, ce qui entraînait un retard d'autant plus important que le serveur envoie de messages. ajout d'un label et d'un goto pour rester dans la fonction tant qu'on est pas arrivé à la fin du flux du socket. La même modification a été apportée au serveur.

# Serveur-1.0.2 : Retour en arrière
1) La combinaison client / serveur présentait un retard de plusieurs messages. L'ajout des if else a rendu ce retard encore plus important. Je retourne au code précédent qui avait le retard le plus faible (égal au nombre de messages systèmes que le serveur envoie). Le header n'est pas ramené en arrière.

# Serveur-1.0.1 : corrections de propreté
1) certains pointeurs était initialisés sur rien et le programme client crashait parfois à l'éxécution. des forum anglophones ont proposé de modifier le header de la class pour mettre tous les pointeurs sur nullptr avant même l'appel du constructeur.
2) le slot "donneesRecues" utilisait une succession de if avec des return, en supposant que ce qui venait après s'éxécutait forcément s'il n'y a pas eu de return. Ajout de else pour ne pas laisser les if avec des pates en l'air.
3) même problématique corrigée sur le slot "deconnexionClient".

# Serveur-1.0.0 : basée sur "Solution chat multicast TCP.pdf"
Simple copier/coller de ce fichier trouvé sur internet pour avoir une base qui fonctionne, puis l'adapter à ce qui est demandé : Le serveur fonctionne en TCP et non pas UDP.

--------------------------------------

# Sources des fichiers

Cours Qt                    : Fourni par M. Boudier
Tuto git Qt Creator         : https://cpb-us-w2.wpmucdn.com/u.osu.edu/dist/7/11881/files/2018/02/QtCreatorGitTutorial-2ahr2m0.pdf #Recherche web, pas d'auteur
Solution chat multicast TCP : http://projet.eu.org/pedago/sin/term/5-Qt_reseau.pdf #Recherche web, pas d'auteur
