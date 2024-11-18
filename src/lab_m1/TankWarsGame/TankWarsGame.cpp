#include "lab_m1/TankWarsGame/TankWarsGame.h"


#include "core/engine.h"
#include "utils/gl_utils.h"
#include "lab_m1/TankWarsGame/transform2D.h"
#include "lab_m1/TankWarsGame/object_2D.h"

#include "TankColorSelectionUI.h"
#include "Point2D.h"
#include "Terrain.h"
#include "TrajectoryBeam.h"

#include <vector>

// 20 secunde
#define MAX_IDLE_TIME_IN_SECONDS 20.f

// 5 secunde
#define SECONDS_BETWEEN_WINNING_AND_TRANSITION 5.f


// Verde pentru teren
#define TERRAIN_GREEN_COLOR glm::vec3(0.0f, 0.5f, 0.0f)

// Background color = RGB(9, 195, 237)
#define DAY_SKY_BACKGROUND_COLOR  glm::vec3(9.f / 255.f, 195.f / 255.f, 237.f / 255.f)


#define TANK_1_OX_RESOLUTION_PERCENTAGE 0.2f
#define TANK_1_INITIAL_DEGREES_ANGLE_WITH_OX 135.f


#define TANK_2_OX_RESOLUTION_PERCENTAGE 0.8f
#define TANK_2_INITIAL_DEGREES_ANGLE_WITH_OX 45.f


// RGB(0, 0, 0) = Black
#define BLACK_COLOR glm::vec3(0.f, 0.f, 0.f)

// RGB(1, 1, 1) = White
#define WHITE_COLOR glm::vec3(1.f, 1.f, 1.f)

// RGB(1, 0, 1) = Red
#define RED_COLOR glm::vec3(1.f, 0.f, 0.f)


// RGB(21, 24, 36)
#define TRANSITION_RECTANGLE_COLOR glm::vec3(21.f / 255.f, 24.f / 255.f, 36.f / 255.f)

// 1.5 secunde
#define TIME_OF_COLORING_DAMAGED_TANK_WITH_RED 0.05f





using namespace std;
using namespace m1;


TankWarsGame::TankWarsGame()
{
    glm::ivec2 resolution = window->GetResolution();

    terrain = Terrain::GetInstance(resolution.x);
    transition = Transition::GetInstance();
    tankColorSelection = TankColorSelectionUI::GetInstance(resolution);
}

TankWarsGame::~TankWarsGame()
{
}

