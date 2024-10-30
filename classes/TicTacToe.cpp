#include "TicTacToe.h"
#include "../tools/Logger.h"

int activePlayer(int active) {
	return active == 1 ? 2 : 1;
}

TicTacToe::TicTacToe() {

}

TicTacToe::~TicTacToe() {
}

// make an X or an O
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
		_grid[i].initHolder(ImVec2((i%3)*100 + 100, (i/3)*100 + 100), "square.png", i%3, i/3);
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

// scan for mouse is temporarily in the actual game class
// this will be moved to a higher up class when the squares have a heirarchy
// we want the event loop to be elsewhere and calling this class, not called by this class
// but this is fine for tic-tac-toe
void TicTacToe::scanForMouse() {
	if (!_gameOver && getCurrentPlayer()->isAIPlayer()) {
		updateAI();
		endTurn();
		return;
	}

	ImVec2 mousePos = ImGui::GetMousePos();
	mousePos.x -= ImGui::GetWindowPos().x;
	mousePos.y -= ImGui::GetWindowPos().y;

	for (int i = 0; i < 9; i++) {
		if (_grid[i].isMouseOver(mousePos)) {
			if (ImGui::IsMouseClicked(0)) {
				if (!_gameOver && actionForEmptyHolder(_grid[i])) {
					Logger::getInstance().log(std::to_string(activePlayer(getCurrentPlayer()->playerNumber())) + " played at " + std::to_string(i));
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
		// if 3 in a row are same owner, we have a winner
		// bit can be null, so account for that :)
		Player* a = _grid[arr[0]].bit() ? _grid[arr[0]].bit()->getOwner() : nullptr;
		Player* b = _grid[arr[1]].bit() ? _grid[arr[1]].bit()->getOwner() : nullptr;
		Player* c = _grid[arr[2]].bit() ? _grid[arr[2]].bit()->getOwner() : nullptr;
		// there can be a winner iff a != null
		if (a && a == b && b == c) {
			Logger::getInstance().log("Found possible winner: " + std::to_string(a->playerNumber()));
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

// state strings
std::string TicTacToe::initialStateString() {
	return "000000000";
}

// this still needs to be tied into imguis init and shutdown
// we will read the state string and store it in each turn object
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

// this still needs to be tied into imguis init and shutdown
// when the program starts it will load the current game from the imgui ini file and set the game state to the last saved state
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

// this is the function that will be called by the AI
void TicTacToe::updateAI() {
	// run negamax on all avalible positions.
	int bestVal = -1000;
	Square* bestMove = nullptr;

	for (int i = 0; i < 9; i++) {
		if (!_grid[i].bit()) {
			TicTacToeAI* newState = this->clone();
			newState->_grid[i] = activePlayer(getCurrentPlayer()->playerNumber());
			// start negamax with response from opponent.
			int moveVal = -newState->negamax(newState, 0, -1000, 1000, activePlayer(getCurrentPlayer()->playerNumber()) == 2 ? 1 : 2);
			delete newState;

			Logger::getInstance().log(std::to_string(i) + " eval is " + std::to_string(moveVal));

			if (moveVal > bestVal) {
				bestMove = &_grid[i];
				bestVal = moveVal;
			}
		}
	}

	//Logger::getInstance().log("AI took this many steps: " + std::to_string(counter));
	if (bestMove) {
		for (int i = 0; i < 9; i++) {
			if (&_grid[i] == bestMove) {
				Logger::getInstance().log(std::to_string(activePlayer(getCurrentPlayer()->playerNumber())) + " played at " + std::to_string(i));
			}
		}
		actionForEmptyHolder(*bestMove);
	}
}

// AI class
// this is a small class that just has a bunch of ints in it to allow us to recursively call minimax
TicTacToeAI* TicTacToe::clone() {
	TicTacToeAI* newGame = new TicTacToeAI();
	std::string gamestate = stateString();
	for (int i = 0; i < 9; i++) {
		int pNum = gamestate[i] - '0';
		newGame->_grid[i] = pNum;
		newGame->_depthSearches = 0;
	}

	return newGame;
}

// helper function for the winner check
int TicTacToeAI::ownerAt(int index) const {
	return _grid[index];
}

int TicTacToeAI::AICheckForWinner() {
	static const int winStates[8][3] = {
        {0, 1, 2},
        {3, 4, 5},
        {6, 7, 8},
        {0, 3, 6},
        {1, 4, 7},
        {2, 5, 8},
        {0, 4, 8},
        {2, 4, 6}
    };

	for (int i = 0; i < 8; i++) {
		const int* arr = winStates[i];
		int a = _grid[arr[0]];
		int b = _grid[arr[1]];
		int c = _grid[arr[2]];

		if (a && a == b && b == c) {
			return a;
		}
	}

	return 0;
}

// helper function for a draw
bool TicTacToeAI::isBoardFull() const {
	for (int i = 0; i < 9; i++) {
		if (!_grid[i]) {
			return false;
		}
	}

	return true;
}

// Returns: positive value if AI wins, negative if human player wins, 0 for draw or undecided
int TicTacToeAI::evaluateBoard(int playerColor) {
	int winner = AICheckForWinner();
	if (!winner) { return 0; }
	return winner == playerColor ? 10 : -10;
}


// init like this : negamax(rootState, depth, -inf, +inf, 1)
// player is the current player's number (AI or human)
int TicTacToeAI::negamax(TicTacToeAI* state, int depth, int alpha, int beta, int playerColor) {
	int score = state->evaluateBoard(playerColor);

	// if draw or terminal state
	if (score != 0 || state->isBoardFull()) {
		return score;
	}

	int bestVal = -1000; // Negative "Infinity"
	for (int i = 0; i < 9; i++) {
		if (!state->_grid[i]) {
			state->_grid[i] = playerColor;
			bestVal = std::max(bestVal, -negamax(state, depth + 1, -beta, -alpha, (3 - playerColor)));
			// undo move when backtracing
			state->_grid[i] = 0;

			alpha = std::max(alpha, bestVal);
			if (alpha >= beta) {
				break;
			}
		}
	}

	return bestVal;
}