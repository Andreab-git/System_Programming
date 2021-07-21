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

/** <h3>Descrizione</h3>
 * Test per contare il numero di referenze che vengono istanziate.<br>
 * Vengono stampate a video. *
 */
void test_refs() {
    DurationLogger dl("Test references");
    {
        // Chiamo costruttore di Object che estende Tracker<Object>
        Object o;
        // Stampo a video il numero di references di Object che e' uguale a 1 in quanto e' stato istanziato
        // solamente un oggetto
        cout << "refs: " << Object::refs() << endl;
        // Definisco un vector di oggetti di tipo Object e della dimensione di 10 elementi
        vector<Object> v{10};
        // Stampo nuovamente a video il numero di references di Object.
        // A questo punto il numero corrisponde a 11 in quanto sono 1 + 10 contenuti nel vector
        cout << "refs: " << Object::refs() << endl;
    }
    // Appena esco dallo scope, mi ritrovo con 0 references in quanto tutti e 11 saranno distrutti
    // Da notare che si riesce comunque ad accedere alla variabile statica count
    cout << "refs: " << Object::refs() << endl;
}

void test_shared() {
    {
        DurationLogger dl("Test shared");
        my_shared<Object> sp(new Object());
        cout << "use " << sp.use_count() << endl;
        my_shared<Object> sp2(sp);
        my_shared<Object> sp3(new Object());
        cout << "use " << sp.use_count() << endl;
        cout << "Tracker " << sp->refs() << endl;
        sp3 = sp2;
        cout << "use " << sp.use_count() << endl;
        cout << "Tracker " << sp->refs() << endl;
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
    test_refs();
    //test_shared();
    //test_ptree();
}
