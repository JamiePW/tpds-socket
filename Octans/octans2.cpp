#include<stdio.h> 
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<iostream>
#include<sys/time.h> 

#include<string>
#include<map>
#include<unordered_map>
#include<vector>

#include<random>
#include<algorithm>

using std::string;
using std::random_device;
using std::default_random_engine;

using namespace std;

const int NF_ROUTER = 0;
const int AES_ENCRYPT = 1;
const int FLOW_TRACKER = 2;
const int FIREWALL = 3;

const int SFC_NUMBER = 6;
const int CHAIN_LEN = 18;

struct NF {
    int nf;
    int core;
};

struct SFC {
    int id;
    int node_id;
    int bottleneck;
    int current;
    int last;
    NF nfs[3];
};

int _nf_metrics_local[4];
int _nf_metrics_remote[4];
int _solo_performance_local[4];
int _solo_performance_remote[4];
double _nf_prediction_parameters_local[4][3];
double _nf_prediction_parameters_remote[4][3];
int sfc_last_nf[20];
int instance_to_core[20];

int nfs[CHAIN_LEN];

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

int mySum(int distribution[]) {
    int result = 0;
    for (int i=0;i<CHAIN_LEN;i++) {
        result += distribution[i];
    }
    return result;
}

bool myCpr(int a[], int b[]) {
    for (int i=0;i<CHAIN_LEN;i++) {
         if (a[i] != b[i]) {
            return false;
          }
    }
    return true;
}

void _init_() {
    _nf_metrics_local[NF_ROUTER] = 94;
    _nf_metrics_local[AES_ENCRYPT] = 2000;  //487
    _nf_metrics_local[FLOW_TRACKER] = 266;
    _nf_metrics_local[FIREWALL] = 194;

    _nf_metrics_remote[NF_ROUTER] = 481;
    _nf_metrics_remote[AES_ENCRYPT] = 2000;  //884
    _nf_metrics_remote[FLOW_TRACKER] = 617;
    _nf_metrics_remote[FIREWALL] = 576;

    _solo_performance_local[NF_ROUTER] = 14791582;
    _solo_performance_local[AES_ENCRYPT] = 805120;
    _solo_performance_local[FLOW_TRACKER] = 5350967;
    _solo_performance_local[FIREWALL] = 6783924;

    _solo_performance_remote[NF_ROUTER] = 5169204;
    _solo_performance_remote[AES_ENCRYPT] = 825056;
    _solo_performance_remote[FLOW_TRACKER] = 3520384;
    _solo_performance_remote[FIREWALL] = 3829262;

    _nf_prediction_parameters_local[NF_ROUTER][0] = 7.264703; _nf_prediction_parameters_local[NF_ROUTER][1] = -3.220732; _nf_prediction_parameters_local[NF_ROUTER][2] = 0.280372;
    _nf_prediction_parameters_local[AES_ENCRYPT][0] = 2.194409; _nf_prediction_parameters_local[AES_ENCRYPT][1] = -2.501355; _nf_prediction_parameters_local[AES_ENCRYPT][2] = 0.090523;
    _nf_prediction_parameters_local[FLOW_TRACKER][0] = 6.978917; _nf_prediction_parameters_local[FLOW_TRACKER][1] = 0.916245; _nf_prediction_parameters_local[FLOW_TRACKER][2] = 0.072959;
    _nf_prediction_parameters_local[FIREWALL][0] = 5.529852; _nf_prediction_parameters_local[FIREWALL][1] = 0.298462; _nf_prediction_parameters_local[FIREWALL][2] = 0.208711;

    _nf_prediction_parameters_remote[NF_ROUTER][0] = 0.019525; _nf_prediction_parameters_remote[NF_ROUTER][1] = 3.504447; _nf_prediction_parameters_remote[NF_ROUTER][2] = 0.019525;
    _nf_prediction_parameters_remote[AES_ENCRYPT][0] = 0.066159; _nf_prediction_parameters_remote[AES_ENCRYPT][1] = -8.151804; _nf_prediction_parameters_remote[AES_ENCRYPT][2] = 0.066159;
    _nf_prediction_parameters_remote[FLOW_TRACKER][0] = 0.038925; _nf_prediction_parameters_remote[FLOW_TRACKER][1] = 16.161206; _nf_prediction_parameters_remote[FLOW_TRACKER][2] = 0.038925;
    _nf_prediction_parameters_remote[FIREWALL][0] = 0.068971; _nf_prediction_parameters_remote[FIREWALL][1] = 8.947197; _nf_prediction_parameters_remote[FIREWALL][2] = 0.068971;

    sfc_last_nf[1] = 0;
    sfc_last_nf[2] = 1;
    sfc_last_nf[3] = 2;
    sfc_last_nf[4] = 0;
    sfc_last_nf[5] = 4;
    sfc_last_nf[6] = 5;
    sfc_last_nf[7] = 0;
    sfc_last_nf[8] = 7;
    sfc_last_nf[9] = 8;
    sfc_last_nf[10] = 0;
    sfc_last_nf[11] = 10;
    sfc_last_nf[12] = 11;
    sfc_last_nf[13] = 0;
    sfc_last_nf[14] = 13;
    sfc_last_nf[15] = 14;
    sfc_last_nf[16] = 0;
    sfc_last_nf[17] = 16;
    sfc_last_nf[18] = 17;

    instance_to_core[0] = 0;
}

