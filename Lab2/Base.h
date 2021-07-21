#ifndef BASE_H
#define BASE_H

#include <string>
#include <memory>
#include <list>
#include <experimental/filesystem>
#include <iostream>

#define LOG1(x) std::cout << x << std::endl
#define LOG2(x, y) std::cout << x << " " << y << std::endl
#define LOG3(x, y, z) std::cout << x << " " << y << " " << z << std::endl
#define LOG4(x, y, z, w) std::cout << x << " " << y << " " << z << " " << w << std::endl

static bool debug = true;

using namespace std;

enum MediaType {
    is_a_Dir = 0, is_a_File = 1
};

// ###########################################
// ##############     BASE    ################
// ###########################################

class Base {
protected:
    string name;
public:
    string getName() const {
        return name;
    };

    // Un metodo virtual rende la classe Base astratta
    // si mette const = 0; quando il metodo virtual e' puro
    // non si suppone che in questa classe verra' implementato
    virtual int mediaType() const = 0;

    // Un metodo virtual rende la classe Base astratta
    virtual void ls(uint indent = 0) const = 0;
};





// ################################################
// #############        FILE       ################
// ################################################

class File : public Base {
    uintmax_t size;
public:
    File(string name, uintmax_t size) : size(size) {
        this->name = name;
    }

    /** <h3>Descrizione</h3>
     *  Restituisce il tipo di istanza (File) codificato come intero.<br>
     *
     *  @return is_a_file: intero rappresentante il tipo di istanza, in questo caso vale 1
     */
    int mediaType() const override {
        return is_a_File;
    }

    /** <h3>Descrizione</h3>
     *  Permette di formattare lo standard output riferito ad un file.<br>
     *  L'output che si ottiene non e' altro che la stampa a video di <i>nome e dimensione</i> del File.
     *
     *  @param indent: rappresenta il numero di spazi che verranno inseriti prima di stampare nome e dimensione.
     */
    void ls(uint indent) const override {
        for (int i = 0; i < indent; i++) {
            cout << " ";
        }
        cout << name << " " << size << endl;
    };


};




// ################################################
// ##############     DIRECTORY    ################
// ################################################

class Directory : public Base {

    static shared_ptr<Directory> root;
    list <shared_ptr<Base>> children;

    weak_ptr<Directory> self;
    weak_ptr<Directory> parent;

    Directory(string name) {
        this->name = name;
    }

    /** <h3>Descrizione</h3>
     *
     *  Chiama il costruttore di <i>Directory</i> per generare un nuovo shared_ptr
     *  ad una directory di nome <i>name</i>.<br>
     *  Salva il valore del puntatore in una variabile interna alla classe.
     *  Se il parametro <i>parent</i>
     *  e' diverso da null, allora assegno il valore del puntatore (che rappresenta il padre)
     *  alla variabile <i>parent<i/>.<br>
     *
     * @param name: Il nome della directory che si vuole creare
     * @param parent: Il puntatore al padre della directory che si vuole creare
     * @return shared_ptr di tipo Directory
     */
    static shared_ptr<Directory> make_dir(const string &name, weak_ptr<Directory> parent) {
        shared_ptr<Directory> dir = shared_ptr<Directory>(new Directory(name));
        dir->self = dir;
        dir->parent = parent.lock() != nullptr ? parent : dir;
        return dir;
    }

public:

    ~Directory() {
        if (debug) LOG4("Chiamato distruttore per \"", this->getName(), "\" indirizzo:", (void *) this);
    }

    /** <h3>Descrizione</h3>
     *  Restituisce la radice.<br>
     *  Se la radice e' un nullptr, allora invoca il metodo <i>make_dir</i> che crea la directory
     *  assegnando il nome '/' ed un weak_ptr a <i>nullptr</i>
     * @return shared_ptr di tipo Directory
     */
    static shared_ptr<Directory> getRoot() {
        if (root == nullptr) {
            root = make_dir("/", shared_ptr<Directory>(nullptr));
        }
        return root;
    }

    /** <h3>Descrizione</h3>
     *  Aggiunge una directory come figlio, se la directory esiste, restituisce il puntatore
     *  alla directory trovata.<br>
     *
     * @param name: Il nome della directory che si vuole creare
     * @return shared_ptr alla directory creata
     */
    shared_ptr<Directory> addDirectory(const string &name) {
        // Crea una nuova directory
        shared_ptr<Directory> dir = make_dir(name, self);
        // Salva il puntatore nella lista contenente i figli
        children.push_back(dir);
        return dir;
    }

