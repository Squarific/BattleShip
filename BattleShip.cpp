// Battleship console game
// Created by Filip Smets
// License: MIT

#include <vector>
#include <iostream>
#include <stdlib.h>
#include <time.h>

using namespace std;

class Coords {
	public:
		int x;
		char y;

		Coords () {};

		Coords (int x, char y) {
			this->x = x;
			this->y = y;
		};

		int getX () {
			return this->x;
		};

		int getY () {
			// Return the Y coordinate as a 1indexed integer
			// Letters go up from A to Z so if we do the letter
			// minus the start point we get a 1indexed coord
			int letterStart = 'A';
			return toupper(this->y) - letterStart + 1;
		};

		bool operator== (Coords c2) {
		    return (this->x == c2.x && this->y == c2.y);
		};

		Coords operator+ (Coords c2) {
			return Coords(this->x + c2.x, this->y + c2.y);
		};
};

class BattleShipShip {
	public:
		enum direction {HORIZONTAL, VERTICAL};

		BattleShipShip(Coords* shipCoords, direction shipDirection, int length) {
			this->coords = *shipCoords;
			this->shipDirection = shipDirection;
			this->length = length;
		};

		bool isOnCoord (Coords target) {
			// Returns true if the ship is on this coord
			// Otherwise returns false

			for (int i = 0; i < this->length; i++) {

				// Because coords have to be saved as chars, we need to know the relative
				// start point since Chars are just integers we convert it to an int
				int letterStart = 'A';

				// Depending on the direction of the ship we have to add to x or y
				int xMultiplier = 1 - this->shipDirection;
				int yMultiplier = this->shipDirection;

				// Calculate the next coord that the ship is on
				Coords shipCoord = this->coords + Coords(xMultiplier * i, yMultiplier * i + letterStart);

				// If this is the target, then this ship is on that coord
				if (shipCoord == target) {
					return true;
				}
			}

			return false;
		}

		bool collides (BattleShipShip* ship) {
			// Returns true if the ships collide
			// Otherwise returns false

			for (int i = 0; i < ship->length; i++) {
				// Because coords have to be saved as chars, we need to know the relative
				// start point since Chars are just integers we convert it to an int
				int letterStart = 'A';

				// Depending on the direction of the ship we have to add to x or y
				int xMultiplier = 1 - ship->shipDirection;
				int yMultiplier = ship->shipDirection;

				// Calculate the next coord that the ship is on
				Coords shipCoord = ship->coords + Coords(xMultiplier * i, yMultiplier * i + letterStart);

				// If the targetship is on the same coord as this ship then there is a collision
				if (this->isOnCoord(shipCoord)) {
					return true;
				}
			}

			return false;
		}
		
		Coords coords;
		int length;
		direction shipDirection;
};


class BattleShipBoard {
	public:
		BattleShipBoard(int width, int height) {
			this->width = width;
			this->height = height;
		};

		bool fire(Coords* target) {
			// Returns false if the target has already been shot
			// Otherwise shoots the target and returns true

			if (this->hasBeenShot(target)) return false;
			this->missiles.push_back(target);
			return true;
		};
		
		bool addShip(BattleShipShip* ship) {
			// Returns false if the ship could not be added
			// Otherwise adds the ship and returns true

			if (!this->shipCanBeAdded(ship)) return false;
			this->ships.push_back(ship);
			return true;
		};

		bool shipCanBeAdded (BattleShipShip* targetShip) {
			// Returns true if the ship can be added
			// Otherwise returns false

			// First check if ship is within bounds
			if (this->isShipOutOfBounds(targetShip)) return false;

			// Check if it does not collide with another ship
			for (BattleShipShip* ship : this->ships) {
				if (ship->collides(targetShip)) return false;
			}

			// Ship can be added
			return true;
		};