//prediction model for single NF
double prediction (int nf, double cache, double memory, bool local1, bool local2) {
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
double * single_prediction (int nf_distribution[]) {
    double *nf_result = new double[CHAIN_LEN];
    int local_nf[CHAIN_LEN];
    int remote_nf[CHAIN_LEN];
    int local_len = 0;
    int remote_len = 0;

    for (int s=0;s < CHAIN_LEN;s++) {
        instance_to_core[s+1] = nf_distribution[s];
        if (nf_distribution[s] == 0) {
            local_nf[local_len] = s+1;
            local_len += 1;
        } else {
            remote_nf[remote_len] = s+1;
            remote_len += 1;
        }
    }

    for (int t=0;t < CHAIN_LEN;t++) {
        if ((instance_to_core[t+1] == 0) && (instance_to_core[sfc_last_nf[t+1]] == 0)) {
            double cache_local_tmp = 0;
            double memory_local_tmp = 0;

            for (int i=0;i<local_len;i++) {
                if (nf_distribution[sfc_last_nf[local_nf[i]] - 1] != 0 || sfc_last_nf[local_nf[i]] - 1 < 0) { //sfc_last_nf[*it] not in local_nf
                    cache_local_tmp += (double) 1 / _nf_metrics_local[nfs[local_nf[i] - 1]];
                    memory_local_tmp += (double) 1 / _nf_metrics_local[nfs[local_nf[i] - 1]];
                }
            }
            for (int i=0;i<remote_len;i++) {
                if (nf_distribution[sfc_last_nf[remote_nf[i]] - 1] == 0 || sfc_last_nf[remote_nf[i]] - 1 < 0) {  //sfc_last_nf[*it] not in remote_nf
                    memory_local_tmp += (double) 1 / _nf_metrics_local[nfs[remote_nf[i] - 1]];
                }
            }
            nf_result[t] = prediction(nfs[t], cache_local_tmp, memory_local_tmp, true, true);
        } else if ((instance_to_core[t+1] == 0) && (instance_to_core[sfc_last_nf[t+1]] == 1)) {
            double cache_local_tmp = 1;
            double memory_local_tmp = 0;

            for (int i=0;i<local_len;i++) {
                if (nf_distribution[sfc_last_nf[local_nf[i]] - 1] != 0 || sfc_last_nf[local_nf[i]] - 1 < 0) { //sfc_last_nf[*it] not in local_nf
                    memory_local_tmp += (double) 1 / _nf_metrics_local[nfs[local_nf[i] - 1]];
                }
            }
            for (int i=0;i<remote_len;i++) {
                if (nf_distribution[sfc_last_nf[remote_nf[i]] - 1] == 0 || sfc_last_nf[remote_nf[i]] - 1 < 0) {  //sfc_last_nf[*it] not in remote_nf
                    memory_local_tmp += (double) 1 / _nf_metrics_local[nfs[remote_nf[i] - 1]];
                }
            }
            nf_result[t] = prediction(nfs[t], cache_local_tmp, memory_local_tmp, false, true);
        } else if ((instance_to_core[t+1] == 1) && (instance_to_core[sfc_last_nf[t+1]] == 1)) {
            double cache_remote_tmp = 0;
            double memory_remote_tmp = 0;

            /*for (vector<int>::iterator it = local_nf.begin(); it != local_nf.end(); it++) {
                if (find(local_nf.begin(), local_nf.end(), sfc_last_nf[*it]) == local_nf.end()) { //sfc_last_nf[*it] not in local_nf
                    memory_remote_tmp += (double) 1 / _nf_metrics_remote[nfs[stoi(*it, nullptr, 10) - 1]];
                }
            }*/
            for (int i=0;i<remote_len;i++) {
                if (nf_distribution[sfc_last_nf[remote_nf[i]] - 1] == 0 || sfc_last_nf[remote_nf[i]] - 1 < 0) {  //sfc_last_nf[*it] not in remote_nf
                    cache_remote_tmp += (double) 1 / _nf_metrics_remote[nfs[remote_nf[i] - 1]];
                    memory_remote_tmp += (double) 1 / _nf_metrics_remote[nfs[remote_nf[i] - 1]];
                }
            }
            nf_result[t] = prediction(nfs[t], cache_remote_tmp, memory_remote_tmp, false, false);
        } else if ((instance_to_core[t+1] == 1) && (instance_to_core[sfc_last_nf[t+1]] == 0)) {
            double cache_remote_tmp = 1;
            double memory_remote_tmp = 0;

            /*for (vector<int>::iterator it = local_nf.begin(); it != local_nf.end(); it++) {
                if (find(local_nf.begin(), local_nf.end(), sfc_last_nf[*it]) == local_nf.end()) {  //sfc_last_nf[*it] not in local_nf
                    memory_remote_tmp += (double) 1 /  _nf_metrics_remote[nfs[stoi(*it, nullptr, 10) - 1]];
                }
            }*/
            for (int i=0;i<remote_len;i++) {
                if (nf_distribution[sfc_last_nf[remote_nf[i]] - 1] == 0 || sfc_last_nf[remote_nf[i]] - 1 < 0) {  //sfc_last_nf[*it] not in remote_nf
                    memory_remote_tmp += (double) 1 / _nf_metrics_remote[nfs[remote_nf[i] - 1]];
                }
            }
            nf_result[t] = prediction(nfs[t], cache_remote_tmp, memory_remote_tmp, true, false);
        }
    }
    return nf_result;
}

//recursive search procedure
void multi_prediction (SFC sfcs[], int server[], int nf_distribution[], double nf_result[], double sfc_result, int nfs_result[]) {
    //timeval start, end;
    int flag = 0;
    int i = 0;  //becareful not to use i in loops!!!
    int node0_core = 0;
    /*for (int j=0;j<nf_result.size();j++) {
        printf("%f\n", nf_result[j]);
    }*/

    //gettimeofday(&start, NULL);

    for (int j=0;j < SFC_NUMBER;j++) {
        double sfc_min = 1000000000;
        //int index_min = 0;
        for (int k=0;k<3;k++) {
            if (sfcs[j].nfs[k].core == 0) {
                node0_core += 1;
            }
            if (sfc_min > nf_result[i]) {
                //(*sfc).bottleneck = index_min;
                //index_min += 1;
                sfcs[j].bottleneck = i;
                sfc_min = nf_result[i];
            }
            i += 1;
        }
    }

    int distribution_tmp[i];
    for (int j=0;j<i;j++) {
        distribution_tmp[j] = 0;
    }
    double nf_result_tmp[i];
    for (int j=0;j<i;j++) {
        nf_result_tmp[j] = 0;
    }
    double sfc_result_tmp = 0;

    int distribution[CHAIN_LEN];

    for (int k=0;k < SFC_NUMBER;k++) {
        if (sfcs[k].node_id != 0 && sfcs[k].bottleneck >= sfcs[k].current && sfcs[k].bottleneck < sfcs[k].last && (server[0] - node0_core) > (sfcs[k].bottleneck - sfcs[k].current)) {
            flag = 1;
            //distribution.clear();
            for (int j=0;j<CHAIN_LEN;j++) {
                distribution[j] = 0;
            }
            for (int j=0;j<CHAIN_LEN;j++) {
                distribution[j] = nf_distribution[j];
            }
            for (int j=0; j < (sfcs[k].bottleneck - sfcs[k].current + 1); j++) {
                distribution[sfcs[k].current + j] = 0;
            }
            /*for (vector<int>::iterator it = distribution.begin(); it != distribution.end(); it++) {
                cout << *it << endl;
            }*/
            nf_result = single_prediction(distribution);
            double result_tmp = myMin(nf_result[0], nf_result[1], nf_result[2]) + myMin(nf_result[3], nf_result[4], nf_result[5]) + myMin(nf_result[6], nf_result[7], nf_result[8]) + myMin(nf_result[9], nf_result[10], nf_result[11]) + myMin(nf_result[12], nf_result[13], nf_result[14]) + myMin(nf_result[15], nf_result[16], nf_result[17]);
            if (sfc_result_tmp < result_tmp) {
                sfc_result_tmp = result_tmp;
                for (int k=0;k<CHAIN_LEN;k++) {
                    distribution_tmp[k] = distribution[k];
                    nf_result_tmp[k] = nf_result[k];
                }
            }
            if (sfc_result < result_tmp) {
                sfc_result = result_tmp;
                for (int t=0;t<CHAIN_LEN;t++) {
                    nfs_result[t] = distribution[t];
                }
            }
        }
    }

    if (flag == 1) {
        int t = 0;
        for (int j=0;j<SFC_NUMBER;j++) {
            for (int k=0;k<3;k++) {
                sfcs[j].nfs[k].core = distribution_tmp[t];
                if (distribution[t] != sfcs[j].node_id) {
                    sfcs[j].current = sfcs[j].bottleneck + 1;
                }
                t += 1;
            }
        }
        /*for (vector<int>::iterator it = distribution_tmp.begin(); it != distribution_tmp.end(); it++) {
            cout << *it << endl;
        }*/
        multi_prediction(sfcs, server, distribution_tmp, nf_result_tmp, sfc_result, nfs_result);
    } else {
        //gettimeofday(&end, NULL);
        //cout << "multi_prediction time: " << ((end.tv_usec - start.tv_usec) + (end.tv_sec - start.tv_sec) * 1000000) << "us" << endl;
        return;
    }
}

//heuristic placement algorithm
map<double, int*> prediction_search (int nfs[]) {
    timeval start1, start2, end;
    gettimeofday(&start1, NULL);

    SFC sfcs[SFC_NUMBER];
    for (int sfc_id=0; sfc_id<SFC_NUMBER; sfc_id++) {
        SFC sfc;
        memset(&sfc, 0, sizeof(SFC));
        sfc.id = sfc_id;
        sfc.node_id = 0;
        sfc.bottleneck = 0;
        sfc.current = 0;
        sfc.last = 0;
        NF tmp[3] = {0};
        //sfc.nfs = &tmp;
        memset(sfc.nfs, 0, 3 * sizeof(NF));
        sfcs[sfc_id] = sfc;
    }
    for (int i=0;i<18;i++) {
        NF nf_tmp;
        nf_tmp.nf = nfs[i];
        nf_tmp.core = 0;
        sfcs[i / 3].current = 3 * (i / 3);
        sfcs[i / 3].last = 3 * (i / 3) + 2;
        sfcs[i / 3].nfs[i % 3] = nf_tmp;
    }

    //server settings
    int server[2];
    server[0] = 11;
    server[1] = 11;
    int node_number = 2;

    int nf_number = CHAIN_LEN;
    int sfc_number = SFC_NUMBER;
    int sfc_choice = myPow(node_number, sfc_number);

    int sfc_distribution[sfc_number];
    for (int i=0;i<sfc_number;i++) {
        sfc_distribution[i] = 0;
    }
    int nf_distribution[nf_number];
    for (int i=0;i<nf_number;i++) {
        nf_distribution[i] = 0;
    }
    //print distribution
    double sfc_result = 0;
    int *nfs_result = new int[CHAIN_LEN];
    for (int i=0;i<nf_number;i++) {
        nfs_result[i] = 0;
    }

    gettimeofday(&start2, NULL);

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
            for (int m=0;m<3;m++) {
                sfcs[j].nfs[m].core = sfc_distribution[j];
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
            double *nf_result = single_prediction(nf_distribution);
            //print nf_result
            //sfc special
            double result_tmp = myMin(nf_result[0], nf_result[1], nf_result[2]) + myMin(nf_result[3], nf_result[4], nf_result[5]) + myMin(nf_result[6], nf_result[7], nf_result[8]) + myMin(nf_result[9], nf_result[10], nf_result[11]) + myMin(nf_result[12], nf_result[13], nf_result[14]) + myMin(nf_result[15], nf_result[16], nf_result[17]);
            if (sfc_result < result_tmp) {
                //printf("sfc_result: %f, result_tmp: %f\n", sfc_result, result_tmp);
                //print nf_result
                sfc_result = result_tmp;
                for (int t=0;t<CHAIN_LEN;t++) {
                    nfs_result[t] = nf_distribution[t];
                }
            }
            multi_prediction(sfcs, server, nf_distribution, nf_result, sfc_result, nfs_result);
        }
    }

    gettimeofday(&end, NULL);
    cout << "sfc search1 run time is : " << ((end.tv_usec - start1.tv_usec) + (end.tv_sec - start1.tv_sec) * 1000000) << "us" << endl;
    cout << "sfc search2 run time is : " << ((end.tv_usec - start2.tv_usec) + (end.tv_sec - start2.tv_sec) * 1000000)  << "us" << endl;
    map<double, int*> myMap;
    myMap[sfc_result] = nfs_result;
    return myMap;
}

