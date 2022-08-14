#include<stdio.h> 
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<iostream>
#include<sys/time.h> 

#include<string>
#include<map>
#include<vector>

#include<random>
#include<algorithm>

using std::string;
using std::random_device;
using std::default_random_engine;

using namespace std;

struct NF {
    string nf;
    int core;
};

struct SFC {
    int id;
    int node_id;
    int bottleneck;
    int current;
    int last;
    vector<NF> nfs;
};

map<string, int> _nf_metrics_local;
map<string, int> _nf_metrics_remote;
map<string, int> _solo_performance_local;
map<string, int> _solo_performance_remote;
map<string, vector<double>> _nf_prediction_parameters_local;
map<string, vector<double>> _nf_prediction_parameters_remote;
map<string, string> sfc_last_nf;
map<string, string> instance_to_core;

vector<string> nfs;

double myMin (double a, double b, double c) {
    double result = a;
    if (b < result) result = b;
    if (c < result) result = c;
    return result;
}

int myPow(int a, int b) { //caculate a**b
    int result = 1;
    for (int i=0;i<b;i++) {
        result *= a;
    }
    return result;
}

int mySum(vector<int> myVector) {
    int result = 0;
    for (int i=0;i<myVector.size();i++) {
        result += myVector[i];
    }
    return result;
}

bool vecCpr(vector<int> a, vector<int> b) {
    if (a.size() != b.size()) {
        return false;
    } else {
        for (int i=0;i<a.size();i++) {
            if (a[i] != b[i]) {
                return false;
            }
        }
        return true;
    }
}

void initMap() {
    _nf_metrics_local["nf_router"] = 94;
    _nf_metrics_local["aes_encrypt"] = 2000;  //487
    _nf_metrics_local["flow_tracker"] = 266;
    _nf_metrics_local["firewall"] = 194;

    _nf_metrics_remote["nf_router"] = 481;
    _nf_metrics_remote["aes_encrypt"] = 2000;  //884
    _nf_metrics_remote["flow_tracker"] = 617;
    _nf_metrics_remote["firewall"] = 576;

    _solo_performance_local["nf_router"] = 14791582;
    _solo_performance_local["aes_encrypt"] = 805120;
    _solo_performance_local["flow_tracker"] = 5350967;
    _solo_performance_local["firewall"] = 6783924;

    _solo_performance_remote["nf_router"] = 5169204;
    _solo_performance_remote["aes_encrypt"] = 825056;
    _solo_performance_remote["flow_tracker"] = 3520384;
    _solo_performance_remote["firewall"] = 3829262;

    vector<double> myVector;
    myVector.push_back(7.264703); myVector.push_back(-3.220732); myVector.push_back(0.280372);
    _nf_prediction_parameters_local["nf_router"] = myVector;
    myVector.clear();
    myVector.push_back(2.194409); myVector.push_back(-2.501355); myVector.push_back(0.090523);
    _nf_prediction_parameters_local["aes_encrypt"] = myVector;
    myVector.clear();
    myVector.push_back(6.978917); myVector.push_back(0.916245); myVector.push_back(0.072959);
    _nf_prediction_parameters_local["flow_tracker"] = myVector;
    myVector.clear();
    myVector.push_back(5.529852); myVector.push_back(0.298462); myVector.push_back(0.208711);
    _nf_prediction_parameters_local["firewall"] = myVector;
    myVector.clear();

    myVector.push_back(0.019525); myVector.push_back(3.504447); myVector.push_back(0.019525);
    _nf_prediction_parameters_remote["nf_router"] = myVector;
    myVector.clear();
    myVector.push_back(0.066159); myVector.push_back(-8.151804); myVector.push_back(0.066159);
    _nf_prediction_parameters_remote["aes_encrypt"] = myVector;
    myVector.clear();
    myVector.push_back(0.038925); myVector.push_back(16.161206); myVector.push_back(0.038925);
    _nf_prediction_parameters_remote["flow_tracker"] = myVector;
    myVector.clear();
    myVector.push_back(0.068971); myVector.push_back(8.947197); myVector.push_back(0.068971);
    _nf_prediction_parameters_remote["firewall"] = myVector;
    myVector.clear();

    sfc_last_nf["1"] = "0";
    sfc_last_nf["2"] = "1";
    sfc_last_nf["3"] = "2";
    sfc_last_nf["4"] = "0";
    sfc_last_nf["5"] = "4";
    sfc_last_nf["6"] = "5";
    sfc_last_nf["7"] = "0";
    sfc_last_nf["8"] = "7";
    sfc_last_nf["9"] = "8";
    sfc_last_nf["10"] = "0";
    sfc_last_nf["11"] = "10";
    sfc_last_nf["12"] = "11";
    sfc_last_nf["13"] = "0";
    sfc_last_nf["14"] = "13";
    sfc_last_nf["15"] = "14";
    sfc_last_nf["16"] = "0";
    sfc_last_nf["17"] = "16";
    sfc_last_nf["18"] = "17";

    instance_to_core["0"] = "0";
}

