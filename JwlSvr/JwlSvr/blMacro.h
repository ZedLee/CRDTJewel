//
//  blMacro.h
//  short
//
//  Created by 李卓 on 19/07/2017.
//  Copyright © 2017 李卓. All rights reserved.
//

#ifndef blMacro_h
#define blMacro_h

//message field
#define REQUEST_TYPE            "request_type"
#define REQUEST_ACKNOWLEDGE     "acknowledge"
#define USER_SOCKET_UUID        "socket_id"
#define USER_NAME               "user_name"
#define USER_PASSWORD           "user_pwd"
#define GAME_UUID               "game_id"
#define GAME_MATCH_STATE        "game_match_state"
#define OTHER_PLAYER_ID         "player2_id"
#define MASSAGE_CONTENT         "msg_content"

//in game message
#define GAME_MOVING_POINT       "moving_point"
#define GAME_MOVING_DIRECTION   "moving_direction"
#define GAME_VECTOR_CLOCK       "vector_clock"


//acknowlegde info
#define ACKNOWLEDGE_SUCESSED    1
#define ACKNOWLEDGE_FAILED      -1

//request type
#define USER_CONNECT            25000
#define USER_FIND_GAME_MATCH    25001
#define USER_LOGOUT             25002
#define USER_GAME_SETUP         25003
#define BOARDCAST_MSG_INGAME    25004

//game match state
#define GAME_MATCH_SUCESSED     25100
#define GAME_MATCH_WAIT         25101
#define GAME_SUBSCRIBER_WAITING 25102
#define GAME_SUBSCRIBER_READY   25103

#endif /* blMacro_h */
