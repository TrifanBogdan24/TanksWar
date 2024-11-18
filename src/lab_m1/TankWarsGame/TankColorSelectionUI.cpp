#include "TankColorSelectionUI.h"
#include "Point2D.h"
#include "object_2D.h"


#define TIMER_SPEED ((16.f/9.f) * 1920.f * 0.05f)

#define TIMER_BAR_HEIGHT 30.f

#define MIDDLE_SEPARATION_LINE_WIDTH 4.f


// RGB(29, 33, 51)
#define TIMER_FILL_COLOR glm::vec3(29.f / 255.f, 3.f / 255.f, 51.f / 255.f)


#define WHITE_COLOR glm::vec3(1.f, 1.f, 1.f)
#define BLACK_COLOR glm::vec3(0.f, 0.f, 0.f)

// RGB(197, 163, 123) - culoarea chenarului stanga-jos
#define UPPER_TANK_PART_BOY_COLOR_FOR_PLAYER_1 glm::vec3(197.f / 255.f, 163.f / 255.f, 123.f / 255.f)
// RGB(108, 81,55)
#define LOWER_TANK_PART_BOY_COLOR_FOR_PLAYER_1 glm::vec3(108.f / 255.f, 81.f / 255.f, 55.f / 255.f)

// RGB(255, 105, 180) - culoarea chenarului stanga-sus
#define UPPER_TANK_PART_GIRL_COLOR_FOR_PLAYER_1 glm::vec3(255.f / 255.f, 105.f / 255.f, 180.f / 255.f)
// RGB(150, 123, 167)
#define LOWER_TANK_PART_GIRL_COLOR_FOR_PLAYER_1 glm::vec3(150.f / 255.f, 123.f / 255.f, 167.f / 255.f)

// RGB(158, 185, 116) - culoarea chenarului dreapta-jos
#define UPPER_TANK_PART_BOY_COLOR_FOR_PLAYER_2  glm::vec3(158.f / 255.f, 185.f / 255.f, 116.f / 255.f)
// RGB(75, 96, 58)
#define LOWER_TANK_PART_BOY_COLOR_FOR_PLAYER_2  glm::vec3(75.f / 255.f, 96.f / 255.f, 58.f / 255.f)

// RGB(138, 43, 226) - culoarea chenarului dreapta-sus
#define UPPER_TANK_PART_GIRL_COLOR_FOR_PLAYER_2 glm::vec3(138.f / 255.f, 43.f / 255.f, 226.f / 255.f)
// RGB(255, 182, 193)
#define LOWER_TANK_PART_GIRL_COLOR_FOR_PLAYER_2 glm::vec3(255.f / 255.f, 182.f / 255.f, 193.f / 255.f)


using namespace std;


TankColorSelectionUI* TankColorSelectionUI::instance = nullptr;



// The first instance must be called with the PARAMETIRZED constructor
TankColorSelectionUI* TankColorSelectionUI::GetInstance(glm::ivec2 &resolution)
{
	if (instance != nullptr) {
		throw std::runtime_error("First instance must be created with a PARMETERIZED constructor!");
	}

	instance = new TankColorSelectionUI(resolution);
	return instance;
}

// Any other instance must be called with the DEFAULT constructor
TankColorSelectionUI* TankColorSelectionUI::GetInstance()
{
	if (instance == nullptr) {
		throw std::runtime_error("Any other instances (created after the first one) must be created with the DEFAULT constructor!");
	}

	instance = new TankColorSelectionUI();
	return instance;
}

TankColorSelectionUI::TankColorSelectionUI(glm::ivec2 &resolution)
{
	this->isInIntro = true;

	CreateMeshes();
	InitColors();

	this->playerSelection1.selectionType = SelectionType::NONE;
	this->playerSelection2.selectionType = SelectionType::NONE;

	InitBottomLeftPoints(resolution);
	InitDimensions(resolution);


	// Call this method only after CREATE!!!
	InitMeshes();

	this->playerSelection1.selectedTrapezoidColors.lowerPartColor = LOWER_TANK_PART_BOY_COLOR_FOR_PLAYER_1;
	this->playerSelection1.selectedTrapezoidColors.upperPartColor = UPPER_TANK_PART_BOY_COLOR_FOR_PLAYER_1;

	this->playerSelection2.selectedTrapezoidColors.lowerPartColor = LOWER_TANK_PART_BOY_COLOR_FOR_PLAYER_2;
	this->playerSelection2.selectedTrapezoidColors.upperPartColor = UPPER_TANK_PART_BOY_COLOR_FOR_PLAYER_2;

}





TankColorSelectionUI::TankColorSelectionUI()
{

}





TankColorSelectionUI::~TankColorSelectionUI()
{
	delete instance;
	instance = nullptr;
}