//prediction model for single NF
double prediction (string nf, double cache, double memory, bool local1, bool local2) {
    double predict_tmp;

    if (local1 == true && local2 == true) {
        predict_tmp = _nf_prediction_parameters_local[nf][0] * cache + _nf_prediction_parameters_local[nf][1] * memory + _nf_prediction_parameters_local[nf][2];
        return (1 - predict_tmp) * _solo_performance_local[nf];
    } else if (local1 == true && local2 == false) {
        predict_tmp = _nf_prediction_parameters_remote[nf][0] * cache + _nf_prediction_parameters_remote[nf][1] * memory + _nf_prediction_parameters_remote[nf][2];
        return (1 - predict_tmp) * _solo_performance_remote[nf];
    } else if (local1 == false && local2 == true) {
        predict_tmp = _nf_prediction_parameters_remote[nf][0] * cache + _nf_prediction_parameters_local[nf][1] * memory + _nf_prediction_parameters_local[nf][2];
        return (1 - predict_tmp) * _solo_performance_remote[nf];
    } else if (local1 == false && local2 == false) {
        predict_tmp = _nf_prediction_parameters_local[nf][0] * cache + _nf_prediction_parameters_remote[nf][1] * memory + _nf_prediction_parameters_remote[nf][2];
        return (1 - predict_tmp) * _solo_performance_local[nf];
    }
}

