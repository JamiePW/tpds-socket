#!/usr/bin/env python3
# -*- coding:utf-8 -*-

import random
import time
import copy

_nf_metrics_local = {}
_nf_metrics_local['nf_router'] = 94
_nf_metrics_local['aes_encrypt'] = 2000 #487
_nf_metrics_local['flow_tracker'] = 266
_nf_metrics_local['firewall'] = 194

_nf_metrics_remote = {}
_nf_metrics_remote['nf_router'] = 481
_nf_metrics_remote['aes_encrypt'] = 2000  #884
_nf_metrics_remote['flow_tracker'] = 617
_nf_metrics_remote['firewall'] = 576

_solo_performance_local = {}
_solo_performance_local['nf_router'] = 14791582
_solo_performance_local['aes_encrypt'] = 805120
_solo_performance_local['flow_tracker'] = 5350967
_solo_performance_local['firewall'] = 6783924

_solo_performance_remote = {}
_solo_performance_remote['nf_router'] = 5169204
_solo_performance_remote['aes_encrypt'] = 825056
_solo_performance_remote['flow_tracker'] = 3520384
_solo_performance_remote['firewall'] = 3829262

_nf_prediction_parameters_local = {}
_nf_prediction_parameters_local['nf_router'] = [7.264703, -3.220732, 0.280372]
_nf_prediction_parameters_local['aes_encrypt'] = [2.194409, -2.501355, 0.090523]
_nf_prediction_parameters_local['flow_tracker'] = [6.978917, 0.916245, 0.072959]
_nf_prediction_parameters_local['firewall'] = [5.529852, 0.298462, 0.208711]

_nf_prediction_parameters_remote = {}
_nf_prediction_parameters_remote['nf_router'] = [0.019525, 3.504447, 0.019525]
_nf_prediction_parameters_remote['aes_encrypt'] = [0.066159, -8.151804, 0.066159]
_nf_prediction_parameters_remote['flow_tracker'] = [0.038925, 16.161206, 0.038925]
_nf_prediction_parameters_remote['firewall'] = [0.068971, 8.947197, 0.068971]


# 组链方式，last=0代表一条新链
sfc_last_nf = {}
instance_to_core = {}
sfc_last_nf['1'] = '0'
sfc_last_nf['2'] = '1'
sfc_last_nf['3'] = '2'
sfc_last_nf['4'] = '0'
sfc_last_nf['5'] = '4'
sfc_last_nf['6'] = '5'
sfc_last_nf['7'] = '0'
sfc_last_nf['8'] = '7'
sfc_last_nf['9'] = '8'
sfc_last_nf['10'] = '0'
sfc_last_nf['11'] = '10'
sfc_last_nf['12'] = '11'
sfc_last_nf['13'] = '0'
sfc_last_nf['14'] = '13'
sfc_last_nf['15'] = '14'
sfc_last_nf['16'] = '0'
sfc_last_nf['17'] = '16'
sfc_last_nf['18'] = '17'
instance_to_core['0'] = '0'


# 单个NF预测模型
def prediction(nf, cache, memory, local1, local2):
    if local1 == True and local2 == True:
        predict_tmp = _nf_prediction_parameters_local[nf][0] * cache + _nf_prediction_parameters_local[nf][1] * memory + _nf_prediction_parameters_local[nf][2]
        return (1 - predict_tmp) * _solo_performance_local[nf]
    elif local1 == True and local2 == False:
        predict_tmp = _nf_prediction_parameters_remote[nf][0] * cache + _nf_prediction_parameters_remote[nf][1] * memory + _nf_prediction_parameters_remote[nf][2]
        return (1 - predict_tmp) * _solo_performance_remote[nf]
    elif local1 == False and local2 == True:
        predict_tmp = _nf_prediction_parameters_remote[nf][0] * cache + _nf_prediction_parameters_local[nf][1] * memory + _nf_prediction_parameters_local[nf][2]
        return (1 - predict_tmp) * _solo_performance_remote[nf]
    elif local1 == False and local2 == False:
        predict_tmp = _nf_prediction_parameters_local[nf][0] * cache + _nf_prediction_parameters_remote[nf][1] * memory + _nf_prediction_parameters_remote[nf][2]
        return (1 - predict_tmp) * _solo_performance_local[nf]


