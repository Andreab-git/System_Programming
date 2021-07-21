#include <iostream>
#include <vector>
#include "lib/tracker.h"
#include "lib/my_shared.h"
#include "lib/process_tree.h"
#include "lib/duration_logger.h"

using namespace std;

template<typename D>
u_int Tracker<D>::count = 0;

class Object : public Tracker<Object> {
};

class Object2 : public Tracker<Object2> {
};

/** <h3>Descrizione</h3>
 * Test per contare il numero di referenze che vengono istanziate.<br>
 * Vengono stampate a video. *
 */
void test_refs() {
    DurationLogger dl("Test references");
    {
        // Chiamo costruttore di Object che estende Tracker<Object>
        Object o;
        Object2 o2;

        if (debug) {
            LOG1("");
            LOG1("---------------------------------------------------");
            LOG1("Stampa situazione iniziale Object e Object2");
            LOG1("---------------------------------------------------");
        }
        // Stampo a video il numero di references di Object che e' uguale a 1 in quanto e' stato istanziato
        // solamente un oggetto
        cout << "refs di Object: " << Object::refs() << endl;
        cout << "refs di Object2: " << Object2::refs() << endl;
        // Definisco un vector di oggetti di tipo Object e della dimensione di 10 elementi
        vector<Object> v{10};
        vector<Object2> v2{5};

        if (debug) {
            LOG1("");
            LOG1("---------------------------------------------------");
            LOG1("Stampa situazione Object e Object2 dopo aver chiamato il costruttore di vector");
            LOG1("---------------------------------------------------");
        }
        // Stampo nuovamente a video il numero di references di Object.
        // A questo punto il numero corrisponde a 11 in quanto sono 1 + 10 contenuti nel vector
        cout << "refs di Object: " << Object::refs() << endl;
        cout << "refs di Object2: " << Object2::refs() << endl;
    }

    if (debug) {
        LOG1("");
        LOG1("---------------------------------------------------");
        LOG1("Stampa situazione finale Object e Object2 dopo essere usciti dallo scope");
        LOG1("---------------------------------------------------");
    }
    // Appena esco dallo scope, mi ritrovo con 0 references in quanto tutti e 11 saranno distrutti
    // Da notare che si riesce comunque ad accedere alla variabile statica count
    cout << "refs di Object: " << Object::refs() << endl;
    cout << "refs di Object2: " << Object2::refs() << endl;
}

void test_shared() {
    DurationLogger dl("Test shared");
    {
        if (debug) {
            LOG1("");
            LOG1("---------------------------------------------------");
            LOG1("Chiamata costruttore di my_shared passando parametro \"new Object\"");
            LOG1("my_shared<Object> sp{new Object()};");
            LOG1("---------------------------------------------------");
        }
        my_shared<Object> sp{new Object()};
        cout << "Ptr counter di sp: " << sp.use_count() << endl;

        if (debug) {
            LOG1("");
            LOG1("---------------------------------------------------");
            LOG1("Chiamata costruttore passando un oggetto di tipo my_shared<T>");
            LOG1("my_shared<Object> sp2(sp);");
            LOG1("---------------------------------------------------");
        }
        my_shared<Object> sp2{sp};
        cout << "Ptr counter di sp2: " << sp2.use_count() << endl;

        if (debug) {
            LOG1("");
            LOG1("---------------------------------------------------");
            LOG1("Chiamata costruttore di my_shared passando parametro \"new Object\"");
            LOG1("my_shared<Object> sp3(new Object());");
            LOG1("---------------------------------------------------");
        }
        my_shared<Object> sp3{new Object()};

        cout << "Ptr counter di sp: " << sp.use_count() << endl;
        cout << "Ptr counter di sp2: " << sp2.use_count() << endl;
        cout << "Ptr counter di sp3: " << sp3.use_count() << endl;
        cout << "Tracker sp:" << sp->refs() << endl;
        cout << "Tracker sp2:" << sp2->refs() << endl;
        cout << "Tracker sp3:" << sp3->refs() << endl;
        sp3 = sp2;
        cout << "Ptr counter di sp: " << sp.use_count() << endl;
        cout << "Ptr counter di sp2: " << sp2.use_count() << endl;
        cout << "Ptr counter di sp3: " << sp3.use_count() << endl;
        cout << "Tracker sp:" << sp->refs() << endl;
        cout << "Tracker sp2:" << sp2->refs() << endl;
        cout << "Tracker sp3:" << sp3->refs() << endl;
    }

    cout << "Tracker " << Object::refs() << endl;
}


void test_ptree() {
    {
        DurationLogger dl("Test p_tree");
        ProcessManager pm;

        pm.update("proc_data1.txt");
        pm.list_processes_by_nfiles(0);

        pm.update("proc_data3.txt");
        pm.list_processes_by_nfiles(1204203); // chrome starter

        pm.list_files_by_nprocesses(0);

        pm.print_tree(0);
    }
}

int main() {
    // Esercizio 1: Implementare un tracker in grado di tracciare in modo distinto classi arbitrarie.
    //test_refs();

    /* Esercizio 2: Implementare una versione semplificata di shared_ptr (senza gestire i weak_ptr) che
     permetta di tenere traccia delle referenze a puntatori nativi e de-allocarli automaticamente
     quando le referenze sono a zero */
    test_shared();


    //test_ptree();
}
