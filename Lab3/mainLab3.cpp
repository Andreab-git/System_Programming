#include <iostream>
#include <vector>
#include "lib/tracker.h"
#include "lib/my_shared.h"
#include "lib/process_tree.h"

using  namespace std;

template <typename D>
u_int Tracker<D>::count=0;

class Object: public Tracker<Object> {
};

void test_refs() {
    Object o;
    cout << "refs: " << Object::refs() << endl;
    {
        vector<Object> v(10);
        cout << "refs: " << Object::refs() << endl;
    }
    cout<<"refs: "<<Object::refs()<<endl;
}

void test_shared() {
    {
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
    ProcessManager pm;

    pm.update("proc_data1.txt");
    pm.list_processes_by_nfiles(0);

    pm.update("proc_data3.txt");
    pm.list_processes_by_nfiles(1204203); // chrome starter

    pm.list_files_by_nprocesses(0);

    pm.print_tree(0);
}

int main() {
    test_refs();
    //test_shared();
    //test_ptree();
}