//function for single prediction
vector<double> single_prediction (vector<int> nf_distribution) {
    vector<double> nf_result;
    vector<string> local_nf;
    vector<string> remote_nf;

    for (int s=0;s<nf_distribution.size();s++) {
        instance_to_core[to_string(s+1)] = to_string(nf_distribution[s]);
        if (nf_distribution[s] == 0) {
            local_nf.push_back(to_string(s+1));
        } else {
            remote_nf.push_back(to_string(s+1));
        }
    }

    for (int t=0;t<nf_distribution.size();t++) {
        if ((instance_to_core[to_string(t+1)] == "0") && (instance_to_core[sfc_last_nf[to_string(t+1)]] == "0")) {
            double cache_local_tmp = 0;
            double memory_local_tmp = 0;

            for (vector<string>::iterator it = local_nf.begin(); it != local_nf.end(); it++) {
                if (find(local_nf.begin(), local_nf.end(), sfc_last_nf[*it]) == local_nf.end()) { //sfc_last_nf[*it] not in local_nf
                    cache_local_tmp += (double) 1 / _nf_metrics_local[nfs[stoi(*it, nullptr, 10) - 1]];
                    memory_local_tmp += (double) 1 / _nf_metrics_local[nfs[stoi(*it, nullptr, 10) - 1]];
                }
            }
            for (vector<string>::iterator it = remote_nf.begin(); it != remote_nf.end(); it++) {
                if (find(remote_nf.begin(), remote_nf.end(), sfc_last_nf[*it]) == remote_nf.end()) {  //sfc_last_nf[*it] not in remote_nf
                    memory_local_tmp += (double) 1 / _nf_metrics_local[nfs[stoi(*it, nullptr, 10) - 1]];
                }
            }
            nf_result.push_back(prediction(nfs[t], cache_local_tmp, memory_local_tmp, true, true));
        } else if ((instance_to_core[to_string(t+1)] == "0") && (instance_to_core[sfc_last_nf[to_string(t+1)]] == "1")) {
            double cache_local_tmp = 1;
            double memory_local_tmp = 0;

            for (vector<string>::iterator it = local_nf.begin(); it != local_nf.end(); it++) {
                if (find(local_nf.begin(), local_nf.end(), sfc_last_nf[*it]) == local_nf.end()) { //sfc_last_nf[*it] not in local_nf
                    memory_local_tmp += (double) 1 / _nf_metrics_local[nfs[stoi(*it, nullptr, 10) - 1]];
                }
            }
            for (vector<string>::iterator it = remote_nf.begin(); it != remote_nf.end(); it++) {
                if (find(remote_nf.begin(), remote_nf.end(), sfc_last_nf[*it]) == remote_nf.end()) {  //sfc_last_nf[*it] not in remote_nf
                    memory_local_tmp += (double) 1 / _nf_metrics_local[nfs[stoi(*it, nullptr, 10) - 1]];
                }
            }
            nf_result.push_back(prediction(nfs[t], cache_local_tmp, memory_local_tmp, false, true));
        } else if ((instance_to_core[to_string(t+1)] == "1") && (instance_to_core[sfc_last_nf[to_string(t+1)]] == "1")) {
            double cache_remote_tmp = 0;
            double memory_remote_tmp = 0;

            /*for (vector<string>::iterator it = local_nf.begin(); it != local_nf.end(); it++) {
                if (find(local_nf.begin(), local_nf.end(), sfc_last_nf[*it]) == local_nf.end()) { //sfc_last_nf[*it] not in local_nf
                    memory_remote_tmp += (double) 1 / _nf_metrics_remote[nfs[stoi(*it, nullptr, 10) - 1]];
                }
            }*/
            for (vector<string>::iterator it = remote_nf.begin(); it != remote_nf.end(); it++) {
                if (find(remote_nf.begin(), remote_nf.end(), sfc_last_nf[*it]) == remote_nf.end()) {  //sfc_last_nf[*it] not in remote_nf
                    cache_remote_tmp += (double) 1 / _nf_metrics_remote[nfs[stoi(*it, nullptr, 10) - 1]];
                    memory_remote_tmp += (double) 1 / _nf_metrics_remote[nfs[stoi(*it, nullptr, 10) - 1]];
                }
            }
            nf_result.push_back(prediction(nfs[t], cache_remote_tmp, memory_remote_tmp, false, false));
        } else if ((instance_to_core[to_string(t+1)] == "1") && (instance_to_core[sfc_last_nf[to_string(t+1)]] == "0")) {
            double cache_remote_tmp = 1;
            double memory_remote_tmp = 0;

            /*for (vector<string>::iterator it = local_nf.begin(); it != local_nf.end(); it++) {
                if (find(local_nf.begin(), local_nf.end(), sfc_last_nf[*it]) == local_nf.end()) {  //sfc_last_nf[*it] not in local_nf
                    memory_remote_tmp += (double) 1 /  _nf_metrics_remote[nfs[stoi(*it, nullptr, 10) - 1]];
                }
            }*/
            for (vector<string>::iterator it = remote_nf.begin(); it != remote_nf.end(); it++) {
                if (find(remote_nf.begin(), remote_nf.end(), sfc_last_nf[*it]) == remote_nf.end()) {  //sfc_last_nf[*it] not in remote_nf
                    memory_remote_tmp += (double) 1 / _nf_metrics_remote[nfs[stoi(*it, nullptr, 10) - 1]];
                }
            }
            nf_result.push_back(prediction(nfs[t], cache_remote_tmp, memory_remote_tmp, true, false));
        }
    }
    return nf_result;
}