void TankColorSelectionUI::CreateMeshes()
{
	this->playerSelection1.firstOptionBox.mesh = object_2D::MemAllocMesh("Player 1 - baiat - chenarul stanga jos");
	this->playerSelection1.secondOptionBox.mesh = object_2D::MemAllocMesh("Player 1 - fata - chenarul stanga sus");

	this->playerSelection2.firstOptionBox.mesh = object_2D::MemAllocMesh("Player 2 - baiat - chenarul dreapta jos");
	this->playerSelection2.secondOptionBox.mesh = object_2D::MemAllocMesh("Player 2 - fata - chenarul dreapta sus");

	this->playerSelection1.selectionBox.mesh = object_2D::MemAllocMesh("Chenarul de selectie al Player-ului 1");
	this->playerSelection2.selectionBox.mesh = object_2D::MemAllocMesh("Chenarul de selectie al Player-ului 2");

	this->timerBarBox.mesh = object_2D::MemAllocMesh("Chenarul pentru temporizator");
	this->middleSeparationLine.mesh = object_2D::MemAllocMesh("Linia care separa orizontal optiunile celor doua jucatori");
}


void TankColorSelectionUI::InitColors()
{
	this->playerSelection1.firstOptionBox.color = UPPER_TANK_PART_BOY_COLOR_FOR_PLAYER_1;
	this->playerSelection1.secondOptionBox.color = UPPER_TANK_PART_GIRL_COLOR_FOR_PLAYER_1;

	this->playerSelection2.firstOptionBox.color = UPPER_TANK_PART_BOY_COLOR_FOR_PLAYER_2;
	this->playerSelection2.secondOptionBox.color = UPPER_TANK_PART_GIRL_COLOR_FOR_PLAYER_2;

	this->playerSelection1.selectionBox.color = WHITE_COLOR;
	this->playerSelection2.selectionBox.color = WHITE_COLOR;

	this->timerBarBox.color = TIMER_FILL_COLOR;
	this->middleSeparationLine.color = BLACK_COLOR;
}


void TankColorSelectionUI::InitBottomLeftPoints(glm::ivec2 &resolution)
{
	this->playerSelection1.firstOptionBox.bottomLeft.x = 0.f;
	this->playerSelection1.firstOptionBox.bottomLeft.y = 0.f;

	this->playerSelection1.secondOptionBox.bottomLeft.x = 0.f;
	this->playerSelection1.secondOptionBox.bottomLeft.y = (resolution.y - TIMER_BAR_HEIGHT) / 2.f;

	this->playerSelection2.firstOptionBox.bottomLeft.x = (resolution.x + MIDDLE_SEPARATION_LINE_WIDTH) / 2.f;
	this->playerSelection2.firstOptionBox.bottomLeft.y = 0.f;

	this->playerSelection2.secondOptionBox.bottomLeft.x = (resolution.x + MIDDLE_SEPARATION_LINE_WIDTH) / 2.f;
	this->playerSelection2.secondOptionBox.bottomLeft.y = (resolution.y - TIMER_BAR_HEIGHT) / 2.f;

	this->timerBarBox.bottomLeft.x = 0.f;
	this->timerBarBox.bottomLeft.y = resolution.y - TIMER_BAR_HEIGHT;

	this->middleSeparationLine.bottomLeft.x = (resolution.x - MIDDLE_SEPARATION_LINE_WIDTH) / 2.f;
	this->middleSeparationLine.bottomLeft.y = 0.f;


	// Coordonatele chenarelor de selectie se calculeaza la apasarea tastelor A/D/S/W, respectiv UP/DOWN/LEFT/RIGHT
}


void TankColorSelectionUI::InitDimensions(glm::ivec2 &resolution)
{
	float boxHeight = (resolution.y - TIMER_BAR_HEIGHT) / 2.f;
	float boxWidth = (resolution.x - MIDDLE_SEPARATION_LINE_WIDTH) / 2.f;


	this->playerSelection1.firstOptionBox.width = boxWidth;
	this->playerSelection1.firstOptionBox.height = boxHeight;

	this->playerSelection1.secondOptionBox.width = boxWidth;
	this->playerSelection1.secondOptionBox.height = boxHeight;

	this->playerSelection2.firstOptionBox.width = boxWidth;
	this->playerSelection2.firstOptionBox.height = boxHeight;

	this->playerSelection2.secondOptionBox.width = boxWidth;
	this->playerSelection2.secondOptionBox.height = boxHeight;

	this->playerSelection1.selectionBox.width = boxWidth;
	this->playerSelection1.selectionBox.height = boxHeight;

	this->playerSelection2.selectionBox.width = boxWidth;
	this->playerSelection2.selectionBox.height = boxHeight;

	this->timerBarBox.width = 0.f;
	this->timerBarBox.height = TIMER_BAR_HEIGHT;

	this->middleSeparationLine.width = MIDDLE_SEPARATION_LINE_WIDTH;
	this->middleSeparationLine.height = resolution.y - TIMER_BAR_HEIGHT;

}