		bool isShipOutOfBounds (BattleShipShip* targetShip) {
			for (int i = 0; i < targetShip->length; i++) {

				// Because coords have to be saved as chars, we need to know the relative
				// start point since Chars are just integers we convert it to an int
				int letterStart = 'A';

				// Depending on the direction of the ship we have to add to x or y
				int xMultiplier = 1 - targetShip->shipDirection;
				int yMultiplier = targetShip->shipDirection;

				// Calculate the next coord that the ship is on
				Coords shipCoord = targetShip->coords + Coords(xMultiplier * i, yMultiplier * i + letterStart);

				// Get the x and y
				int x = shipCoord.getX();
				int y = shipCoord.getY();

				// If the coord is out of bounds the ship is too
				if (x < 1 || x > this->width || y < 1 || y > this->height) {
					return false;
				}
			}
		};

		bool hasBeenShot (Coords* target) {
			// Returns true if a missile for this coord exists
			// Otherwise returns false

			for (Coords* missile : this->missiles) {
				if (*target == *missile) return true;
			}

			return false;
		};

		vector<BattleShipShip*> ships;
		vector<Coords*> missiles;
		int width;
		int height;
};

class BattleShipGame {
	private:
		vector<BattleShipBoard> boards;
	public:
		BattleShipGame() {
			this->boards.push_back(BattleShipBoard(10, 10));
			this->boards.push_back(BattleShipBoard(10, 10));
		};

		BattleShipBoard* getBoard (int board) {
			return &this->boards.at(board);
		};

		vector<int> defaultSizes = {5, 4, 3, 3, 2};
		bool ended = false;
		bool fire(int player, Coords* target) {

		};
};

class BattleShipCMDInterface {
	public:
		
		void setup (BattleShipBoard* board, vector<int>* sizes) {
			for (int size : *sizes) {
				// Keep asking the player where to place the ship
				// untill we succeed at adding it
				while (true) {
					int inputX;
					char inputY;

					cout << "Adding ship of length " << size << endl;

					// Ask the coordinates
					cout << "Please provide a X coordinate (1-10):" << endl;
					cin >> inputX;

					cout << "Please provide a Y coordinate (A-L) " << endl;
					cin >> inputY;

					Coords shipCoords = Coords(inputX, inputY);

					// Ask for a direction
					cout << "Please provide a direction (0 = horizontal, 1 = vertical)" << endl;
					BattleShipShip::direction dir = static_cast<BattleShipShip::direction>(rand() % 1);

					BattleShipShip tempShip = BattleShipShip(&shipCoords, dir, size);
					if (board->addShip(&tempShip)) break;

					cout << "Couldn't add ship, lets try again... " << endl;
				}
			}
		};
};

class BattleShipAI {
	public:
		enum aiType {SIMPLE, ADVANCED};
		aiType currentAiType;

		BattleShipAI(BattleShipAI::aiType currentAiType) {
			this->currentAiType = currentAiType;
		};

		void setup (BattleShipBoard* board, vector<int>* sizes) {
			// Add ships to the board

			for (int size : *sizes) {

				// Keep trying randomly to add a ship untill we succeed
				while (true) {
					// Generate a random coordinate
					int randomX = rand() % board->width + 1;
					int randomY = rand() % board->height + 1;
					Coords shipCoords = Coords(randomX, randomY);

					// Generate a random direction
					BattleShipShip::direction dir = static_cast<BattleShipShip::direction>(rand() % 1);


					BattleShipShip tempShip = BattleShipShip(&shipCoords, dir, size);
					if (board->addShip(&tempShip)) break;
				}
			}
		};

		void nextTarget(BattleShipBoard* board) {

		};
};

int main () {
	cout << "test";
	// Initialize by seeding random
	srand(time(NULL));
	cout << "test";

	BattleShipGame game = BattleShipGame();

	BattleShipAI computer = BattleShipAI(BattleShipAI::ADVANCED);
	BattleShipCMDInterface player = BattleShipCMDInterface();

	// Setup the game for the computer and the player
	cout << "Letting the computer place its pieces...";
	computer.setup(game.getBoard(1), &game.defaultSizes);
	cout << "Now it's your turn!";
	player.setup(game.getBoard(1), &game.defaultSizes);

	//while (!game.ended) {
	//
	//}

	return 0;
}