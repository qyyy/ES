#include <iostream>
#include <vector>
#include <ctime>
#include <random>
#include <algorithm>
using namespace std;

int calculate(pair<vector<int>,vector<float>> &x){
    return x.first[0]*x.first[0]-2*x.first[0]*x.first[1]+x.first[1]*x.first[1];
}

float cal_theata(float a, float b){
    default_random_engine e;
    normal_distribution<float> dis(0,6.5);
    return a*exp(b+dis(e));
}

bool cmp(pair<vector<int>,vector<float>> &a, pair<vector<int>,vector<float>> &b){
    return calculate(a) < calculate(b);
}

template <class T> bool is_out_of_bound(T x, T bound){
        return x>bound||x<-bound;
}

void calculate_mu_from_parent(vector<pair<vector<int>,vector<float>>> &parent, vector<pair<vector<int>,vector<float>>> &child, unsigned int mu, unsigned int i, default_random_engine &e, normal_distribution<float> &dis){
    for(unsigned int j=0;j<2;j++){
        do{
            child[i].second[j] = cal_theata(parent[i].second[j],dis(e));
        }while(is_out_of_bound(child[i].second[j], (float)6.5)||(child[i].second[j]>-0.000001&&child[i].second[j]<0.000001)||child[i].second[j]<=1.0);
        do{
            child[mu-i-1].second[j] = cal_theata(parent[i].second[j],dis(e));
        }while(is_out_of_bound(child[mu - i - 1].second[j], (float)6.5)||(child[mu - i - 1].second[j]>-0.000001&&child[mu - i - 1].second[j]<0.000001)||child[mu-i-1].second[j]<=1.0);
    }
}

void calculate_x_from_parent(vector<pair<vector<int>,vector<float>>> &parent, vector<pair<vector<int>,vector<float>>> &child, int mu, int i, default_random_engine &e, normal_distribution<float> &dis){
    for(unsigned int j=0;j<2;j++){
        do{
            child[i].first[j] = parent[i].first[j]+(int)(child[i].second[j]*dis(e));
        }while(is_out_of_bound(child[i].first[j], 10));
        do{
            child[mu-i-1].first[j] = parent[i].first[j]+(int)(child[i].second[j]*dis(e));
        }while(is_out_of_bound(child[mu - i - 1].first[j], 10));
    }
}

void train(vector<pair<vector<int>,vector<float>>> &parent, vector<pair<vector<int>,vector<float>>> &child, unsigned int parentLength, unsigned int mu){
    for(unsigned int i=0;i<parentLength;i++) {
        default_random_engine e;
        normal_distribution<float> dis(0,6.5);
        calculate_mu_from_parent(parent,child,mu,i,e,dis);
        normal_distribution<float> dis_1(0,child[i].second[0]);
        calculate_x_from_parent(parent,child,mu,i,e,dis_1);
    }
}

void copy_vector(vector<pair<vector<int>,vector<float>>> &parent, vector<pair<vector<int>,vector<float>>> &child, unsigned int &flag, unsigned int parentLength){
    for(int i=0;i<parentLength;i++) {
        parent[i] = child[i+10];
        if (i == parentLength - 1 && parent[i] == child[i+10])
            ++flag;
        else if (i == parentLength - 1)
            flag = 0;
    }
}

void print_vector(vector<pair<vector<int>,vector<float>>> &parent){
    for(auto my_item:parent)
        cout << calculate(my_item) << " " << my_item.second[0] << " " << my_item.second[1] << endl;
    system("pause");
}

template <class T> void swap_component(T &a, T &b){
    T temp = a;
    a = b;
    b = temp;
}

void random_swap_x(vector<pair<vector<int>,vector<float>>> &child, int i, int random_int){
    if(random_int==0)
        swap_component(child[i].second[0], child[i + 1].second[0]);
    else if(random_int==1)
        swap_component(child[i].second[0],child[i+1].second[1]);
    else if(random_int==2)
        swap_component(child[i].second[1],child[i+1].second[0]);
}

void random_swap_mu(vector<pair<vector<int>,vector<float>>> &child, int i, int random_int){
    if(random_int==0)
        swap_component(child[i].second[0],child[i+1].second[0]);
    else if(random_int==1)
        swap_component(child[i].second[0],child[i+1].second[1]);
    else if(random_int==2)
        swap_component(child[i].second[1],child[i+1].second[0]);
}

void crossover(vector<pair<vector<int>,vector<float>>> &child, unsigned int mu){
    srand((unsigned)time(0));
    for(unsigned int i = 0;i < mu;i+=2){
        int random_int = rand()%4;
        random_swap_x(child,i,random_int);
        random_int = rand()%4;
        random_swap_mu(child,i,random_int);
    }
}

int es_do(vector<pair<vector<int>,vector<float>>> &parent){
    unsigned int trainNumber = 100000;
    unsigned int mu = 20;
    unsigned int parentLength = parent.size();
    unsigned int unchangedNumber = 0;
    vector<pair<vector<int>,vector<float>>> child(mu,make_pair(vector<int>(mu,0),vector<float>(mu,0.0)));
    for(unsigned int _=0;_<trainNumber;_++){
        train(parent,child,parentLength,mu);
        crossover(child,mu);
        sort(child.begin(),child.end(),cmp);
        copy_vector(parent,child,unchangedNumber,parentLength);
        cout << parent[parentLength-1].first[0] << " " << parent[parentLength-1].first[1] << " " << parent[parentLength-1].second[0] << " " << parent[parentLength-1].second[1] << " " <<calculate(parent[parentLength-1]) << endl;
    }
    return calculate(parent[parentLength-1]);
}

void inti_vec(vector<pair<vector<int>,vector<float>>> &parent){//λ设为10，μ设为20
    srand((unsigned)time(NULL));
    int len = parent.size();
    for(int i=0;i<len;i++) {
        parent[i].first[0] = rand() % 21 - 10;
        parent[i].first[1] = rand() % 21 - 10;
        parent[i].second[0] = 1.0;
        parent[i].second[1] = 1.0;
    }
}

int main() {
    unsigned int lamda = 10;
    vector<pair<vector<int>,vector<float>>> parent(lamda,make_pair(vector<int>(lamda,0),vector<float>(lamda,0.0)));
    inti_vec(parent);
    cout << es_do(parent) << endl;
    return 0;
}