//recursive search procedure
void multi_prediction (vector<SFC> sfcs, int server[], vector<int> nf_distribution, vector<double> nf_result, double sfc_result, vector<int> nfs_result) {
    int flag = 0;
    int i = 0;  //becareful not to use i in loops!!!
    int node0_core = 0;
    /*for (int j=0;j<nf_result.size();j++) {
        printf("%f\n", nf_result[j]);
    }*/
    

    for (vector<SFC>::iterator sfc = sfcs.begin(); sfc != sfcs.end(); sfc++) {
        double sfc_min = 1000000000;
        //int index_min = 0;
        for (vector<NF>::iterator nf = (*sfc).nfs.begin(); nf != (*sfc).nfs.end(); nf++) {
            if ((*nf).core == 0) {
                node0_core += 1;
            }
            if (sfc_min > nf_result[i]) {
                //(*sfc).bottleneck = index_min;
                //index_min += 1;
                (*sfc).bottleneck = i;
                sfc_min = nf_result[i];
            }
            i += 1;
        }
    }

    vector<int> distribution_tmp;
    distribution_tmp.insert(distribution_tmp.begin(), i, 0);
    vector<double> nf_result_tmp;
    nf_result_tmp.insert(nf_result_tmp.begin(), i, 0);
    double sfc_result_tmp = 0;

    vector<int> distribution;

    for (vector<SFC>::iterator sfc = sfcs.begin(); sfc != sfcs.end(); sfc++) {
        if ((*sfc).node_id != 0 && (*sfc).bottleneck >= (*sfc).current && (*sfc).bottleneck < (*sfc).last && (server[0] - node0_core) > ((*sfc).bottleneck - (*sfc).current)) {
            flag = 1;
            distribution.clear();
            for (vector<int>::iterator iterm = nf_distribution.begin(); iterm != nf_distribution.end(); iterm++) {
                distribution.push_back(*iterm);
            }
            for (int j=0; j < ((*sfc).bottleneck - (*sfc).current + 1); j++) {
                distribution[(*sfc).current + j] = 0;
            }
            /*for (vector<int>::iterator it = distribution.begin(); it != distribution.end(); it++) {
                cout << *it << endl;
            }*/
            nf_result = single_prediction(distribution);
            double result_tmp = myMin(nf_result[0], nf_result[1], nf_result[2]) + myMin(nf_result[3], nf_result[4], nf_result[5]) + myMin(nf_result[6], nf_result[7], nf_result[8]) + myMin(nf_result[9], nf_result[10], nf_result[11]) + myMin(nf_result[12], nf_result[13], nf_result[14]) + myMin(nf_result[15], nf_result[16], nf_result[17]);
            if (sfc_result_tmp < result_tmp) {
                sfc_result_tmp = result_tmp;
                for (int k=0;k<distribution.size();k++) {
                    distribution_tmp[k] = distribution[k];
                    nf_result_tmp[k] = nf_result[k];
                }
            }
            if (sfc_result < result_tmp) {
                sfc_result = result_tmp;
                for (int t=0;t<distribution.size();t++) {
                    nfs_result[t] = distribution[t];
                }
            }
        }
    }

    if (flag == 1) {
        int t = 0;
        for (vector<SFC>::iterator sfc = sfcs.begin(); sfc != sfcs.end(); sfc++) {
            for (vector<NF>::iterator nf = (*sfc).nfs.begin(); nf != (*sfc).nfs.end(); nf++) {
                (*nf).core = distribution_tmp[t];
                if (distribution[t] != (*sfc).node_id) {
                    (*sfc).current = (*sfc).bottleneck + 1;
                }
                t += 1;
            }
        }
        /*for (vector<int>::iterator it = distribution_tmp.begin(); it != distribution_tmp.end(); it++) {
            cout << *it << endl;
        }*/
        multi_prediction(sfcs, server, distribution_tmp, nf_result_tmp, sfc_result, nfs_result);
    } else {
        return;
    }
}