# 单次预测模型
def single_prediction(nf_distribution):

    nf_result = []
    local_nf = []
    remote_nf = []
    
    for s in range(len(nf_distribution)):
        instance_to_core[str(s + 1)] = str(nf_distribution[s])
        if nf_distribution[s] == 0:
            local_nf.append(str(s + 1))
        else:
            remote_nf.append(str(s + 1))
    for t in range(len(nf_distribution)):
        if instance_to_core[str(t + 1)] == '0' and instance_to_core[sfc_last_nf[str(t + 1)]] == '0':
            cache_local_tmp = 0
            memory_local_tmp = 0
            for nf in local_nf:
                if sfc_last_nf[nf] not in local_nf:
                    cache_local_tmp += 1 / _nf_metrics_local[nfs[int(nf) - 1]]
                    memory_local_tmp += 1 / _nf_metrics_local[nfs[int(nf) - 1]]
            for nf in remote_nf:
                if sfc_last_nf[nf] not in remote_nf:
                    memory_local_tmp += 1 / _nf_metrics_local[nfs[int(nf) - 1]]
            nf_result.append(prediction(nfs[t], cache_local_tmp, memory_local_tmp, True, True))
        elif instance_to_core[str(t + 1)] == '0' and instance_to_core[sfc_last_nf[str(t + 1)]] == '1':
            cache_local_tmp = 1
            memory_local_tmp = 0
            for nf in local_nf:
                if sfc_last_nf[nf] not in local_nf:
                    memory_local_tmp += 1 / _nf_metrics_local[nfs[int(nf) - 1]]
            for nf in remote_nf:
                if sfc_last_nf[nf] not in remote_nf:
                    memory_local_tmp += 1 / _nf_metrics_local[nfs[int(nf) - 1]]
            nf_result.append(prediction(nfs[t], cache_local_tmp, memory_local_tmp, False, True))
        elif instance_to_core[str(t + 1)] == '1' and instance_to_core[sfc_last_nf[str(t + 1)]] == '1':
            cache_remote_tmp = 0
            memory_remote_tmp = 0
            # for nf in local_nf:
            #   if sfc_last_nf[nf] not in local_nf:
            #       memory_remote_tmp += 1 / _nf_metrics_remote[nfs[int(nf) - 1]]
            for nf in remote_nf:
                if sfc_last_nf[nf] not in remote_nf:
                    cache_remote_tmp += 1 / _nf_metrics_remote[nfs[int(nf) - 1]]
                    memory_remote_tmp += 1 / _nf_metrics_remote[nfs[int(nf) - 1]]
            nf_result.append(prediction(nfs[t], cache_remote_tmp, memory_remote_tmp, False, False))
        elif instance_to_core[str(t + 1)] == '1' and instance_to_core[sfc_last_nf[str(t + 1)]] == '0':
            cache_remote_tmp = 1
            memory_remote_tmp = 0
            # for nf in local_nf:
            #   if sfc_last_nf[nf] not in local_nf:
            #       memory_remote_tmp += 1 / _nf_metrics_remote[nfs[int(nf) - 1]]
            for nf in remote_nf:
                if sfc_last_nf[nf] not in remote_nf:
                    memory_remote_tmp += 1 / _nf_metrics_remote[nfs[int(nf) - 1]]
            nf_result.append(prediction(nfs[t], cache_remote_tmp, memory_remote_tmp, True, False))
    return nf_result


# 递归搜索过程
def multi_prediction(sfcs, server, nf_distribution, nf_result, sfc_result, nfs_result):
    flag = 0
    i = 0
    node0_core = 0
    # print(nf_result)

    for sfc in sfcs:
        sfc_min = 1000000000
        # index_min = 0
        for nf in sfc['nfs']:
            if nf['core'] == 0:
                node0_core += 1
            if sfc_min > nf_result[i]:
                # sfc['bottleneck'] = index_min
                # index_min += 1
                sfc['bottleneck'] = i
                sfc_min = nf_result[i]
            i += 1

    distribution_tmp = [0] * i
    nf_result_tmp = [0] * i
    sfc_result_tmp = 0
    for sfc in sfcs:
        if sfc['node_id'] != 0 and sfc['bottleneck'] >= sfc['current'] and sfc['bottleneck'] < sfc['last'] and (server['node_0'] - node0_core) > (sfc['bottleneck'] - sfc['current']):
            flag = 1
            distribution = []
            for iterm in nf_distribution:
                distribution.append(iterm)
            for j in range(sfc['bottleneck'] - sfc['current'] + 1):
                distribution[sfc['current'] + j] = 0
            # print(distribution)
            nf_result = single_prediction(distribution)
            # result_tmp = min(nf_result[0], nf_result[1], nf_result[2]) + min(nf_result[3], nf_result[4], nf_result[5]) + min(nf_result[6], nf_result[7], nf_result[8])
            result_tmp = min(nf_result[0], nf_result[1], nf_result[2]) + min(nf_result[3], nf_result[4], nf_result[5]) + min(nf_result[6], nf_result[7], nf_result[8]) + min(nf_result[9], nf_result[10], nf_result[11]) + min(nf_result[12], nf_result[13], nf_result[14]) + min(nf_result[15], nf_result[16], nf_result[17])
            if sfc_result_tmp < result_tmp:
                sfc_result_tmp = result_tmp
                for k in range(len(distribution)):
                    distribution_tmp[k] = distribution[k]
                    nf_result_tmp[k] = nf_result[k]
            if sfc_result < result_tmp:
                sfc_result = result_tmp
                for t in range(len(distribution)):
                    nfs_result[t] = distribution[t]
                    

    if flag == 1:
        t = 0
        for sfc in sfcs:
            for nf in sfc['nfs']:
                nf['core'] = distribution_tmp[t]
                if distribution[t] != sfc['node_id']:
                    sfc['current'] = sfc['bottleneck'] + 1
                t += 1
        # print(distribution_tmp)
        multi_prediction(sfcs, server, distribution_tmp, nf_result_tmp, sfc_result, nfs_result)
    else:
        return


