# CRDTJewel
A multiplayers jewel implementation via operation-based CRDT, reliable communication provided by zero message queue

## Introduction
Operation-based CRDTs only send its update operation to other replicas and expect them to repeat the update operation. This also an advantage compare to Convergent Replicated Data Type, because the full state contains more redundancy and use more bandwidth in transmission. See figure below, we can tell downstream replicas eventually will execute the operations F() and G() in any sequence, but the result of synchronization may not be same for all replicas because of different sequence of execution. Therefore, the working conditions of operation-based replication would be slightly complicated than state-based replication, And I using ZeroMQ to achieve riliable communication between the replicas.<br>
<img src="https://github.com/ZedLee/CRDTJewel/blob/master/img/WechatIMG2831.jpeg" width="480" height="200"></img><br>

#### JwlSvr
Server program, only for setting up the game for players and provides pub/sub communicational architecture for all replicas(synchronize players' operations) .

##### Environmental Requirement
* G++4.9.* or higher
* libsodium
* libuuid
* System: Linux or Mac OS (compilation pass)

#### JwlAlg
Console version client (cocos2dx version still needed to debug), contains a CRDTs game borad and a simple console user interface.

##### Environmental Requirement
* Clang 8.* or higher
* libsodium
* System: Mac OS (compilation pass)

## Library dependencies
* zeroMQ: http://zeromq.org<br>
* log4z: https://github.com/zsummer/log4z<br>
* json: https://github.com/nlohmann/json<br>

### Reference
* [1] Conflict-free replicated data types, Shapiro, Marc; Preguiça, Nuno; Baquero, Carlos; Zawirski, Marek (2011-10-10), SSS'11 Proceedings of the 13th international conference on Stabilization, safety, and the security of distributed systems. Springer-Verlag Berlin, Heidelberg: 386–400 <br>
* [2] A service framework for operation-based CRDTs, Martin Krasser, 10/19/2016<br>

### Contact me 
* Email: aaa304482692@gmail.com
* Alternative Email: lizhuo1993@foxmail.com