#include <iostream>
#include "../Lab2/Base.h"
#include <memory>

using namespace std;

shared_ptr<Directory> Directory::root = nullptr;

int main() {
    // Chiamando getRoot() si ottiene uno shared_ptr di tipo Directory
    // Inizialmente root e' uguale a nullptr. Se e' uguale a nullptr, allora
    // si imposta la directory di partenza assegnandogli il nome '/' ed il puntatore
    // al padre della directory puntera' a nullptr.
    shared_ptr<Directory> root = Directory::getRoot();
    // Aggiunge la directory ma se esiste, restituisce il puntatore alla directory
    // trovata.
    LOG1("Aggiunta dir alfa");
    auto alfa = root->addDirectory("alfa");
    LOG1("Aggiunta dir beta/beta1");
    root->addDirectory("beta")->addDirectory("beta1");
    LOG1("Aggiunta dir beta/beta2");
    root->getDirectory("beta")->addDirectory("beta2");
    LOG1("Stampa albero a partire da root");
    alfa->getDirectory("..")->ls(4);

    cout << "----" << endl;

    root->remove("beta");
    root->ls(4);

    cout << "----" << endl;

    root->addFile("Banana", 100);
    root->ls(5);
}
