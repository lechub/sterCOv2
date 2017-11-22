/*
 * Impulsator.h
 *
 *  Created on: 30.09.2016
 *      Author: lechu
 */

#ifndef IMPULSATOR_H_
#define IMPULSATOR_H_

#include "Button.h"


class Impulsator {
public:
	typedef enum{NONE, LEFT, RIGHT, PUSH, RELEASE} Move;


private:
	Gpio * leftGpio = nullptr;
	Gpio * rightGpio = nullptr;
	Button * pushBtn = nullptr;

	volatile Move	move = NONE;
	volatile bool 	semafor = false;
	//	bool	pushState = false;


	/**
	 * sprawdzenie czy byl obrot - sprawdzac czesto, zeby nie przeoczyc
	 */
	Move	getTurnNow(){
		static bool	lState = false;	//leftGpio->getInputValue();
		bool left = leftGpio->getInputValue();
		bool right = rightGpio->getInputValue(); //pobranie stanu
		if (lState != left){	// zmiana stanu
			lState = left;		// zapamietanie poprzedniego
			if (right == left) return LEFT;	// Jesli lewy przeskoczyl, a prawy juz byl w tym stanie, to w lewo
			else return RIGHT;				// Jesli lewy przeskoczyl, a prawy jeszcze nie, to obrot w prawo
		}
		return NONE;
	}


public:

	Impulsator(Gpio * left, Gpio * right, Button * push){
		setup(left, right, push);
	}

	Impulsator(){};

	inline bool isPressed(){ return pushBtn->isPressed();	}

	inline void setup(Gpio * left, Gpio * right, Button * push){
		leftGpio = left;
		rightGpio = right;
		pushBtn = push;
	}

	Move getMove(){
		semafor = true;
		Move result = move;
		move = NONE;
		semafor = false;
		return result;
	}

	void co10ms(){
		if (semafor) return;
		static bool lastPush = false;
		Move turn = getTurnNow();
		if (turn != NONE){		// czy jest obrot?
			move = turn;		// nowy obrot
			return;
		}
		if (isPressed() != lastPush){	// czy nacisniety?
			lastPush = !lastPush;
			move = lastPush ? PUSH : RELEASE;	// nacisniety czy puszczony
			return;
		}
		return;
	}


};

#endif /* IMPULSATOR_H_ */