//heuristic placement algorithm
map<double, vector<int>> prediction_search (vector<string> nfs) {
    //timeval start1, start2, end;
    //gettimeofday(&start1, NULL);

    vector<SFC> sfcs;
    for (int sfc_id=0; sfc_id<6; sfc_id++) {
        SFC sfc;
        memset(&sfc, 0, sizeof(SFC));
        sfc.id = sfc_id;
        sfc.node_id = 0;
        sfc.bottleneck = 0;
        sfc.current = 0;
        sfc.last = 0;
        vector<NF> tmp;
        tmp.clear();
        sfc.nfs = tmp;
        sfcs.push_back(sfc);
    }
    for (int i=0;i<18;i++) {
        NF nf_tmp;
        memset(&nf_tmp, 0 ,sizeof(NF));
        nf_tmp.nf = nfs[i];
        nf_tmp.core = 0;
        /*if (i >= 0 && i < 3) {
            sfcs[0].current = 0;
            sfcs[0].last = 2;
            sfcs[0].nfs.push_back(nf_tmp);
        } else if (i >= 3 && i < 6) {
            sfcs[1].current = 3;
            sfcs[1].last = 5;
            sfcs[1].nfs.push_back(nf_tmp);
        } else if (i >= 6 && i < 9) {
            sfcs[2].current = 6;
            sfcs[2].last = 8;
            sfcs[2].nfs.push_back(nf_tmp);
        } else if (i >= 9 && i < 12) {
            sfcs[3].current = 9;
            sfcs[3].last = 11;
            sfcs[3].nfs.push_back(nf_tmp);
        } else if (i >= 12 && i < 15) {
            sfcs[4].current = 12;
            sfcs[4].last = 14;
            sfcs[4].nfs.push_back(nf_tmp);
        } else if (i >= 15 && i < 18) {
            sfcs[5].current = 15;
            sfcs[5].last = 17;
            sfcs[5].nfs.push_back(nf_tmp);
        }*/
        sfcs[i / 3].current = 3 * (i / 3);
        sfcs[i / 3].last = 3 * (i / 3) + 2;
        sfcs[i / 3].nfs.push_back(nf_tmp);
    }

    //server settings
    int server[2];
    server[0] = 11;
    server[1] = 11;
    int node_number = 2;

    int nf_number = nfs.size();
    int sfc_number = sfcs.size();
    int sfc_choice = myPow(node_number, sfc_number);

    vector<int> sfc_distribution;
    sfc_distribution.insert(sfc_distribution.begin(), sfc_number, 0);
    vector<int> nf_distribution;
    nf_distribution.insert(nf_distribution.begin(), nf_number, 0);
    //print distribution
    double sfc_result = 0;
    vector<int> nfs_result;
    nfs_result.insert(nfs_result.begin(), nf_number, 0);

    //gettimeofday(&start2, NULL);

    for (int i=0;i<sfc_choice;i++) {
        int k = 0;
        int iShadow = i;
        for (int j=0;j<sfc_number;j++) {
            sfc_distribution[j] = iShadow % node_number;
            iShadow = iShadow / node_number;
            if (sfc_distribution[j] != 0) {
                sfc_distribution[j] = 1;
            }
            sfcs[j].node_id = sfc_distribution[j];
            for (vector<NF>::iterator nf = sfcs[j].nfs.begin(); nf != sfcs[j].nfs.end(); nf++) {
                (*nf).core = sfc_distribution[j];
                nf_distribution[k] = sfc_distribution[j];
                k += 1;
                sfcs[0].current = 0;
                sfcs[1].current = 3;
                sfcs[2].current = 6;
                sfcs[3].current = 9;
                sfcs[4].current = 12;
                sfcs[5].current = 15;
            }
        }
        if ((nf_number - mySum(nf_distribution)) < server[0] && mySum(nf_distribution) < server[1]) {
            //print nf_distribution
            vector<double> nf_result = single_prediction(nf_distribution);
            //print nf_result
            //sfc special
            double result_tmp = myMin(nf_result[0], nf_result[1], nf_result[2]) + myMin(nf_result[3], nf_result[4], nf_result[5]) + myMin(nf_result[6], nf_result[7], nf_result[8]) + myMin(nf_result[9], nf_result[10], nf_result[11]) + myMin(nf_result[12], nf_result[13], nf_result[14]) + myMin(nf_result[15], nf_result[16], nf_result[17]);
            if (sfc_result < result_tmp) {
                //printf("sfc_result: %f, result_tmp: %f\n", sfc_result, result_tmp);
                //print nf_result
                sfc_result = result_tmp;
                for (int t=0;t<nf_distribution.size();t++) {
                    nfs_result[t] = nf_distribution[t];
                }
            }
            multi_prediction(sfcs, server, nf_distribution, nf_result, sfc_result, nfs_result);
        }
    }

    //gettimeofday(&end, NULL);
    //cout << "sfc search1 run time is : " << ((end.tv_usec - start1.tv_usec) + (end.tv_sec - start1.tv_sec) * 1000000) << "us" << endl;
    //cout << "sfc search2 run time is : " << ((end.tv_usec - start2.tv_usec) + (end.tv_sec - start2.tv_sec) * 1000000)  << "us" << endl;
    map<double, vector<int>> myMap;
    myMap[sfc_result] = nfs_result;
    return myMap;
}

