#include "Game.h"
#include "Bit.h"
#include "BitHolder.h"
#include "Turn.h"
#include "../Application.h"

Game::Game() {
	_score = 0;
	_table = nullptr;
	_winner = nullptr;
	_currentTurnNo = 0;
	_lastMove = "";
	_gameNumber = -1;
	_AIPlaying = false;
	_gameOver  = false;
}


Game::~Game() {
	for (auto & _turn : _turns) {
		delete _turn;
	}
	_turns.clear();
	for (auto & _player : _players) {
		delete _player;
	}
	_players.clear();

	_score = 0;
	_table = nullptr;
	_winner = nullptr;
	_currentTurnNo = 0;
	_lastMove = "";
}

void Game::setNumberOfPlayers(unsigned int n) {
	_players.clear();
	for (unsigned int i = 1; i <= n; i++) {
		Player *player = Player::initWithGame(this);
		// player->setName( std::format( "Player-{}", i ) );
		player->setName( "Player" );
		player->setPlayerNumber(i-1); // player numbers are zero-based
		_players.push_back(player);
	}
	_winner = nullptr;
	_gameOver = false;
	_gameNumber = 0;

	Turn *turn = Turn::initStartOfGame(this);
	_turns.clear();
	_turns.push_back(turn);
}

void Game::setAIPlayer(unsigned int playerNumber) {
	_players.at(playerNumber)->setAIPlayer(true);
}

void Game::startGame() {
	std::string startState = stateString();
	Turn *turn = _turns.at(0);
	turn->_boardState = startState;
	turn->_gameNumber = _gameNumber;
	_currentTurnNo = 0;
}

void Game::endTurn() {
	_currentTurnNo++;
	std::string startState = stateString();
	Turn *turn = new Turn;
	turn->_boardState = stateString();
	turn->_date = (int)_currentTurnNo;
	turn->_score = _score;
	turn->_gameNumber = _gameNumber;
	_turns.push_back(turn);
	ClassGame::EndOfTurn();
}

void Game::bitMovedFromTo(Bit *bit, BitHolder *src, BitHolder *dst) {
	endTurn();
}

Bit* Game::bitToPlaceInHolder(BitHolder *holder) {
	return nullptr;
}

bool Game::actionForEmptyHolder(BitHolder& holder) {
	return false;
}

bool Game::clickedBit(Bit *bit) {
	return true;
}

bool Game::animateAndPlaceBitFromTo(Bit *bit, BitHolder*src, BitHolder*dst) {
	return false;
}

bool Game::gameHasAI() {
    return false;
}

void Game::updateAI() {

}