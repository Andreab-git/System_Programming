#include <iostream>
#include "../Lab2/Base.h"
#include <memory>

using namespace std;

shared_ptr<Directory>  Directory::root = nullptr;

int main() {

  shared_ptr<Directory> root = Directory::getRoot();
  auto alfa = root->addDirectory("alfa");
  root->addDirectory("beta")->addDirectory("beta1");
  root->getDir("beta")->addDirectory("beta2");
  alfa->getDir("..")->ls(4);

  cout<<"----"<<endl;

  root->remove("beta");
  root->ls(4);
}