//greedy placement algorithm
map<double, vector<int>> prediction_placement (vector<string> nfs) {
    timeval start1, start2, end;
    gettimeofday(&start1, NULL);

    int server[2];
    server[0] = 11;
    server[1] = 11;
    int node_number = 2;
    int nf_number = nfs.size();
    int nf_choice = myPow(node_number, nf_number);
    vector<int> distribution;
    distribution.insert(distribution.begin(), nf_number, 0);
    //print distribution
    double sfc_result = 0;
    vector<int> nfs_result;
    nfs_result.insert(nfs_result.begin(), nf_number, 0);

    gettimeofday(&start2, NULL);

    for (int i=0;i<nf_choice;i++) {
        int iShadow = i;
        for (int j=0;j<nf_number;j++) {
            distribution[j] = iShadow % node_number;
            iShadow = iShadow / node_number;
            if (distribution[j] != 0) {
                distribution[j] = 1;
            }
        }
        if ((nf_number - mySum(distribution)) < server[0] && mySum(distribution) < server[1]) {
            //print distribution
            vector<double> nf_result = single_prediction(distribution);
            //print distribution
            //print nf_result
            //sfc special
            double result_tmp = myMin(nf_result[0], nf_result[1], nf_result[2]) + myMin(nf_result[3], nf_result[4], nf_result[5]) + myMin(nf_result[6], nf_result[7], nf_result[8]) + myMin(nf_result[9], nf_result[10], nf_result[11]) + myMin(nf_result[12], nf_result[13], nf_result[14]) + myMin(nf_result[15], nf_result[16], nf_result[17]);
            if (sfc_result < result_tmp) {
                //printf("sfc_result: %f, result_tmp: %f\n", sfc_result, result_tmp);
                //print nf_result
                sfc_result = result_tmp;
                for (int k=0;k<distribution.size();k++) {
                    nfs_result[k] = distribution[k];
                }
            }
        }
    }

    gettimeofday(&end, NULL);
    //cout << "nf search1 run time is : " << ((end.tv_usec - start1.tv_usec) + (end.tv_sec - start1.tv_sec) * 1000000) << "us" << endl;
    //cout << "nf search2 run time is : " << ((end.tv_usec - start2.tv_usec) + (end.tv_sec - start2.tv_sec) * 1000000)  << "us" << endl;
    map<double, vector<int>> myMap;
    myMap[sfc_result] = nfs_result;
    return myMap;
}

int main() {
    initMap();

    string nf_set[4] = {"nf_router", "firewall", "flow_tracker", "aes_encrypt"};
    int total_time_search = 0;
    long long total_time_placement = 0;
    int hit_number = 0;
    int miss_number = 0;
    double miss_loss = 0;
    timeval start, end;

    random_device rd;
    default_random_engine random(rd());

    for (int i=0;i<10;i++) {
        map<double, vector<int>>::iterator a, b;
        nfs.clear();
        for (int j=0;j<18;j++) {
            nfs.push_back(nf_set[random() % 4]);
        }
        gettimeofday(&start, NULL);
        map<double, vector<int>> result_search = prediction_search(nfs);
        gettimeofday(&end, NULL);
        total_time_search += ((end.tv_usec - start.tv_usec) + (end.tv_sec - start.tv_sec) * 1000000);

        gettimeofday(&start, NULL);
        map<double, vector<int>> result_placement = prediction_placement(nfs);
        gettimeofday(&end, NULL);
        total_time_placement += (long long) ((end.tv_usec - start.tv_usec) + (end.tv_sec - start.tv_sec) * 1000000);

        a = result_search.begin();
        b = result_placement.begin();
        if (vecCpr(a->second, b->second)) {
            hit_number += 1;
        } else {
            miss_number += 1;
            miss_loss += (b->first - a->first) / b->first;
            printf("result_search: %f\n", a->first);
            printf("result_placement: %f\n", b->first);
        }
    }
    
    cout << "sfc search run time is : " << total_time_search << "us" << endl;
    cout << "sfc placement run time is : " << total_time_placement << "us" << endl;
    cout << "hit ratio is : " << (double) hit_number / 100 << endl;
    cout << "miss loss is : " << (double) miss_loss / miss_number << endl;

    return 0;
}