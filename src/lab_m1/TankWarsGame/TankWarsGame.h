#pragma once

#include "components/simple_scene.h"

#include "Point2D.h"
#include "Terrain.h"
#include "Transition.h"
#include "TrapezoidVertices.h"
#include "Tank.h"
#include "TankColorSelectionUI.h"
#include "TrajectoryBeam.h"



namespace m1
{
    enum class GameStatus {
        COLOR_SELECTION_INTRO_INTERFACE,
        PLAYING,           // Not idle
        DECREASE_IDLE_TANKS_LIVES,
        A_PLAYER_WON,
        TRANSITION_TO_RESET_GAME
    };

    class TankWarsGame : public gfxc::SimpleScene
    {
    public:
        TankWarsGame();
        ~TankWarsGame();

        void Init() override;
        void GameReset();

    private:
        void InitTanks();

        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
       
        void UpdateTankColorSelectionUI(float deltaTimeSeconds);
        void UpdateGameWithWinner(float deltaTimeSeconds);
        void UpdatePlayingGame(float deltaTimeSeconds);
        void UpdateDecreaseTanksLives(float deltaTimeSeconds);
        void UpdateTransitionToResetGame(float deltaTimeSeconds);


        void UpdateTransitionStatus(float deltaTimeSeconds);
        void UpdateProjectiles(float deltaTimeSeconds);
        void UpdateTerrainHeights(float deltaTimeSeconds);
        
        void RenderProjectiles();
        void RenderTankColorSelectionUI();
        void RenderTerrain();
        void RenderTransition();
        void RenderTank(Tank& tank);
        void RenderTrajectoryBeam(Tank& tank);

        void HideAllTrajectoriesAndProjectiles();


       void FrameEnd() override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;


        void OnInputUpdatePlayingKeyHoldBindings(float deltaTime, int mods);

        void OnKeyPressTankColorSelectionUIKeyBindings(int key, int mods);
        void OnKeyPressActualGameKeyBindings(int key, int mods);

        Projectile* GetProjectileToLaunch();


    protected:
        float cx, cy;
        float translateX, translateY;
        float scaleX, scaleY;
        float angularStep;

        Terrain* terrain;

        Tank tank1;
        Tank tank2;

        Transition* transition;

        TankColorSelectionUI* tankColorSelection;

        GameStatus gameStatus;

        std::vector<Projectile> projectiles;

        // Timpul (in secunde) scurs de la pornirea aplicatiei si cel mai recent moment al apasarii unei teste (relevante)
        double elapsedTimeOfLastKeyPressed;

        // Timpul (in secunde) scurs de la pornirea jocului si momentul in care un jucator a castigat
        double elapsedTimePlayerWon;
    };
}   // namespace m1
