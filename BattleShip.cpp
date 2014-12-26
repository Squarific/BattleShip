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

		Coords up () {
			int letterStart = 'A';
			return Coords(this->getX(), (this->getY() - 2) + letterStart);
		};

		Coords down () {
			int letterStart = 'A';
			return Coords(this->getX(), (this->getY()) + letterStart);
		};

		Coords left () {
			return Coords(this->x - 1, this->y);
		};

		Coords right () {
			return Coords(this->x + 1, this->y);
		};

		bool operator== (Coords c2) {
		    return (this->x == c2.x && this->y == c2.y);
		};

		Coords operator+ (Coords c2) {
			int extraY = c2.getY() - 1;
			return Coords(this->x + c2.x, this->y + extraY);
		};
};

class BattleShipShip {
	public:
		enum direction {HORIZONTAL, VERTICAL};

		BattleShipShip(Coords shipCoords, direction shipDirection, int length) {
			this->coords = shipCoords;
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
		BattleShipBoard(int width, int height, bool enemy) {
			this->width = width;
			this->height = height;
			this->enemy = enemy;
		};

		bool fire (Coords target, bool debug) {
			// Returns false if the target has already been shot
			// Otherwise shoots the target and returns true
			int x = target.getX();
			int y = target.getY();

			if (x < 1 || y < 1 || x > this->width || y > this->height) {
				if (debug) {
					cout << "This coordinate is out off the board!";
				}
				return false;
			}

			if (this->hasBeenShot(target)) {
				if (debug) {
					cout << "This coordinate has already been shot to.";
				}
				return false;
			}
			this->missiles.push_back(target);
			return true;
		};
		
		bool addShip(BattleShipShip ship) {
			this->addShip(ship, false);
		}

		bool addShip(BattleShipShip ship, bool debug) {
			// Returns false if the ship could not be added
			// Otherwise adds the ship and returns true

			if (!this->shipCanBeAdded(&ship, debug)) return false;
			this->ships.push_back(ship);
			return true;
		};

		bool shipCanBeAdded (BattleShipShip* targetShip, bool debug) {
			// Returns true if the ship can be added
			// Otherwise returns false

			// First check if ship is within bounds
			if (this->isShipOutOfBounds(targetShip)) {
				if (debug) {
					cout << "The ship didn't fit into the board!" << endl;
				}
				return false;
			}

			// Check if it does not collide with another ship
			for (BattleShipShip ship : this->ships) {
				if (ship.collides(targetShip)) {
					if (debug) {
						cout << "The ship collided with another one!" << endl;
					}
					return false;
				}
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
					return true;
				}
			}
			return false;
		};

		bool hasBeenShot (Coords target) {
			// Returns true if a missile for this coord exists
			// Otherwise returns false

			for (Coords missile : this->missiles) {
				if (target == missile) return true;
			}

			return false;
		};

		bool hasShip (Coords target) {
			// Returns true if this coordinate contains a ship
			// Otherwise returns false
			for (BattleShipShip ship : this->ships) {
				if (ship.isOnCoord(target)) return true;
			}

			return false;
		};

		bool sunkShip (Coords target) {
			// Returns if there is a sunk ship at this coordinate
			// Otherwise returns false

			for (BattleShipShip ship : this->ships) {
				if (ship.isOnCoord(target)) {
					if (this->isShipDown(ship)) {
						return true;
					}
					return false;
				};
			}

			return false;
		};

		bool isShipDown (BattleShipShip ship) {
			// Returns if the given ship is down

			for (int i = 0; i < ship.length; i++) {
				// Because coords have to be saved as chars, we need to know the relative
				// start point since Chars are just integers we convert it to an int
				int letterStart = 'A';

				// Depending on the direction of the ship we have to add to x or y
				int xMultiplier = 1 - ship.shipDirection;
				int yMultiplier = ship.shipDirection;

				// Calculate the next coord that the ship is on
				Coords shipCoord = ship.coords + Coords(xMultiplier * i, yMultiplier * i + letterStart);

				// If the coord is not hit, the ship is not down yet
				if (!hasBeenShot(shipCoord)) {
					return false;
				}
			}
			return true;
		};