    /** <h3>Descrizione</h3>
     *
     * Aggiunge un File, ritorna null se non lo puo' creare.<br>
     *
     * @param name: il nome del File che si vuole creare.
     * @param size: la dimensione del File che si sta creando.
     * @return file: lo shared_ptr che punta ad un elemento di tipo File, oppure null ptr
     */
    shared_ptr<File> addFile(const string &name, uintmax_t size) {
        shared_ptr<File> f = nullptr;
        try {
            f = make_shared<File>(name, size);
            children.push_back(f);
        } catch (const bad_alloc& e) {
            cout << "Allocation failed: " << e.what() << endl;
            f = nullptr;
            return f;
        }
        return f;
    }

    /** <h3>Descrizione</h3>
     * Restituisce una directory figlio il cui nome corrisponde a <i>name</i>
     * e permette di navigare tra
     * i figli(".") o il padre("..").<br>
     * Se si inserisce '.' , restituisce il puntatore a questa cartella. <br>
     * Se si inserice '..' , restituisce il puntatore al padre
     *
     * @param name: Il nome della directory figlio contenuta nella directory
     * in cui ci si trova oppure, inserendo ".." o "." e' possibile ottenere
     * il puntatore al <i>padre</i> o alla <i>cartella corrente</i>.
     * @return Uno shared_ptr di tipo <i>Base</i>
     */
    shared_ptr<Base> get(const std::string &name) {
        // E' stato richiesto il puntatore alla cartella corrente
        if (name == ".") return this->self.lock();

        // E' stato richiesto il puntatore al padre della cartella
        if (name == "..") return this->parent.lock();

        // Viene eseguito un controllo, se esiste il nome passato come parametro,
        // nella lista di figli della directory
        for (auto c: children) {
            if (c->getName() == name) {
                return c;
            }
        }

        // Se nessuno dei controlli precedenti e' positivo, allora viene restituito un puntatore a null
        return shared_ptr<Base>(nullptr);
    }


    /** <h3>Descrizione</h3>
     *  Fornisce uno shared_ptr<Directory> che punta ad una Directory.<br>
     *
     * @param name: Il nome della directory di cui si vuole ottenere un puntatore.
     * @return shared_ptr: puo' essere nullptr se la Directory di nome <b><i>name</i></b> non esiste.
     */
    shared_ptr<Directory> getDirectory(const string &name) {
        // Cerco di ottenere un puntatore ad una Directory
        shared_ptr<Base> dir = this->get(name);
        // Se il puntatore e' uguale a null oppure non si tratta di una directory
        if (dir == nullptr || dir->mediaType() != is_a_Dir) {
            // ritorno uno shared_ptr a nullptr
            return shared_ptr<Directory>(nullptr);
        } else {
            // eseguo un dynamic_pointer_cast di tipo Directory del valore del puntatore ottenuto
            // usando il metodo this->get(name).
            // dynamic pointer cast ritorna una copia dello shared_ptr<Base> e ne esegue il casting a
            // shared_ptr<Directory>
            return dynamic_pointer_cast<Directory>(dir);
        }
    };

    /** <h3>Descrizione</h3>
     *  Fornisce un puntatore di tipo shared_ptr<File> al file di nome <i>name</i>
     *  contenuto nella directory.<br>
     *
     * @return shared_ptr<File>: puo' essere nullptr qualora il nome del File non fosse presente.
     */
    shared_ptr<File> getFile(string name) {
        shared_ptr<Base> file = this->get(name);
        if (file == nullptr || file->mediaType() != is_a_File) {
            return shared_ptr<File>(nullptr);
        } else {
            return dynamic_pointer_cast<File>(file);
        }
    };

    /** <h3>Descrizione</h3>
     *  Rimuove dalla collezione di figli della directory corrente l'oggetto di nome <i>name</i>, se esiste,
     *  restituendo true.<br>
     *  Se l'oggetto indicato non esiste o si cerca di rimuovere ".." o "." viene restituito false
     * @return
     */
    bool remove(const string &name) {
        // Prima di tutto, cerco di ottenere un puntatore ad una directory
        shared_ptr<Base> target = this->get(name);
        // Se quel puntatore che ottengo e' diverso da nullptr allora lo rimuovo dai figli
        if (target != nullptr) {
            children.remove(target);
            return true;
        }
        // Se il puntatore che ottengo e' un nullptr, allora ritorno false
        return false;
    }

    /** <h3>Descrizione</h3>
    *  Restituisce il tipo di istanza (Directory) codificato come intero.<br>
    *
    *  @return is_a_dir: intero rappresentante il tipo di istanza, in questo caso vale 0
    */
    int mediaType() const override {
        return is_a_Dir;
    }

    /** <h3>Descrizione</h3>
     *  Stampa l'albero di directory a partire dalla directory corrente.<br>
     *
     * @param indent: Il numero di spazi di indentazione (uint)
     */
    void ls(uint indent = 0) const override {
        string rootF = "/";
        if (this->name != rootF) {
        for (int i = 0; i < indent; i++)
            std::cout << " ";
        }
        std::cout << "[+] " << name << endl;
        for (auto child: children) {
            child->ls(indent + 4);
        }
    }

};


#endif // BASE_H
