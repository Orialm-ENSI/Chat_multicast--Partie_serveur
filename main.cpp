// fichiers par défaut
#include <QtWidgets/QApplication>

// fichiers perso
#include "fenetre_serveur.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    fenetre_serveur fenetre;
    fenetre.show();

    return app.exec();
}
