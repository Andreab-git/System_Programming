//
// Created by ff on 4/23/20.
//
//#include <>

#ifndef LEZIONE_2_BASE_H
#define LEZIONE_2_BASE_H

#include <string>
#include <memory>
#include <list>
#include <experimental/filesystem>
#include <iostream>


using namespace std;

enum MType {
    TDir=0, TFile=1
};

class Base {
protected:
    string name;
public:
    string getName() const{
        return name;
    };
    virtual int mType() const = 0;
    virtual void ls(uint indent=0) const  = 0;
};

class File: public Base {
    uintmax_t  size;
public:
    File(string name, uintmax_t size): size(size){
        this->name = name;
    }

    int mType() const override {
        return TFile;
    }

    void ls(uint indent) const override {
        for(int i=0; i<indent; i++){
            cout<<" ";
        }
        cout<<name<<" "<<size<<endl;
    };


};

class Directory: public Base {

    static shared_ptr<Directory> root;
    list<shared_ptr<Base>> children;

    weak_ptr<Directory> self;
    weak_ptr<Directory> parent;

    Directory(string name) {
        this->name = name;
    }

    static shared_ptr<Directory> make(string name, weak_ptr<Directory> parent){
        shared_ptr<Directory> d = shared_ptr<Directory>(new Directory(name));
        d->self = d;
        d->parent = parent.lock() != nullptr ? parent: d;
        return d;
    }

public:
    static void ciao() {cout<<"aaa";}

    /** <h3>Descrizione</h3>
     *  Restituisce la radice.<br>
     *  Se la radice e' un nullptr, allora invoca il metodo <i>make</i> che crea la directory
     *  assegnando il nome '/' ed un weak_ptr a <i>nullptr</i>
     * @return shared_ptr di tipo Directory
     */
    static shared_ptr<Directory> getRoot(){
        if(root == nullptr) {
            root = make("/", shared_ptr<Directory>(nullptr));
        }
        return root;
    }

    std::shared_ptr<Directory> addDirectory(string name) {
        shared_ptr<Directory> d = make(name, self);
        children.push_back(d);
        return d;
    }

    shared_ptr<File> addFile(string name, uintmax_t size) {
        shared_ptr<File> f = make_shared<File>(name, size);
        children.push_back(f);
        return f;
    }

    shared_ptr<Base> get(std::string name) {
        if(name == "."){
            return this->self.lock();
        }

        if(name == ".."){
            return this->parent.lock();
        }

        for(auto c: children){
            if(c->getName() == name){
                return c;
            }
        }
        return shared_ptr<Base>(nullptr);
    }

    shared_ptr<Directory> getDir(string name) {
        shared_ptr<Base> dir = this->get(name);
        if(dir == nullptr || dir->mType() != TDir){
            return shared_ptr<Directory>(nullptr);
        } else {
            return dynamic_pointer_cast<Directory>(dir);
        }
    };

    shared_ptr<File> getFile(string name) {
        shared_ptr<Base> file = this->get(name);
        if(file == nullptr || file->mType() != TFile){
            return shared_ptr<File>(nullptr);
        } else {
            return dynamic_pointer_cast<File>(file);
        }
    };

    void remove(string name) {
        shared_ptr<Base> target = this->get(name);
        if(target != nullptr) {
            children.remove(target);
        }
    }

    int mType() const override {
       return TDir;
    }

    void ls(uint indent=0) const  override {
        for(int i=0; i<indent; i++)
            std::cout<<" ";
        std::cout<<"+ "<<name<<endl;
        for(auto child: children){
            child->ls(indent+4);
        }
    }

};


#endif //LEZIONE_2_BASE_H