		Coords getLastNonSunkHit () {
			// Returns last hit that does not have a sunk ship but does have a ship
			// If none, returns coords(0, 0)
			for (vector<Coords>::reverse_iterator missile = this->missiles.rbegin(); missile != this->missiles.rend(); ++missile) {
				if (this->hasShip(*missile) && !this->sunkShip(*missile)) {
					return *missile;
				}
			}

			return Coords(0, 0 + 'A');
		};

		bool gameOver () {
			// Returns true if there is still a ship left
			// Otherwise returns false

			for (BattleShipShip ship : this->ships) {
				if (!this->isShipDown(ship)) return false;
			}

			return true;
		};

		const char* getStatus (Coords target) {
			bool hasMissile = this->hasBeenShot(target);
			bool hasShip = this->hasShip(target);

			if (this->enemy) {
				if (hasMissile) {
					if (hasShip) {
						return "*";
					} else {
						return "#";
					}
				}
			} else {
				if (hasMissile) {
					if (hasShip) {
						return "*";
					} else {
						return "#";
					}
				} else {
					if (hasShip) {
						return "O";
					}
				}
			}

			return " ";
		}

		vector<BattleShipShip> ships;
		vector<Coords> missiles;
		int width;
		int height;
		bool enemy;
};

// Stream overloading
ostream& operator<<(ostream& os, BattleShipBoard& board) {
	cout << "  1 2 3 4 5 6 7 8 9 10" << endl;
	cout << " +-+-+-+-+-+-+-+-+-+-+" << endl;
	for (int y = 0; y < 10; y++) {
		cout << (char)('A' + y) << "|";
		for (int x = 1; x < 11; x++) {
			cout << board.getStatus(Coords(x, y + 'A')) << "|";
		}
		cout << endl << " +-+-+-+-+-+-+-+-+-+-+" << endl;
	}
	return os;
}

class BattleShipGame {
	private:
		vector<BattleShipBoard> boards;
	public:
		BattleShipGame() {
			this->boards.push_back(BattleShipBoard(10, 10, false));
			this->boards.push_back(BattleShipBoard(10, 10, true));
		};

		BattleShipBoard* getBoard (int board) {
			return &this->boards.at(board);
		};

		bool ended () {
			for (BattleShipBoard board : this->boards) {
				if (board.gameOver()) return true;
			}

			return false;
		};

		vector<int> defaultSizes = {5, 4, 3, 3, 2};
};

class BattleShipCMDInterface {
	public:
		BattleShipGame* game;

		BattleShipCMDInterface (BattleShipGame* game) {
			this->game = game;
		}

		void setup (BattleShipBoard* board, vector<int>* sizes) {
			for (int size : *sizes) {
				// Keep asking the player where to place the ship
				// untill we succeed at adding it
				while (true) {
					int inputX;
					char inputY;
					int direction;

					cout << "Adding ship of length " << size << endl;

					// Ask the coordinates
					cout << "Please provide a X coordinate (1-10):" << endl;
					cin >> inputX;

					cout << "Please provide a Y coordinate (A-L) " << endl;
					cin >> inputY;

					Coords shipCoords = Coords(inputX, inputY);

					// Ask for a direction
					cout << "Please provide a direction (0 = horizontal, 1 = vertical)" << endl;
					cin >> direction;
					BattleShipShip::direction dir = static_cast<BattleShipShip::direction>(direction);

					cout << "Adding a ship at (" << inputX << ", " << inputY << ")" << endl;
					BattleShipShip tempShip = BattleShipShip(shipCoords, dir, size);
					if (board->addShip(tempShip, true)) {
						cout << "Ship added! Current board: " << endl;
						cout << *board;
						break;
					};

					cout << "Couldn't add ship, lets try again... " << endl;
				}
			}
		};

		void fire (BattleShipBoard* board) {
			while (true) {
				int x;
				char y;

				cout << "What is the coordinate you want to fire at? (Example: '1 A')" << endl;
				cin >> x;
				cin >> y;

				Coords target = Coords(x, y);
				if (board->fire(target, true)) {
					cout << "Fired at (" << x << ", " << y << ")" << endl;
					if (board->hasShip(target)) {
						cout << "You hit him!" << endl;
					} else {
						cout << "Better luck next time!" << endl;
					}
					if (board->sunkShip(target)) {
						cout << "Here you observe a ship in its natural habitat. The missile came and hit it. This killed the ship." << endl;
						cout << "Congrats, that ship is down!" << endl;
					}
					break;
				}

				cout << "Couldn't fire there, lets try again." << endl;
			}
		};
};