# 启发示搜索方法
def prediction_search(nfs):
    # time_start1 = time.time()
    # 指定组链方式
    sfcs = []
    for sfc_id in range(6):
        sfc = {}
        sfc['id'] = sfc_id
        sfc['node_id'] = 0
        sfc['bottleneck'] = 0
        sfc['current'] = 0
        sfc['last'] = 0
        sfc['nfs'] = []
        sfcs.append(sfc)
    for nf in nfs[0:3]:
        nf_tmp = {}
        nf_tmp['nf'] = nf
        nf_tmp['core'] = 0
        sfcs[0]['current'] = 0
        sfcs[0]['last'] = 2
        sfcs[0]['nfs'].append(nf_tmp)
    for nf in nfs[3:6]:
        nf_tmp = {}
        nf_tmp['nf'] = nf
        nf_tmp['core'] = 0
        sfcs[1]['current'] = 3
        sfcs[1]['last'] = 5
        sfcs[1]['nfs'].append(nf_tmp)
    for nf in nfs[6:9]:
        nf_tmp = {}
        nf_tmp['nf'] = nf
        nf_tmp['core'] = 0
        sfcs[2]['current'] = 6
        sfcs[2]['last'] = 8
        sfcs[2]['nfs'].append(nf_tmp)
    for nf in nfs[9:12]:
        nf_tmp = {}
        nf_tmp['nf'] = nf
        nf_tmp['core'] = 0
        sfcs[3]['current'] = 9
        sfcs[3]['last'] = 11
        sfcs[3]['nfs'].append(nf_tmp)
    for nf in nfs[12:15]:
        nf_tmp = {}
        nf_tmp['nf'] = nf
        nf_tmp['core'] = 0
        sfcs[4]['current'] = 12
        sfcs[4]['last'] = 14
        sfcs[4]['nfs'].append(nf_tmp)
    for nf in nfs[15:18]:
        nf_tmp = {}
        nf_tmp['nf'] = nf
        nf_tmp['core'] = 0
        sfcs[5]['current'] = 15
        sfcs[5]['last'] = 17
        sfcs[5]['nfs'].append(nf_tmp)
    
    # server设置指定
    server = {}
    server["node_0"] = 11
    server['node_1'] = 11
    node_number = 2
        
    nf_number = len(nfs)
    sfc_number = len(sfcs)
    sfc_choice = node_number ** sfc_number
    sfc_distribution = [0] * sfc_number
    nf_distribution = [0] * nf_number
    # print(distribution)
    sfc_result = 0
    nfs_result = [0] * nf_number
    # time_start2 = time.time()
    for i in range(sfc_choice):
        k = 0
        for j in range(sfc_number):
            sfc_distribution[j] = i % node_number
            i = (int)(i / node_number)
            if sfc_distribution[j] != 0:
                sfc_distribution[j] = 1
            sfcs[j]['node_id'] = sfc_distribution[j]
            for nf in sfcs[j]['nfs']:
                nf['core'] = sfc_distribution[j]
                nf_distribution[k] = sfc_distribution[j]
                k += 1
                sfcs[0]['current'] = 0
                sfcs[1]['current'] = 3
                sfcs[2]['current'] = 6
                sfcs[3]['current'] = 9
                sfcs[4]['current'] = 12
                sfcs[5]['current'] = 15
        if (nf_number - sum(nf_distribution)) < server['node_0'] and sum(nf_distribution) < server['node_1']:
            # print(nf_distribution)
            nf_result = single_prediction(nf_distribution)
            # print(nf_result)
            # sfc special
            # result_tmp = min(nf_result[0], nf_result[1], nf_result[2]) + min(nf_result[3], nf_result[4], nf_result[5]) + min(nf_result[6], nf_result[7], nf_result[8])
            result_tmp = min(nf_result[0], nf_result[1], nf_result[2]) + min(nf_result[3], nf_result[4], nf_result[5]) + min(nf_result[6], nf_result[7], nf_result[8]) + min(nf_result[9], nf_result[10], nf_result[11]) + min(nf_result[12], nf_result[13], nf_result[14]) + min(nf_result[15], nf_result[16], nf_result[17])
            if sfc_result < result_tmp:
                # print("sfc_result: " + str(sfc_result) + ", result_tmp: " + str(result_tmp))
                # print(nf_result)
                sfc_result = result_tmp
                for t in range(len(nf_distribution)):
                    nfs_result[t] = nf_distribution[t]
            multi_prediction(sfcs, server, nf_distribution, nf_result, sfc_result, nfs_result)

    # time_end = time.time()
    # print("sfc search1 run time is : " + str(time_end - time_start1))
    # print("sfc search2 run time is : " + str(time_end - time_start2))
    return (sfc_result, nfs_result)


