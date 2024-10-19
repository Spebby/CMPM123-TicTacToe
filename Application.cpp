#include "Application.h"
#include "imgui/imgui.h"
#include "classes/TicTacToe.h"
#include "tools/Logger.h"

namespace ClassGame {
    //
    // our global variables
    //
    TicTacToe *game = nullptr;
    bool gameOver = false;
    int gameWinner = -1;

    // game starting point
    // this is called by the main render loop in main.cpp
    //
    void GameStartUp() {
        game = new TicTacToe();
        game->setUpBoard();
    }

    //
    // game render loop
    // this is called by the main render loop in main.cpp
    //
    void RenderGame() {
        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

        //ImGui::ShowDemoWindow();

        ImGui::Begin("Settings");
        //ImGui::Text("Current Player Number: %d", game->getCurrentPlayer()->playerNumber());
        //ImGui::Text("Current Board State: %s", game->stateString().c_str());

        if (game->_gameOver) {
            ImGui::Text("Game Over!");
            ImGui::Text("Winner: %d", gameWinner);
            if (ImGui::Button("Reset Game")) {
                game->stopGame();
                game->setUpBoard();
                game->_gameOver = false;
                gameWinner = -1;
            }
        }
        ImGui::End();

        ImGui::Begin("GameWindow");
        game->drawFrame();
        ImGui::End();
        Logger::getInstance().draw();
    }

    //
    // end turn is called by the game code at the end of each turn
    // this is where we check for a winner
    //
    void EndOfTurn() {
        Player* winner = game->checkForWinner();
        if (winner) {
            gameWinner = winner->playerNumber();
        } else if (game->checkForDraw()) {
            gameWinner = -1;
        } else {
            Logger::getInstance().log(std::to_string(game->getCurrentPlayer()->playerNumber()) + "\'s turn.");
        }
    }
}
