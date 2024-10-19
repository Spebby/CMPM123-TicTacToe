#pragma once

#include <iostream>
#include <vector>
#include <string>

#include "Player.h"
#include "Turn.h"
#include "Bit.h"
#include "BitHolder.h"

class GameTable;

class Game {
public:
	Game();
	~Game();

	void		startGame();
	virtual		void	setUpBoard() = 0;
	virtual		void	drawFrame() = 0;

	// end the current game turn
	void	endTurn();
	
	// Should return true if it is legal for the given bit to be moved from its current holder.
	// Default implementation always returns true. 
	virtual		bool	canBitMoveFrom(Bit*bit, BitHolder *src) = 0;

	// /** Should return true if it is legal for the given Bit to move from src to dst.
	// Default implementation always returns true.
	virtual		bool	canBitMoveFromTo(Bit* bit, BitHolder*src, BitHolder*dst) = 0;

	// can we do something with an empty holder?  do it here
	virtual		bool	actionForEmptyHolder(BitHolder& holder);

	// Should handle any side effects of a Bit's movement, such as captures or scoring.
	// Does not need to do the actual movement! That's already happened.
	// It should end by calling endTurn, if the player's turn is over.
	// Default implementation just calls endTurn.
	virtual		void	bitMovedFromTo(Bit *bit, BitHolder *src, BitHolder *dst);

	// Called instead of the above if a Bit is simply clicked, not dragged.
	// Should return NO if the click is illegal (i.e. clicking an empty draw pile in a card game.)
	// Default implementation always returns true.
	virtual		bool	clickedBit(Bit *bit);

	// Called on mouse-down/touch of an *empty* BitHolder. Should return a Bit if
	// it's OK to place a new Bit there; else nil.
	virtual		Bit*	bitToPlaceInHolder(BitHolder *holder);

	virtual		Player* checkForWinner() = 0;
	virtual     bool 	checkForDraw() = 0;
	virtual		bool	animateAndPlaceBitFromTo(Bit *bit, BitHolder*src, BitHolder*dst);

	virtual		void	stopGame() = 0;
    virtual     bool    gameHasAI();
    virtual     void    updateAI();

	virtual		std::string	initialStateString() = 0;
	virtual		std::string stateString() const = 0;
	virtual		void setStateString(const std::string &s) = 0;
    
	void		setNumberOfPlayers(unsigned int playerCount);
	void		setAIPlayer(unsigned int playerNumber);

	const unsigned int			getCurrentTurnNo() { return _currentTurnNo; };
	const int					getScore() { return _score; };
	void						setScore(int score) { _score = score; };
	Player*						getCurrentPlayer() { if (_players.size() == 0) return nullptr; else return _players.at(_currentTurnNo % _players.size()); };
	Player*						getPlayerAt(unsigned int playerNumber) { return _players.at(playerNumber); };

	GameTable				*_table;
	Player					*_winner;

	std::vector<Player*>	_players;
	std::vector<Turn*>		_turns;

	unsigned int			_currentTurnNo;
	int						_score;
	std::string				_lastMove;

	int						_gameNumber;
    bool                	_AIPlaying;

	bool					_gameOver;
};