# 穷举搜索方法
def prediction_placement(nfs):
    # time_start1 = time.time()
    server = {}
    server["node_0"] = 11
    server['node_1'] = 11
    node_number = 2
    nf_number = len(nfs)
    nf_choice = node_number ** nf_number
    distribution = [0] * nf_number
    # print(distribution)
    sfc_result = 0
    nfs_result = [0] * nf_number
    # time_start2 = time.time()
    for i in range(nf_choice):
        # print(i)
        for j in range(nf_number):
            distribution[j] = i % node_number
            i = (int)(i / node_number)
            if distribution[j] != 0:
                distribution[j] = 1
        if (nf_number - sum(distribution)) < server['node_0'] and sum(distribution) < server['node_1']:
            # print(distribution)
            nf_result = single_prediction(distribution)
            # print(distribution)
            # print(nf_result)
            # sfc special
            # result_tmp = min(nf_result[0], nf_result[1], nf_result[2]) + min(nf_result[3], nf_result[4], nf_result[5]) + min(nf_result[6], nf_result[7], nf_result[8])
            result_tmp = min(nf_result[0], nf_result[1], nf_result[2]) + min(nf_result[3], nf_result[4], nf_result[5]) + min(nf_result[6], nf_result[7], nf_result[8]) + min(nf_result[9], nf_result[10], nf_result[11]) + min(nf_result[12], nf_result[13], nf_result[14]) + min(nf_result[15], nf_result[16], nf_result[17])
            if sfc_result < result_tmp:
                # print("sfc_result: " + str(sfc_result) + ", result_tmp: " + str(result_tmp))
                # print(nf_result)
                sfc_result = result_tmp
                
                for k in range(len(distribution)):
                    nfs_result[k] = distribution[k]

    # time_end = time.time()
    # print("nf search1 run time is: " + str(time_end - time_start1))
    # print("nf search2 run time is: " + str(time_end - time_start2))
    return (sfc_result, nfs_result)


if __name__ == '__main__':
    nf_set = ['nf_router', 'firewall', 'flow_tracker', 'aes_encrypt']
    total_time_search = 0
    total_time_placement = 0
    hit_number = 0
    miss_number = 0
    miss_loss = 0
    for i in range(100):
        nfs = []
        for j in range(18):
            nfs.append(nf_set[random.randint(0,3)])
        time_start = time.time()
        (result_search, distribution_search) = prediction_search(nfs)
        time_end = time.time()
        total_time_search += time_end - time_start
        time_start = time.time()
        (result_placement, distribution_placement) = prediction_placement(nfs)
        time_end = time.time()
        total_time_placement += time_end - time_start
        if distribution_search == distribution_placement:
            hit_number += 1
        else:
            miss_number += 1
            miss_loss += float(result_placement - result_search)/result_placement
            # print(result_search)
            # print(result_placement)
    print("sfc search run time is : " + str(total_time_search))
    print("sfc placement run time is : " + str(total_time_placement))
    print("hit ratio is : " + str(float(hit_number)/100))
    print("miss loss is : " + str(miss_loss/miss_number))