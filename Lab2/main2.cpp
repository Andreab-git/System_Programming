#include <iostream>
#include "../Lab2/Base.h"
#include <memory>

using namespace std;

shared_ptr<Directory> Directory::root = nullptr;

int main() {
  // Chiamando getRoot() si ottiene uno shared_ptr di tipo Directory
  // Inizialmente root e' uguale a nullptr. Se e' uguale a nullptr, allora
  // si imposta la directory di partenza assegnandogli il nome '/' ed il padre
  // sara' uguale a nullptr.
  shared_ptr<Directory> root = Directory::getRoot();
  auto alfa = root->addDirectory("alfa");
  root->addDirectory("beta")->addDirectory("beta1");
  root->getDir("beta")->addDirectory("beta2");
  alfa->getDir("..")->ls(4);

  cout<<"----"<<endl;

  root->remove("beta");
  root->ls(4);
}
