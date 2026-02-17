//==================================================================
//
//	éûä‘ä«óù [time.h]
//
//
// Author :Å@KURUMISAWA		Date	: 2024:11/28
//==================================================================
#ifndef _TIME_H
#define _TIME_H
#include <Windows.h>
#include <cstdint> 

class GameFrameTime {
public:
   
    static void SetElapsedTime(float time);
    static float GetElapsedTime();

private:
    static float deltaTime;
};


#endif//_TIME_H