//greedy placement algorithm
map<double, int*> prediction_placement (int nfs[]) {
    timeval start1, start2, end, end2;
    gettimeofday(&start1, NULL);

    int server[2];
    server[0] = 11;
    server[1] = 11;
    int node_number = 2;
    int nf_number = CHAIN_LEN;
    int nf_choice = myPow(node_number, nf_number);
    int distribution[nf_number];
    for (int i=0;i<nf_number;i++) {
        distribution[i] = 0;
    }
    //print distribution
    double sfc_result = 0;
    int *nfs_result = new int[CHAIN_LEN];
    for (int i=0;i<nf_number;i++) {
        nfs_result[i] = 0;
    }

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
            //gettimeofday(&start2, NULL);
            double *nf_result = single_prediction(distribution);
            //gettimeofday(&end2, NULL);
            //cout << "single_prediction time: " << ((end2.tv_usec - start2.tv_usec) + (end2.tv_sec - start2.tv_sec) * 1000000)  << "us" << endl;
            //print distribution
            //print nf_result
            //sfc special
            double result_tmp = myMin(nf_result[0], nf_result[1], nf_result[2]) + myMin(nf_result[3], nf_result[4], nf_result[5]) + myMin(nf_result[6], nf_result[7], nf_result[8]) + myMin(nf_result[9], nf_result[10], nf_result[11]) + myMin(nf_result[12], nf_result[13], nf_result[14]) + myMin(nf_result[15], nf_result[16], nf_result[17]);
            if (sfc_result < result_tmp) {
                //printf("sfc_result: %f, result_tmp: %f\n", sfc_result, result_tmp);
                //print nf_result
                sfc_result = result_tmp;
                for (int k=0;k<nf_number;k++) {
                    nfs_result[k] = distribution[k];
                }
            }
        }
    }

    gettimeofday(&end, NULL);
    cout << "nf search1 run time is : " << ((end.tv_usec - start1.tv_usec) + (end.tv_sec - start1.tv_sec) * 1000000) << "us" << endl;
    cout << "nf search2 run time is : " << ((end.tv_usec - start2.tv_usec) + (end.tv_sec - start2.tv_sec) * 1000000)  << "us" << endl;
    map<double, int*> myMap;
    myMap[sfc_result] = nfs_result;
    return myMap;
}

