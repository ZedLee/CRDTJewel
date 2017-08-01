# CRDTJewel
A multiplayers jewel implementation via operation-based CRDT, reliable commucation provided by zero message queue

## Introduction
Operation-based CRDTs only send its update operation to other replicas and expect them to repeat the update operation. This also an advantage compare to Convergent Replicated Data Type, because the full state contains more redundancy and use more bandwidth in transmission. See figure 1 below, we can tell downstream replicas eventually will execute the operations F() and G() in any sequence, but the result of synchronization may not be same for all replicas because of different sequence of execution, therefore, the working conditions of operation-based replication would be slightly complicated than state-based replication. In this model where the updates are broadcast to all replicas, an object for which all concurrent updates are commutative is called CmRDT (commutative replicated data type).<br>
![Alt text](https://github.com/ZedLee/CRDTJewel/blob/master/img/WechatIMG2831.jpeg)

## Library dependencies
zeroMQ: http://zeromq.org<br>
log4z: https://github.com/zsummer/log4z<br>
json: https://github.com/nlohmann/json<br>

### Reference

### Contact me 
Email: aaa304482692@gmail.com
Alternative Email: lizhuo1993@foxmail.com