class BattleShipAI {
	public:
		BattleShipGame* game;

		BattleShipAI(BattleShipGame* game) {
			this->game = game;
		};

		void setup (BattleShipBoard* board, vector<int>* sizes) {
			// Add ships to the board

			for (int size : *sizes) {

				// Keep trying randomly to add a ship untill we succeed
				while (true) {
					// Generate a random coordinate
					int randomX = rand() % board->width + 1;
					int randomY = (rand() % board->height) + 'A';
					Coords shipCoords = Coords(randomX, randomY);

					// Generate a random direction
					BattleShipShip::direction dir = static_cast<BattleShipShip::direction>(rand() % 2);

					BattleShipShip tempShip = BattleShipShip(shipCoords, dir, size);
					if (board->addShip(tempShip)) break;
				}
			}
		};

		Coords randomFire (BattleShipBoard* board) {
			Coords targetCoords;

			while (true) {
				// Generate a random coordinate
				int randomX = rand() % board->width + 1;
				int randomY = (rand() % board->height) + 'A';
				targetCoords = Coords(randomX, randomY);

				if (board->fire(targetCoords, false)) {
					break;
				}
			}

			return targetCoords;
		}

		void fire(BattleShipBoard* board) {
			// This function makes the computer shoot a missile
			Coords lastHit = board->getLastNonSunkHit();
			Coords targetCoords;

			Coords up = lastHit.up();
			Coords down = lastHit.down();
			Coords left = lastHit.left();
			Coords right = lastHit.right();

			// Shoot a ship
			if (board->hasShip(lastHit) && !board->sunkShip(lastHit)) {
				if (board->hasBeenShot(up) && board->hasShip(up) && !board->sunkShip(up) && board->fire(down, false)) {
					targetCoords = down;
				} else if (board->hasBeenShot(left) && board->hasShip(left) && !board->sunkShip(left) && board->fire(right, false)) {
					targetCoords = right;
				} else if (board->hasBeenShot(down) && board->hasShip(down) && !board->sunkShip(down) && board->fire(up, false)) {
					targetCoords = up;
				} else if (board->hasBeenShot(right) && board->hasShip(right) && !board->sunkShip(right) && board->fire(left, false)) {
					targetCoords = left;
				} else if (board->fire(down, false)) {
					targetCoords = down;
				} else if (board->fire(right, false)) {
					targetCoords = right;
				} else if (board->fire(up, false)) {
					targetCoords = up;
				} else if (board->fire(left, false)) {
					targetCoords = left;
				} else {
					// something strange happened, lets just fire randomly
					targetCoords = this->randomFire(board);
				}
			} else {
				targetCoords = this->randomFire(board);
			}

			// Let the player know what happened
			cout << "The computer fired at (" << targetCoords.getX() << ", " << targetCoords.y << ")" << endl;
			if (board->sunkShip(targetCoords)) {
				cout << "The computer sunk a ship! :o" << endl;
			}
		};
};

int main () {
	// Initialize by seeding random
	srand(time(NULL));

	BattleShipGame game = BattleShipGame();
	BattleShipAI computer = BattleShipAI(&game);
	BattleShipCMDInterface player = BattleShipCMDInterface(&game);

	// Setup the game for the computer and the player
	cout << "Letting the computer place its pieces..." << endl;
	computer.setup(game.getBoard(1), &game.defaultSizes);
	cout << "Now it's your turn!" << endl;
	player.setup(game.getBoard(0), &game.defaultSizes);

	while (!game.ended()) {
		cout << "Current board status: " << endl;
		cout << *game.getBoard(0) << endl << *game.getBoard(1);
		
		// Let the player fire a missile
		cout << "Now it's your turn!" << endl;
		player.fire(game.getBoard(1));
		if (game.ended()) break;

		// Let the computer hit a missile
		cout << "Now it's the computers turn." << endl;
		computer.fire(game.getBoard(0));
	}

	cout << "Game over!" << endl;

	return 0;
}