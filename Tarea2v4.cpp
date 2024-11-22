#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>
#include <chrono>
#include <ctime>
#include <fstream>
#include <string.h>
#include <vector>

class Monitor_P_C {
    public:
        Monitor_P_C(int N,int t, int p, int c){
            n = N;
            deftime = t;
            items = std::vector<int>(N,0);
            P = p;
            C = c;
            std::string name;
            time_t curr_time;
            curr_time = time(NULL);
            tm *tm_local = localtime(&curr_time);
            name = "output_y"+std::__cxx11::to_string(tm_local->tm_year+1900)+"d"+std::__cxx11::to_string(tm_local->tm_yday)+"h"+std::__cxx11::to_string(tm_local->tm_hour)+"m"+std::__cxx11::to_string(tm_local->tm_min)+"s"+std::__cxx11::to_string(tm_local->tm_sec)+".txt";      
            outputfile.open(name, std::fstream::out | std::fstream::app);
            outputfile << "Se a creado el Monitor\n";
        }
        // funcion usada por productor_thread para agregar elemento producido a la cola
        void Producir(int x){
            //bloqueo para tener uso exclusivo de lo datos del monitor
            std::lock_guard<std::mutex> lck(monitMutex);
            items[in] = x;
            count_items++;         
            if(count_items == n){
                // si se llena el vector items, se duplica su tamaño
                dupsize();
            }
            in = (in + 1) % n;    
            outputfile << "Se a agregado el item:" << x << " / la cola tiene: " << count_items << " items / la cola tiene una capacidad maxima de: " << n << " items\n";
            monitCond.notify_one();
        }
        // funcion usada por consumer_thread para consumir elemento de la cola
        void Consumir(int *x){
            //bloqueo para tener uso exclusivo de lo datos del monitor
            std::unique_lock<std::mutex> lck(monitMutex);
            while(count_items == 0){
                //en caso de ausencia de elementos en items consede bloqueo par que algun productor produsca si pasan 10 segundo y no hay elementos en items consumir termina terminando el hilo consumidor que solo usa la funcion consumir
                monitCond.wait_for(lck, std::chrono::seconds(deftime));
                if (count_items == 0){return;}
            }
            *x = items[out];
            count_items--;
            out = (out + 1) % n;         
            if(count_items <= n/4){
                //si el numero de items es menor a 1/4 del tamaño del vector items se disminuye el tamaño del vector items a la mitad
                halfsize();
            }
            outputfile << "Se a consumido un el item:" << *x << " / la cola tiene: " << count_items << " items / la cola tiene una capacidad maxima de: " << n << " items\n";
        }
        int getP(){return P;}
        int getC(){return C;}
    private:
        bool not_empty(){return count_items>0;}
        int n;
        std::vector<int> items;
        int deftime;
        int count_items = 0; 
        int in = 0, out = 0;
        int P,C;
        mutable std::mutex monitMutex;
        mutable std::condition_variable monitCond;
        std::fstream outputfile;
        //funcion para duplicar size de items
        void dupsize(){ 
            //crea un nuevo vector con el doble del tamaño de items
            std::vector<int> new_array(n*2,0);
            //pasa sus datos de la posicion 0 a n-1 para no tener que modificar in y out asi las posiciones se mantienen
            for (int i = 0; i < n; i++){
                new_array[i] = items[i];
            }
            //intercambia items con el vector recien creado
            items.swap(new_array);
            n = n*2;
            outputfile << "se a duplicado el tamaño de la cola\n";
        }      
        //funcion para dividir a la mitad size de items
        void halfsize(){     
            std::vector<int> new_array(n/2);
            //se copian los datos de items al nuevo vector y se redefinen in y out
            if (in < out){
                //si en el vector circular in es menor que out se copia de out hasta el final y desde el inicio hasta in que son los datos del arreglo
                copy(items.begin() + out, items.end(), new_array.begin());
                copy(items.begin(), items.begin() + in, new_array.begin()+(items.size()-out));
                in = out-in;
            }else{
                //si out es menor a in simplemente se copian los datos entre medio
                std::copy(items.begin() + out, items.begin() + in, new_array.begin());
                in = in-out;
            }
            //se define out como 0 gracia a la forma que fueron copiados los datos
            out = 0;
            //intercambia items con el vector recien creado
            items.swap(new_array);
            n = n/2;
            outputfile << "se a reducido a la mitad el tamaño de la cola\n";
        }
};

int main(int argc, char const *argv[]){  
    int P = -1,C = -1,s = -1,t = -1;
    //se busca los argumentos "-p,-c,-s,-t" y si su argumento siguiente es un numero se define la respectiva variable como con ese numero en caso de no ser un numero continua al siguiente argumento
    for (int i = 0; i < argc; ++i){
        if (strcmp(argv[i],"-p") == 0){
            try{
                P = std::__cxx11::stoi(argv[i+1]);
                i++;
                continue;
            }
            catch(std::invalid_argument& e){
                continue;
            }
        }
        if (strcmp(argv[i],"-c") == 0){
            try{
                C = std::__cxx11::stoi(argv[i+1]);
                i++;
                continue;
            }
            catch(std::invalid_argument& e){
                continue;
            }
        }
        if (strcmp(argv[i],"-s") == 0){
            try{
                s = std::__cxx11::stoi(argv[i+1]);
                i++;
                continue;
            }
            catch(std::invalid_argument& e){
                continue;
            }
        }
        if (strcmp(argv[i],"-t") == 0){
            try{
                t = std::__cxx11::stoi(argv[i+1]);
                i++;
                continue;
            }
            catch(std::invalid_argument& e){
                continue;
            }
        }
    }
    //si falto algun valor se pedira aqui
    while(P < 1){
        std::cout << P << std::endl;
        std::cout << "No se a introducido una cantidad de Productores o la cantidad introducida no es valida" << std::endl;
        std::cin >> P;
    }
    while(C < 1){
        std::cout << "No se a introducido una cantidad de Consumidores o la cantidad introducida no es valida" << std::endl;
        std::cin >> C;
    }
    while(s < 1){
        std::cout << "No se a introducido una capacidad de produccion o la capacidad introducida no es valida" << std::endl;
        std::cin >> s;
    }
    while(t < 1){
        std::cout << "No se a introducido una tiempo de espera de productores o el tiempo introducido no es valida" << std::endl;
        std::cin >> t;
    }
    Monitor_P_C MPC(s,t,P,C);
    // se crean los respectivos hilos donde os productores usan Producir y consumidores consumir
    std::vector<std::thread> hilos;
    for (int i = 0; i < MPC.getP(); i++){
        hilos.emplace_back([&MPC, i](){
                MPC.Producir(i);
        });
    }
    int salida;
    for (int i = 0; i < MPC.getC(); i++){
        hilos.emplace_back([&MPC, &salida](){
            MPC.Consumir(&salida);
        });
    }
    for (auto &hilo : hilos) if (hilo.joinable()) hilo.join();
    return 0;
}