void TankWarsGame::Init()
{
    // Rezolutia: 1280 x 720
    glm::ivec2 resolution = window->GetResolution();
    auto camera = GetSceneCamera();
    camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
    camera->SetPosition(glm::vec3(0, 0, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();
    GetCameraInput()->SetActive(false);

    glm::vec3 corner = glm::vec3(0, 0, 0);
    float squareSide = 100;

    cx = corner.x + squareSide / 2;
    cy = corner.y + squareSide / 2;

    translateX = 0;
    translateY = 0;
    scaleX = 1;
    scaleY = 1;
    angularStep = 0;



    terrain->terrainColor = TERRAIN_GREEN_COLOR;
    terrain->CreateTerrainMeshes();

    terrain->SetRandomTerrainFunction();
    terrain->ComputeTerrainVertices();


    transition->rectangleColor = TRANSITION_RECTANGLE_COLOR;

    gameStatus = GameStatus::COLOR_SELECTION_INTRO_INTERFACE;
}




void TankWarsGame::InitTanks()
{
    glm::ivec2 resolution = window->GetResolution();

    tank1.InitializeTank(
        TANK_1_OX_RESOLUTION_PERCENTAGE * resolution.x,   // initial value on OX
        TANK_1_INITIAL_DEGREES_ANGLE_WITH_OX,
        tankColorSelection->playerSelection1.selectedTrapezoidColors.lowerPartColor,
        tankColorSelection->playerSelection1.selectedTrapezoidColors.upperPartColor,
        tankColorSelection->playerSelection1.selectedTrapezoidColors.upperPartColor,
        BLACK_COLOR
    );



    tank2.InitializeTank(
        TANK_2_OX_RESOLUTION_PERCENTAGE * resolution.x,   // initial value on OX
        TANK_2_INITIAL_DEGREES_ANGLE_WITH_OX,
        tankColorSelection->playerSelection2.selectedTrapezoidColors.lowerPartColor,
        tankColorSelection->playerSelection2.selectedTrapezoidColors.upperPartColor,
        tankColorSelection->playerSelection2.selectedTrapezoidColors.upperPartColor,
        BLACK_COLOR
    );

    tank1.UpdateTankVertices(TANK_1_OX_RESOLUTION_PERCENTAGE * resolution.x);
    tank2.UpdateTankVertices(TANK_2_OX_RESOLUTION_PERCENTAGE * resolution.x);
}


void TankWarsGame::GameReset()
{
    glm::ivec2 resolution = window->GetResolution();

    terrain->SetRandomTerrainFunction();
    terrain->ComputeTerrainVertices();

    tank1.ReinitTank(
        TANK_1_OX_RESOLUTION_PERCENTAGE * resolution.x,   // initial value on OX
        TANK_1_INITIAL_DEGREES_ANGLE_WITH_OX
    );


    tank2.ReinitTank(
        TANK_2_OX_RESOLUTION_PERCENTAGE * resolution.x,   // initial value on OX
        TANK_2_INITIAL_DEGREES_ANGLE_WITH_OX
    );


    tank1.UpdateTankVertices(TANK_1_OX_RESOLUTION_PERCENTAGE * resolution.x);
    tank2.UpdateTankVertices(TANK_2_OX_RESOLUTION_PERCENTAGE * resolution.x);

    HideAllTrajectoriesAndProjectiles();

    tank1.UpdateTankMeshes();
    tank2.UpdateTankMeshes();
}



void TankWarsGame::FrameStart()
{
    glClearColor(DAY_SKY_BACKGROUND_COLOR[0], DAY_SKY_BACKGROUND_COLOR[1], DAY_SKY_BACKGROUND_COLOR[2], 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    glViewport(0, 0, resolution.x, resolution.y);
}




/// Renders a frame
void TankWarsGame::Update(float deltaTimeSeconds)
{
    if (gameStatus == GameStatus::COLOR_SELECTION_INTRO_INTERFACE) {
        UpdateTankColorSelectionUI(deltaTimeSeconds);
        return;
    }
    else if (gameStatus == GameStatus::PLAYING) {
        UpdatePlayingGame(deltaTimeSeconds);
        return;
    }
    else if (gameStatus == GameStatus::A_PLAYER_WON) {
        UpdateGameWithWinner(deltaTimeSeconds);
        return;
    }
    else if (gameStatus == GameStatus::DECREASE_IDLE_TANKS_LIVES) {
        UpdateDecreaseTanksLives(deltaTimeSeconds);
        return;
    }
    else if (gameStatus == GameStatus::TRANSITION_TO_RESET_GAME) {
        UpdateTransitionToResetGame(deltaTimeSeconds);
        return;
    }
}

void TankWarsGame::UpdateTankColorSelectionUI(float deltaTimeSeconds)
{
    // Double Checking
    if (gameStatus != GameStatus::COLOR_SELECTION_INTRO_INTERFACE) {
        return;
    }

    glClearColor(WHITE_COLOR[0], WHITE_COLOR[1], WHITE_COLOR[2], 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    tankColorSelection->UpdateTimer(deltaTimeSeconds, resolution);
    
    if (tankColorSelection->isInIntro) {
        RenderTankColorSelectionUI();
        return;
    }

    // Interfata UI de intro a luat sfarsit
    // Incepe jocul!
    glClearColor(DAY_SKY_BACKGROUND_COLOR[0], DAY_SKY_BACKGROUND_COLOR[1], DAY_SKY_BACKGROUND_COLOR[2], 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    InitTanks();
    gameStatus = GameStatus::PLAYING;
    this->elapsedTimeOfLastKeyPressed = Engine::GetElapsedTime();
}


void TankWarsGame::UpdatePlayingGame(float deltaTimeSeconds)
{
    // Double Checking
    if (gameStatus != GameStatus::PLAYING) {
        return;
    }

    double elapsedCurrentTime = Engine::GetElapsedTime();

    if ((gameStatus == GameStatus::PLAYING || gameStatus == GameStatus::A_PLAYER_WON)
        && elapsedCurrentTime - this->elapsedTimeOfLastKeyPressed > MAX_IDLE_TIME_IN_SECONDS) {
        this->gameStatus = GameStatus::DECREASE_IDLE_TANKS_LIVES;
    }

    if ((tank1.tankLife <= 0.f || tank2.tankLife <= 0.f) && gameStatus == GameStatus::PLAYING) {
        this->gameStatus = GameStatus::A_PLAYER_WON;
        this->elapsedTimePlayerWon = Engine::GetElapsedTime();
    }


    glm::ivec2 resolution = window->GetResolution();

    UpdateTerrainHeights(deltaTimeSeconds);
    UpdateProjectiles(deltaTimeSeconds);
    
    tank1.UpdateTankVertices(tank1.position.x);
    tank1.UpdateTankMeshes();
    tank1.trajectoryBeam->UpdateTankTrajectoryBeam();
    tank2.UpdateTankVertices(tank2.position.x);
    tank2.UpdateTankMeshes();
    tank2.trajectoryBeam->UpdateTankTrajectoryBeam();

    RenderTrajectoryBeam(tank1);
    RenderTrajectoryBeam(tank2);

    RenderTerrain();
    RenderTank(tank1);
    RenderTank(tank2);
    RenderProjectiles();
}


void TankWarsGame::UpdateGameWithWinner(float deltaTimeSeconds)
{
    // Double Checking
    if (gameStatus != GameStatus::A_PLAYER_WON) {
        return;
    }

    double elapsedCurrentTime = Engine::GetElapsedTime();
    glm::ivec2 resolution = window->GetResolution();


    if (elapsedCurrentTime - this->elapsedTimePlayerWon > SECONDS_BETWEEN_WINNING_AND_TRANSITION) {
        this->gameStatus = GameStatus::TRANSITION_TO_RESET_GAME;
        this->transition->isInTransition = true;
        transition->ReinitTransition(window->GetResolution());
    }

    if (tank1.tankLife > 0.f) {
        tank1.UpdateTankVertices(tank1.position.x);
        tank1.UpdateTankMeshes();
        tank1.trajectoryBeam->UpdateTankTrajectoryBeam();
        RenderTank(tank1);
        RenderTrajectoryBeam(tank1);
    }

    if (tank2.tankLife > 0.f) {
        tank2.UpdateTankVertices(tank2.position.x);
        tank2.UpdateTankMeshes();
        tank2.trajectoryBeam->UpdateTankTrajectoryBeam();
        RenderTank(tank2);
        RenderTrajectoryBeam(tank2);
    }

    UpdateTerrainHeights(deltaTimeSeconds);
    UpdateProjectiles(deltaTimeSeconds);
    RenderTerrain();
    RenderProjectiles();
}

void TankWarsGame::UpdateDecreaseTanksLives(float deltaTimeSeconds)
{
    // Double Checking
    if (gameStatus != GameStatus::DECREASE_IDLE_TANKS_LIVES) {
        return;
    }

    tank1.DecreaseIdleTankLife(deltaTimeSeconds);
    tank2.DecreaseIdleTankLife(deltaTimeSeconds);

    if (tank1.tankLife <= 0 || tank2.tankLife <= 0) {
        // Game over
        gameStatus = GameStatus::TRANSITION_TO_RESET_GAME;
        this->transition->isInTransition = true;
        transition->ReinitTransition(window->GetResolution());
    }
    else {
        tank1.UpdateTankVertices(tank1.position.x);
        tank2.UpdateTankVertices(tank2.position.x);
        tank1.UpdateTankMeshes();
        tank2.UpdateTankMeshes();
        tank1.trajectoryBeam->UpdateTankTrajectoryBeam();
        tank1.trajectoryBeam->UpdateTankTrajectoryBeam();
    }



    RenderTerrain();
    RenderTrajectoryBeam(tank1);
    RenderTrajectoryBeam(tank2);
    RenderTank(tank1);
    RenderTank(tank2);
    RenderProjectiles();
}


void TankWarsGame::HideAllTrajectoriesAndProjectiles()
{
    for (TrajectoryBeam::BeamPoint& beamPoint : tank1.trajectoryBeam->beamPoints) {
        beamPoint.isVisible = false;
    }
    for (TrajectoryBeam::BeamPoint& beamPoint : tank2.trajectoryBeam->beamPoints) {
        beamPoint.isVisible = false;
    }
    for (Projectile& projectile : projectiles) {
        projectile.isVisible = false;
    }
}



void TankWarsGame::UpdateTransitionToResetGame(float deltaTimeSeconds)
{
    // Double Checking
    if (gameStatus != GameStatus::TRANSITION_TO_RESET_GAME) {
        return;
    }

    UpdateTransitionStatus(deltaTimeSeconds);
    RenderTransition();
    RenderTerrain();
    RenderTrajectoryBeam(tank1);
    RenderTrajectoryBeam(tank2);
    RenderTank(tank1);
    RenderTank(tank2);
}


void TankWarsGame::UpdateTransitionStatus(float deltaTimeSeconds)
{
    // Double Checking
    if (gameStatus != GameStatus::TRANSITION_TO_RESET_GAME) {
        return;
    }

    TransitionState lastTransitionState = transition->transitionState;

    glm::ivec2 resolution = window->GetResolution();
    transition->UpdateTransition(resolution, deltaTimeSeconds);

    if (lastTransitionState == TransitionState::FIRST_HALF && transition->transitionState == TransitionState::SECOND_HALF) {
        GameReset();
    } else if (lastTransitionState == TransitionState::SECOND_HALF && transition->transitionState == TransitionState::END) {
        gameStatus = GameStatus::PLAYING;
        elapsedTimeOfLastKeyPressed = Engine::GetElapsedTime();
    }
}


void TankWarsGame::RenderTankColorSelectionUI()
{
    if (tankColorSelection->playerSelection1.selectionType != SelectionType::NONE) {
        RenderMesh2D(tankColorSelection->playerSelection1.selectionBox.mesh, glm::mat3(1.f), tankColorSelection->playerSelection1.selectionBox.color);
    }
    if (tankColorSelection->playerSelection2.selectionType != SelectionType::NONE) {
        RenderMesh2D(tankColorSelection->playerSelection2.selectionBox.mesh, glm::mat3(1.f), tankColorSelection->playerSelection2.selectionBox.color);
    }

    RenderMesh2D(tankColorSelection->timerBarBox.mesh, glm::mat3(1.f), tankColorSelection->timerBarBox.color);
    RenderMesh2D(tankColorSelection->middleSeparationLine.mesh, glm::mat3(1.f), tankColorSelection->middleSeparationLine.color);

    RenderMesh2D(tankColorSelection->playerSelection1.firstOptionBox.mesh, glm::mat3(1.f), tankColorSelection->playerSelection1.firstOptionBox.color);
    RenderMesh2D(tankColorSelection->playerSelection1.secondOptionBox.mesh, glm::mat3(1.f), tankColorSelection->playerSelection1.secondOptionBox.color);
    
    RenderMesh2D(tankColorSelection->playerSelection2.firstOptionBox.mesh, glm::mat3(1.f), tankColorSelection->playerSelection2.firstOptionBox.color);
    RenderMesh2D(tankColorSelection->playerSelection2.secondOptionBox.mesh, glm::mat3(1.f), tankColorSelection->playerSelection2.secondOptionBox.color);
}

void TankWarsGame::RenderTransition()
{
    RenderMesh2D(transition->rectangleMesh, glm::mat3(1.f), transition->rectangleColor);
}

void TankWarsGame::RenderTerrain()
{
    glm::ivec2 resolution = window->GetResolution();
    float width = resolution.x;
    float height = resolution.y;


    for (size_t i = 0; i < terrain->terrainVertices.size() - 1; i++) {
        float x1 = terrain->terrainVertices[i].x;
        float y1 = terrain->terrainVertices[i].y;
        float x2 = terrain->terrainVertices[i + 1].x;
        float y2 = terrain->terrainVertices[i + 1].y;

        glm::mat3 modelMatrix =
            glm::mat3(1.0f)
            * transform2D::Translate(x1, 0)
            * transform2D::ShearX((y2 - y1) / (x2 - x1))
            * transform2D::Scale(x2 - x1, std::max(y1, y2));

        RenderMesh2D(terrain->terrainMeshes[i], modelMatrix, terrain->terrainColor);
    }
}


void TankWarsGame::UpdateProjectiles(float deltaTimeSeconds)
{
    for (Projectile &projectile : this->projectiles) {
        if (projectile.isVisible) {
            projectile.UpdateProjectilePosition(deltaTimeSeconds, tank1, tank2);
        }
    }
}

void TankWarsGame::UpdateTerrainHeights(float deltaTimeSeconds)
{
    terrain->ComputeLandslide(deltaTimeSeconds);
}




void TankWarsGame::RenderTank(Tank& tank)
{
    if (tank.tankLife <= 0) {
        return;
    }


    if (tank.semiCircleWasRecenltyHit) {
        RenderMesh2D(tank.semiCircle.appearance.mesh, glm::mat3(1.f), RED_COLOR);
        if (Engine::GetElapsedTime() - tank.elapsedTimeSemiCircleWasHit >= TIME_OF_COLORING_DAMAGED_TANK_WITH_RED) {
            tank.semiCircleWasRecenltyHit = false;
        }
    }
    else {
        RenderMesh2D(tank.semiCircle.appearance.mesh, glm::mat3(1.f), tank.semiCircle.appearance.color);
    }

    if (tank.bottomTrapezoidWasRecenltyHit) {
        RenderMesh2D(tank.bottomTrapezoid.appearance.mesh, glm::mat3(1.f), RED_COLOR);
        if (Engine::GetElapsedTime() - tank.elapsedTimeBottomTrapezoidWasHit >= TIME_OF_COLORING_DAMAGED_TANK_WITH_RED) {
            tank.bottomTrapezoidWasRecenltyHit = false;
        }
    }
    else {
        RenderMesh2D(tank.bottomTrapezoid.appearance.mesh, glm::mat3(1.f), tank.bottomTrapezoid.appearance.color);
    }

    if (tank.turretWasRecenltyHit) {
        RenderMesh2D(tank.turret.appearance.mesh, glm::mat3(1.f), RED_COLOR);
        if (Engine::GetElapsedTime() - tank.elapsedTimeTurretWasHit >= TIME_OF_COLORING_DAMAGED_TANK_WITH_RED) {
            tank.turretWasRecenltyHit = false;
        }
    }
    else {
        RenderMesh2D(tank.turret.appearance.mesh, glm::mat3(1.f), tank.turret.appearance.color);
    }

    if (tank.topTrapezoidWasRecenltyHit) {
        RenderMesh2D(tank.topTrapezoid.appearance.mesh, glm::mat3(1.f), RED_COLOR);
        if (Engine::GetElapsedTime() - tank.elapsedTimeTopTrapezoidWasHit >= TIME_OF_COLORING_DAMAGED_TANK_WITH_RED) {
            tank.topTrapezoidWasRecenltyHit = false;
        }
    }
    else {
        RenderMesh2D(tank.topTrapezoid.appearance.mesh, glm::mat3(1.f), tank.topTrapezoid.appearance.color);
    }

    RenderMesh2D(tank.lifeBar.outlineRectangle.mesh, glm::mat3(1.f), tank.lifeBar.outlineRectangle.color);
    RenderMesh2D(tank.lifeBar.fillRectangle.mesh, glm::mat3(1.f), tank.lifeBar.fillRectangle.color);

}



void TankWarsGame::RenderProjectiles()
{
    for (Projectile projectile : this->projectiles) {
        if (projectile.isVisible) {
            RenderMesh2D(projectile.appearance.mesh, glm::mat3(1.f), projectile.appearance.color);
        }
    }
}

void TankWarsGame::RenderTrajectoryBeam(Tank& tank)
{
    if (tank.tankLife <= 0.f) {
        // A totally destroyed tank does not have a trajectory anymore
        return;
    }

    for (TrajectoryBeam::BeamPoint beamPoint : tank.trajectoryBeam->beamPoints) {
        if (beamPoint.isVisible) {
            RenderMesh2D(beamPoint.appearance.mesh, glm::mat3(1.f), beamPoint.appearance.color);
        }
    }
}


void TankWarsGame::FrameEnd()
{
}

// Funcțiile de callback
void TankWarsGame::OnInputUpdate(float deltaTimeSeconds, int mods)
{

    if (gameStatus == GameStatus::COLOR_SELECTION_INTRO_INTERFACE) {
        return;
    }
    else {
        OnInputUpdatePlayingKeyHoldBindings(deltaTimeSeconds, mods);
        return;
    }
}




void TankWarsGame::OnInputUpdatePlayingKeyHoldBindings(float deltaTimeSeconds, int mods)
{
    if (transition->isInTransition) {
        return;
    }
    if (gameStatus == GameStatus::TRANSITION_TO_RESET_GAME) {
        return;
    }


    glm::ivec2 resolution = window->GetResolution();


    if (window->KeyHold(GLFW_KEY_A)) {
        // Muta tancul Playerului 1 la stanga: tasta "A"
        tank1.MoveTankToLeft(deltaTimeSeconds, resolution);
        gameStatus = (tank1.tankLife > 0.f && tank2.tankLife > 0.f) ? GameStatus::PLAYING : GameStatus::A_PLAYER_WON;
        elapsedTimeOfLastKeyPressed = Engine::GetElapsedTime();
    }

    if (window->KeyHold(GLFW_KEY_D)) {
        // Muta tancul Playerului 1 la dreapta: tasta "D"
        tank1.MoveTankToRight(deltaTimeSeconds, resolution);
        gameStatus = (tank1.tankLife > 0.f && tank2.tankLife > 0.f) ? GameStatus::PLAYING : GameStatus::A_PLAYER_WON;
        elapsedTimeOfLastKeyPressed = Engine::GetElapsedTime();
    }


    if (window->KeyHold(GLFW_KEY_LEFT)) {
        // Muta tancul Playerului 1 la stanga: tasta "sageata stanga"
        tank2.MoveTankToLeft(deltaTimeSeconds, resolution);
        gameStatus = (tank1.tankLife > 0.f && tank2.tankLife > 0.f) ? GameStatus::PLAYING : GameStatus::A_PLAYER_WON;
        elapsedTimeOfLastKeyPressed = Engine::GetElapsedTime();
    }

    if (window->KeyHold(GLFW_KEY_RIGHT)) {
        // Muta tancul Playerului 1 la dreapta: tasta "sageata dreapta"
        tank2.MoveTankToRight(deltaTimeSeconds, resolution);
        gameStatus = (tank1.tankLife > 0.f && tank2.tankLife > 0.f) ? GameStatus::PLAYING : GameStatus::A_PLAYER_WON;
        elapsedTimeOfLastKeyPressed = Engine::GetElapsedTime();
    }


    if (window->KeyHold(GLFW_KEY_W)) {
        // Muta turela Playerului 1 la stanga
        tank1.MoveTurretToLeft(deltaTimeSeconds, resolution);
        gameStatus = (tank1.tankLife > 0.f && tank2.tankLife > 0.f) ? GameStatus::PLAYING : GameStatus::A_PLAYER_WON;
        elapsedTimeOfLastKeyPressed = Engine::GetElapsedTime();
    }

    if (window->KeyHold(GLFW_KEY_S)) {
        // Muta turela Playerului 1 la dreapta
        tank1.MoveTurretToRight(deltaTimeSeconds, resolution);
        gameStatus = (tank1.tankLife > 0.f && tank2.tankLife > 0.f) ? GameStatus::PLAYING : GameStatus::A_PLAYER_WON;
        elapsedTimeOfLastKeyPressed = Engine::GetElapsedTime();
    }



    if (window->KeyHold(GLFW_KEY_DOWN)) {
        // Muta turela Playerului 2 la stanga
        tank2.MoveTurretToLeft(deltaTimeSeconds, resolution);
        gameStatus = (tank1.tankLife > 0.f && tank2.tankLife > 0.f) ? GameStatus::PLAYING : GameStatus::A_PLAYER_WON;
        elapsedTimeOfLastKeyPressed = Engine::GetElapsedTime();
    }

    if (window->KeyHold(GLFW_KEY_UP)) {
        // Muta turela Playerului 3 la dreapta
        tank2.MoveTurretToRight(deltaTimeSeconds, resolution);
        gameStatus = (tank1.tankLife > 0.f && tank2.tankLife > 0.f) ? GameStatus::PLAYING : GameStatus::A_PLAYER_WON;
        elapsedTimeOfLastKeyPressed = Engine::GetElapsedTime();
    }



}

void TankWarsGame::OnKeyPress(int key, int mods)
{
    if (gameStatus == GameStatus::COLOR_SELECTION_INTRO_INTERFACE) {
        OnKeyPressTankColorSelectionUIKeyBindings(key, mods);
        return;
    }
    else {
        OnKeyPressActualGameKeyBindings(key, mods);
        return;
    }
}

void TankWarsGame::OnKeyPressTankColorSelectionUIKeyBindings(int key, int mods)
{
    if (key == GLFW_KEY_S || key == GLFW_KEY_W) {
        if (tankColorSelection->playerSelection1.selectionType == SelectionType::NONE) {
            if (key == GLFW_KEY_S) {
                tankColorSelection->playerSelection1.UpdateSelectionBox(SelectionType::BOY, true);
            }
            else {
                tankColorSelection->playerSelection1.UpdateSelectionBox(SelectionType::GIRL, true);
            }
        }
        else if (tankColorSelection->playerSelection1.selectionType == SelectionType::BOY) {
            tankColorSelection->playerSelection1.UpdateSelectionBox(SelectionType::GIRL, true);
        }
        else if (tankColorSelection->playerSelection1.selectionType == SelectionType::GIRL) {
            tankColorSelection->playerSelection1.UpdateSelectionBox(SelectionType::BOY, true);
        }
    }


    if (key == GLFW_KEY_DOWN || key == GLFW_KEY_UP) {
        if (tankColorSelection->playerSelection2.selectionType == SelectionType::NONE) {
            if (key == GLFW_KEY_DOWN) {
                tankColorSelection->playerSelection2.UpdateSelectionBox(SelectionType::BOY, false);
            }
            else {
                tankColorSelection->playerSelection2.UpdateSelectionBox(SelectionType::GIRL, false);
            }
        }
        else if (tankColorSelection->playerSelection2.selectionType == SelectionType::BOY) {
            tankColorSelection->playerSelection2.UpdateSelectionBox(SelectionType::GIRL, false);
        }
        else if (tankColorSelection->playerSelection2.selectionType == SelectionType::GIRL) {
            tankColorSelection->playerSelection2.UpdateSelectionBox(SelectionType::BOY, false);
        }
    }


    if (key == GLFW_KEY_ENTER) {
        // Incepe jocul
        InitTanks();
        gameStatus = GameStatus::PLAYING;
        this->elapsedTimeOfLastKeyPressed = Engine::GetElapsedTime();
    }
}


void TankWarsGame::OnKeyPressActualGameKeyBindings(int key, int mods)
{
    if (gameStatus == GameStatus::TRANSITION_TO_RESET_GAME) {
        return;
    }

    if (key == GLFW_KEY_SPACE) {
        // Playerul 1 a tras cu tunul
        Projectile *projectile = GetProjectileToLaunch();
        projectile->LaunchProjectile(tank1);

        gameStatus = (tank1.tankLife > 0.f && tank2.tankLife > 0.f) ? GameStatus::PLAYING : GameStatus::A_PLAYER_WON;
        elapsedTimeOfLastKeyPressed = Engine::GetElapsedTime();
    }

    if (key == GLFW_KEY_ENTER) {
        // Playerul 2 a tras cu tunul
        Projectile *projectile = GetProjectileToLaunch();
        projectile->LaunchProjectile(tank2);
        
        gameStatus = (tank1.tankLife > 0.f && tank2.tankLife > 0.f) ? GameStatus::PLAYING : GameStatus::A_PLAYER_WON;
        elapsedTimeOfLastKeyPressed = Engine::GetElapsedTime();
    }
}



Projectile *TankWarsGame::GetProjectileToLaunch()
{
    for (Projectile& projectile : this->projectiles) {
        if (!projectile.isVisible) {
            return &projectile;
        }
    }


    this->projectiles.push_back(Projectile());
    return &this->projectiles.back();
}


void TankWarsGame::OnKeyRelease(int key, int mods) {}
void TankWarsGame::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) {}
void TankWarsGame::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) {}
void TankWarsGame::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) {}
void TankWarsGame::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) {}
void TankWarsGame::OnWindowResize(int width, int height) {}

