#include "TicTacToe.h"
#include "../tools/Logger.h"

const int AI_PLAYER = 1;
const int HUMAN_PLAYER = -1;

TicTacToe::TicTacToe() {

}

TicTacToe::~TicTacToe() {
}

//
// make an X or an O
//
Bit* TicTacToe::PieceForPlayer(const int playerNumber) {
	// depending on playerNumber load the "x.png" or the "o.png" graphic
	Bit *bit = new Bit();
	// should possibly be cached from player class?
	bit->LoadTextureFromFile(playerNumber == 1 ? "x.png" : "o.png");
	bit->setOwner(getPlayerAt(playerNumber));
	return bit;
}

void TicTacToe::setUpBoard() {
	setNumberOfPlayers(2);
    // this allows us to draw the board correctly

    // setup the board
	for (int i = 0; i < 9; i++) {
		_grid[i].initHolder(ImVec2((i/3)*100 + 100, (i%3)*100 + 100), "square.png", i%3, i/3);
	}

	/*
	// if we have an AI set it up
	if (gameHasAI()) {
		setAIPlayer(_gameOptions.AIPlayer);
	}
	*/
	// setup up turns etc.
	startGame();
}

//
// scan for mouse is temporarily in the actual game class
// this will be moved to a higher up class when the squares have a heirarchy
// we want the event loop to be elsewhere and calling this class, not called by this class
// but this is fine for tic-tac-toe
//
void TicTacToe::scanForMouse() {
	if (gameHasAI() && getCurrentPlayer() && getCurrentPlayer()->isAIPlayer()) {
		updateAI();
		return;
	}

	ImVec2 mousePos = ImGui::GetMousePos();
	mousePos.x -= ImGui::GetWindowPos().x;
	mousePos.y -= ImGui::GetWindowPos().y;

	for (int i = 0; i < 9; i++) {
		if (_grid[i].isMouseOver(mousePos)) {
			if (ImGui::IsMouseClicked(0)) {
				if (!_gameOver && actionForEmptyHolder(_grid[i])) {
					Logger::getInstance().log(std::to_string(getCurrentPlayer()->playerNumber()) + " played at " + std::to_string(i));
					endTurn();
				}
			} else {
				_grid[i].setHighlighted(true);
			}
		} else {
			_grid[i].setHighlighted(false);
		}
	}
}

//
// draw the board and then the pieces
// this will also go somewhere else when the heirarchy is set up
//
void TicTacToe::drawFrame() {
	scanForMouse();

	for (int i = 0; i < 9; i++) {
		_grid[i].paintSprite();
		if (_grid[i].bit()) {
			_grid[i].bit()->paintSprite();
		}
	}
}

// about the only thing we need to actually fill out for tic-tac-toe
bool TicTacToe::actionForEmptyHolder(BitHolder& holder) {
	// if there is already a piece here, then quit.
	if (holder.bit()) {
		return false;
	}

	Bit *bit = PieceForPlayer(getCurrentPlayer()->playerNumber());
	if (bit) {
		bit->setPosition(holder.getPosition());
		holder.setBit(bit);
		return true;
	}

	return false;
}

bool TicTacToe::canBitMoveFrom(Bit *bit, BitHolder *src) {
	// you can't move anything in tic tac toe
	return false;
}

bool TicTacToe::canBitMoveFromTo(Bit* bit, BitHolder*src, BitHolder*dst) {
	// you can't move anything in tic tac toe
	return false;
}

void TicTacToe::stopGame() {
	// im not huge on the "destroyBit" stuff. if I have time, consider implementing custom destructor that does this
	// so we can use delete.
	for (int i = 0; i < 9; i++) {
		_grid[i].destroyBit();
	}
}

// helper function for the winner check
Player* TicTacToe::ownerAt(int index ) const {
	Bit* b = _grid[index].bit();
	return b ? b->getOwner() : nullptr;
}


Player* TicTacToe::checkForWinner() {
	// there are only 8 possible winning combinations in TTT, so just check those.
	for (int* arr : _winStates) {
		std::cout << arr[0] << " " << arr[1] << " " << arr[2] << std::endl;
		// if 3 in a row are same owner, we have a winner
		// bit can be null, so account for that :)
		Player* a = _grid[arr[0]].bit() ? _grid[arr[0]].bit()->getOwner() : nullptr;
		Player* b = _grid[arr[1]].bit() ? _grid[arr[1]].bit()->getOwner() : nullptr;
		Player* c = _grid[arr[2]].bit() ? _grid[arr[2]].bit()->getOwner() : nullptr;
		// there can be a winner iff a != null
		if (a && a == b && b == c) {
			Logger::getInstance().log("Found possible winner: " + a->playerNumber());
			_winner = a;
			_gameOver = true;
			return a;
		}
	}

	// check to see if either player has won
	return nullptr;
}

bool TicTacToe::checkForDraw() {
	// check to see if the board is full
	for (Square& i : _grid) {
		Bit* b = i.bit();
		if (!b || !b->getOwner()) {
			return false;
		}
	}

	_gameOver = true;
	return true;
}

//
// state strings
//
std::string TicTacToe::initialStateString() {
	return "000000000";
}

//
// this still needs to be tied into imguis init and shutdown
// we will read the state string and store it in each turn object
//
std::string TicTacToe::stateString() const {
	std::string s;
	for (int i = 0; i < 9; i++) {
		Bit *bit = _grid[i].bit();
		if (bit) {
			s += std::to_string(bit->getOwner()->playerNumber() + 1);
		} else {
			s += "0";
		}
	}

	return s;
}

//
// this still needs to be tied into imguis init and shutdown
// when the program starts it will load the current game from the imgui ini file and set the game state to the last saved state
//
void TicTacToe::setStateString(const std::string &s) {
	for (int i = 0; i < 9; i++) {
		int playerNumber = s[i] - '0';
		if (playerNumber) {
			_grid[i].setBit(PieceForPlayer(playerNumber - 1));
		} else {
			_grid[i].setBit(nullptr);
		}
	}
}


//
// this is the function that will be called by the AI
//
void TicTacToe::updateAI() {

}

//
// AI class
// this is a small class that just has a bunch of ints in it to allow us to recursively call minimax
//

TicTacToeAI* TicTacToe::clone() {
	TicTacToeAI* newGame = new TicTacToeAI();
	return newGame;
}

//
// helper function for the winner check
//
int TicTacToeAI::ownerAt(int index ) const {
	return 0;
}

int TicTacToeAI::AICheckForWinner() {
	return -1;
}

//
// helper function for a draw
//
bool TicTacToeAI::isBoardFull() const {
	return false;	
}

//
// Returns: positive value if AI wins, negative if human player wins, 0 for draw or undecided
//
int TicTacToeAI::evaluateBoard() {
	// Check for winner
	return 0;
}

//
// player is the current player's number (AI or human)
//
int TicTacToeAI::negamax(TicTacToeAI* state, int depth, int playerColor) {
	return 0;
}

//
// evaluate board for minimax needs to return 
// actual winner
//
int TicTacToeAI::evaluateBoardMinimax() {
	return 0; // No winner yet or draw
}

//
// player is the current player's number (AI or human)
//
int TicTacToeAI::minimax(TicTacToeAI* state, int depth, bool isMaximizingPlayer) {
	return 0;
}