/// Call this method only after CREATE!!!
void TankColorSelectionUI::InitMeshes()
{

	object_2D::UpdateRectangle(
		this->middleSeparationLine.mesh,
		glm::vec3(this->middleSeparationLine.bottomLeft.x, this->middleSeparationLine.bottomLeft.y, 0.f),
		this->middleSeparationLine.width,
		this->middleSeparationLine.height,
		this->middleSeparationLine.color,
		true
	);


	object_2D::UpdateRectangle(
		this->playerSelection1.firstOptionBox.mesh,
		glm::vec3(this->playerSelection1.firstOptionBox.bottomLeft.x, this->playerSelection1.firstOptionBox.bottomLeft.y, 0.f),
		this->playerSelection1.firstOptionBox.width,
		this->playerSelection1.firstOptionBox.height,
		this->playerSelection1.firstOptionBox.color,
		true
	);

	object_2D::UpdateRectangle(
		this->playerSelection1.secondOptionBox.mesh,
		glm::vec3(this->playerSelection1.secondOptionBox.bottomLeft.x, this->playerSelection1.secondOptionBox.bottomLeft.y, 0.f),
		this->playerSelection1.secondOptionBox.width,
		this->playerSelection1.secondOptionBox.height,
		this->playerSelection1.secondOptionBox.color,
		true
	);


	object_2D::UpdateRectangle(
		this->playerSelection2.firstOptionBox.mesh,
		glm::vec3(this->playerSelection2.firstOptionBox.bottomLeft.x, this->playerSelection2.firstOptionBox.bottomLeft.y, 0.f),
		this->playerSelection2.firstOptionBox.width,
		this->playerSelection2.firstOptionBox.height,
		this->playerSelection2.firstOptionBox.color,
		true
	);

	object_2D::UpdateRectangle(
		this->playerSelection2.secondOptionBox.mesh,
		glm::vec3(this->playerSelection2.secondOptionBox.bottomLeft.x, this->playerSelection2.secondOptionBox.bottomLeft.y, 0.f),
		this->playerSelection2.secondOptionBox.width,
		this->playerSelection2.secondOptionBox.height,
		this->playerSelection2.secondOptionBox.color,
		true
	);
}



void TankColorSelectionUI::UpdateTimer(float deltaTimeSeconds, glm::ivec2 &resolution)
{
	this->timerBarBox.width += deltaTimeSeconds * TIMER_SPEED;
	
	if (this->timerBarBox.width >= resolution.x) {
		this->timerBarBox.width = resolution.x;
		this->isInIntro = false;
		return;
	}

	object_2D::UpdateRectangle(
		this->timerBarBox.mesh,
		glm::vec3(this->timerBarBox.bottomLeft.x, this->timerBarBox.bottomLeft.y, 0.f),
		this->timerBarBox.width,
		this->timerBarBox.height,
		this->timerBarBox.color,
		true
	);

}




void PlayerSelection::UpdateSelectionBox(SelectionType selectionType_value, bool isFirstPlayer)
{
	this->selectionType = selectionType_value;

	if (this->selectionType == SelectionType::NONE) {
		return;
	}

	if (this->selectionType == SelectionType::BOY) {
		this->selectionBox.bottomLeft.x = this->firstOptionBox.bottomLeft.x;
		this->selectionBox.bottomLeft.y = this->firstOptionBox.bottomLeft.y;

		if (isFirstPlayer) {
			this->selectedTrapezoidColors.lowerPartColor = LOWER_TANK_PART_BOY_COLOR_FOR_PLAYER_1;
			this->selectedTrapezoidColors.upperPartColor = UPPER_TANK_PART_BOY_COLOR_FOR_PLAYER_1;
		}
		else {
			this->selectedTrapezoidColors.lowerPartColor = LOWER_TANK_PART_BOY_COLOR_FOR_PLAYER_2;
			this->selectedTrapezoidColors.upperPartColor = UPPER_TANK_PART_BOY_COLOR_FOR_PLAYER_2;

		}
	} else if (this->selectionType == SelectionType::GIRL) {
		this->selectionBox.bottomLeft.x = this->secondOptionBox.bottomLeft.x;
		this->selectionBox.bottomLeft.y = this->secondOptionBox.bottomLeft.y;

		if (isFirstPlayer) {
			this->selectedTrapezoidColors.lowerPartColor = LOWER_TANK_PART_GIRL_COLOR_FOR_PLAYER_1;
			this->selectedTrapezoidColors.upperPartColor = UPPER_TANK_PART_GIRL_COLOR_FOR_PLAYER_1;
		}
		else {
			this->selectedTrapezoidColors.lowerPartColor = LOWER_TANK_PART_GIRL_COLOR_FOR_PLAYER_2;
			this->selectedTrapezoidColors.upperPartColor = UPPER_TANK_PART_GIRL_COLOR_FOR_PLAYER_2;

		}
	}


	object_2D::UpdateRectangle(
		this->selectionBox.mesh,
		glm::vec3(this->selectionBox.bottomLeft.x, this->selectionBox.bottomLeft.y, 0.f),
		this->selectionBox.width,
		this->selectionBox.height,
		this->selectionBox.color,
		false
	);
}