int main() {
    _init_();

    int total_time_search = 0;
    long long total_time_placement = 0;
    int hit_number = 0;
    int miss_number = 0;
    double miss_loss = 0;
    timeval start, end;

    random_device rd;
    default_random_engine random(rd());

    for (int i=0;i<100;i++) {
        map<double, int*>::iterator a, b;
        //nfs.clear();
        for (int j=0;j<CHAIN_LEN;j++) {
            nfs[j] = random() % 4;
        }
        gettimeofday(&start, NULL);
        map<double, int*> result_search = prediction_search(nfs);
        gettimeofday(&end, NULL);
        total_time_search += ((end.tv_usec - start.tv_usec) + (end.tv_sec - start.tv_sec) * 1000000);

        gettimeofday(&start, NULL);
        map<double, int*> result_placement = prediction_placement(nfs);
        gettimeofday(&end, NULL);
        total_time_placement += (long long) ((end.tv_usec - start.tv_usec) + (end.tv_sec - start.tv_sec) * 1000000);

        a = result_search.begin();
        b = result_placement.begin();
        if (myCpr(a->second, b->second)) {
            hit_number += 1;
        } else {
            miss_number += 1;
            miss_loss += (b->first - a->first) / b->first;
            //printf("result_search: %f\n", a->first);
            //printf("result_placement: %f\n", b->first);
        }
    }
    
    cout << "sfc search run time is : " << total_time_search << "us" << endl;
    cout << "sfc placement run time is : " << total_time_placement << "us" << endl;
    cout << "hit ratio is : " << (double) hit_number / 100 << endl;
    cout << "miss loss is : " << (double) miss_loss / miss_number << endl;

    return 0;
}