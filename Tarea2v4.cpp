#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>
#include <chrono>
#include <ctime>
#include <fstream>
#include <string.h>

class Monitor_P_C {
    public:
        Monitor_P_C(int N,int t, int p, int c){
            std::cout << "enter";
            n = N;
            deftime = t;
            items = new int[N];
            P = p;
            C = c;
            std::string name;
            time_t curr_time;
            curr_time = time(NULL);
            tm *tm_local = localtime(&curr_time);
            name = "output_y"+std::__cxx11::to_string(tm_local->tm_year+1900)+"d"+std::__cxx11::to_string(tm_local->tm_yday)+"h"+std::__cxx11::to_string(tm_local->tm_hour)+"m"+std::__cxx11::to_string(tm_local->tm_min)+"s"+std::__cxx11::to_string(tm_local->tm_sec)+".txt";      
            outputfile.open(name, std::fstream::out | std::fstream::app);
            std::cout << "contruct";
        }
        void Producir(int x){
            std::lock_guard<std::mutex> lck(monitMutex);
            items[in] = x;
            in = (in + 1) % n;    
            count_items++;         
            if(count_items == n){
                dupsize();
            }
            monitCond.notify_one();
        }
        void Consumir(int *x){
            std::unique_lock<std::mutex> lck(monitMutex);
            while(count_items == 0){
                monitCond.wait_for(lck, std::chrono::seconds(deftime));
                if (count_items == 0){return;}
            }
            *x = items[out];
            out = (out + 1) % n;
            count_items--;         
            if(count_items == n/4){
                halfsize();
            }
        }
        int getP(){return P;}
        int getC(){return C;}
    private:
        bool not_empty(){return count_items>0;}
        int n;
        int* items;
        int deftime;
        int count_items = 0; 
        int in = 0, out = 0;
        int P,C;
        mutable std::mutex monitMutex;
        mutable std::condition_variable monitCond;
        std::fstream outputfile;
        void dupsize(){
            int* new_array = new int[n*2];
            for (int i = 0; i < n; i++){
                new_array[i] = items[i];
            }
            items = new_array;
            n = n*2;
            outputfile << "se a duplicado el tamaño de la cola\n";
        }        
        void halfsize(){       
            int i = 0;
            int* new_array = new int[n/2];
            while(in != out){
                new_array[i] = items[in];
                in = (in+1)%n;
                i++;
            }
            in = 0;
            out = i;
            items = new_array;
            n = n*2;
            outputfile << "se a reducido a la mitad el tamaño de la cola\n";
        }
};

int main(int argc, char const *argv[]){  
    std::cout << "pre-arg"; 
    int P = -1,C = -1,s = -1,t = -1;
    std::cout << "pre-arg";
    for (int i = 0; i < argc; ++i){
        if (strcmp(argv[i],"-p") == 0){
            P = std::__cxx11::stoi(argv[i+1]);
            i++;
            continue;
        }
        if (strcmp(argv[i],"-c") == 0){
            C = std::__cxx11::stoi(argv[i+1]);
            i++;
            continue;
        }
        if (strcmp(argv[i],"-s") == 0){
            s = std::__cxx11::stoi(argv[i+1]);
            i++;
            continue;
        }
        if (strcmp(argv[i],"-t") == 0){
            t = std::__cxx11::stoi(argv[i+1]);
            i++;
            continue;
        }
    }
    std::cout << "arg";
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
    std::thread hilo_productor([&MPC](){
        for (int i = 0; i < MPC.getP(); ++i){
            MPC.Producir(i);
        }
    });
    std::thread hilo_consumidor([&MPC](){
        for (int i = 0; i < MPC.getC(); ++i){
            int out;
            MPC.Consumir(&out);
        }
    });

    hilo_consumidor.join();
    hilo_productor.join();
    return 0;
}
