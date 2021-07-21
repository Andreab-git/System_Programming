#include <iostream>
#include <vector>
#include "lib/tracker.h"
#include "lib/my_shared.h"
#include "lib/process_tree.h"
#include "lib/duration_logger.h"

using namespace std;

template<typename D>
u_int Tracker<D>::count = 0;

class ClassaA : public Tracker<ClassaA> {
};

class ClassB : public Tracker<ClassB> {
};

/** <h3>Descrizione</h3>
 * Test per contare il numero di referenze che vengono istanziate.<br>
 * Viene usata la classe Tracker.
 * Vengono stampate a video. *
 */
void test_refs() {
    DurationLogger dl("Test references");
    {
        // Chiamo costruttore di ClassaA che estende Tracker<ClassaA>
        ClassaA o;
        ClassB o2;

        if (debug) {
            LOG1("");
            LOG1("---------------------------------------------------");
            LOG1("Stampa situazione iniziale ClassaA e ClassB");
            LOG1("---------------------------------------------------");
        }
        // Stampo a video il numero di references di ClassaA che e' uguale a 1 in quanto e' stato istanziato
        // solamente un oggetto
        cout << "refs di ClassaA: " << ClassaA::refs() << endl;
        cout << "refs di ClassB: " << ClassB::refs() << endl;
        // Definisco un vector di oggetti di tipo ClassaA e della dimensione di 10 elementi
        vector<ClassaA> v{10};
        vector<ClassB> v2{5};

        if (debug) {
            LOG1("");
            LOG1("---------------------------------------------------");
            LOG1("Stampa situazione ClassaA e ClassB dopo aver chiamato il costruttore di vector");
            LOG1("---------------------------------------------------");
        }
        // Stampo nuovamente a video il numero di references di ClassaA.
        // A questo punto il numero corrisponde a 11 in quanto sono 1 + 10 contenuti nel vector
        cout << "refs di ClassaA: " << ClassaA::refs() << endl;
        cout << "refs di ClassB: " << ClassB::refs() << endl;
    }

    if (debug) {
        LOG1("");
        LOG1("---------------------------------------------------");
        LOG1("Stampa situazione finale ClassaA e ClassB dopo essere usciti dallo scope");
        LOG1("---------------------------------------------------");
    }
    // Appena esco dallo scope, mi ritrovo con 0 references in quanto tutti e 11 saranno distrutti
    // Da notare che si riesce comunque ad accedere alla variabile statica count
    cout << "refs di ClassaA: " << ClassaA::refs() << endl;
    cout << "refs di ClassB: " << ClassB::refs() << endl;
}


/** <h3>Descrizione</h3>
 * Test per contare il numero di referenze che vengono istanziate.<br>
 * Viene usata la classe my_shared.
 * Vengono stampate a video. *
 */
void test_shared() {
    DurationLogger dl("Test shared");
    {
        if (debug) {
            LOG1("");
            LOG1("---------------------------------------------------");
            LOG1("Chiamata costruttore di my_shared passando parametro \"new ClassaA\"");
            LOG1("my_shared<ClassaA> sp{new ClassaA()};");
            LOG1("---------------------------------------------------");
        }
        my_shared<ClassaA> sp{new ClassaA()};
        cout << "Ptr counter di sp: " << sp.use_count() << endl;

        if (debug) {
            LOG1("");
            LOG1("---------------------------------------------------");
            LOG1("Chiamata costruttore passando un oggetto di tipo my_shared<T>");
            LOG1("my_shared<ClassaA> sp2(sp);");
            LOG1("---------------------------------------------------");
        }
        my_shared<ClassaA> sp2{sp};
        // Qui il counter ha valore 2 perche' quando viene creato l'oggetto sp2, viene usato un copy costructor
        // che nell'ultima istruzione, va ad incrementare il valore del counter di sp2.
        cout << "Ptr counter di sp2: " << sp2.use_count() << endl;

        if (debug) {
            LOG1("");
            LOG1("---------------------------------------------------");
            LOG1("Chiamata costruttore di my_shared passando parametro \"new ClassaA\"");
            LOG1("my_shared<ClassaA> sp3(new ClassaA());");
            LOG1("---------------------------------------------------");
        }
        my_shared<ClassaA> sp3{new ClassaA()};

        // Dato che sp ed sp2 hanno un puntatore a count uguale, sp ed sp2 avranno lo stesso valore di count
        // In questo caso il valore e' 2.
        cout << "Ptr counter di sp: " << sp.use_count() << endl;
        cout << "Ptr counter di sp2: " << sp2.use_count() << endl;

        // sp3 viene creato utilizzando nuovamente il costruttore my_shared(T *ptr): ref(ptr), count(new u_int(1))
        // pertanto avra' un counter inizializzato a 1 che corrisponde al valore stampato a video.
        cout << "Ptr counter di sp3: " << sp3.use_count() << endl;

        // Essendo stato utilizzato solamente 2 volte il costruttore my_shared(T *ptr): ref(ptr), count(new u_int(1)),
        // e' stata fatta solo 2 volte la new ClassaA pertanto il valore count registrato da Tracker sara' uguale a 2.
        cout << "Tracker sp:" << sp->refs() << endl;
        cout << "Tracker sp2:" << sp2->refs() << endl;
        cout << "Tracker sp3:" << sp3->refs() << endl;

        // In seguito all'overloading dell'operatore di assegnazione verra' eseguito uno swap tra sp2 ed sp3.
        sp3 = sp2;
        cout << "Ptr counter di sp: " << sp.use_count() << endl;
        cout << "Ptr counter di sp2: " << sp2.use_count() << endl;
        cout << "Ptr counter di sp3: " << sp3.use_count() << endl;
        cout << "Tracker sp:" << sp->refs() << endl;
        cout << "Tracker sp2:" << sp2->refs() << endl;
        cout << "Tracker sp3:" << sp3->refs() << endl;
    }

    cout << "Tracker " << ClassaA::refs() << endl;
}


void test_ptree() {
    {
//        DurationLogger dl("Test p_tree");
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
//    test_refs();

    /* Esercizio 2: Implementare una versione semplificata di shared_ptr (senza gestire i weak_ptr) che
     permetta di tenere traccia delle referenze a puntatori nativi e de-allocarli automaticamente
     quando le referenze sono a zero */
//    test_shared();

    /*
     * Esercizio 3: Esaminare /proc
     */
    test_ptree();
}
