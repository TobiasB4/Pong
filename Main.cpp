#include "Pong.h"

enum Buttons {
	PaddleOneUp = 0,
	PaddleOneDown,
	PaddleTwoUp,
	PaddleTwoDown,
};

enum class CollisionType {
	None,
	Top,
	Middle,
	Bottom,
	Left,
	Right
};

struct Contact {
	CollisionType type;
	float penetration;
};



class Vec2 {
public:
	float x, y;
	Vec2()
		:x(0.0f), y(0.0f)
	{}

	Vec2(float x, float y)
		:x(x), y(y)
	{}

	Vec2 operator+(Vec2 const& rhs) {
		return Vec2(x + rhs.x, y + rhs.y);
	}

	Vec2& operator+=(Vec2 const& rhs) {
		x += rhs.x;
		y += rhs.y;

		return *this;
	}

	Vec2 operator*(float rhs) {
		return Vec2(x * rhs, y * rhs);
	}


};



class Ball {
public:
	Vec2 position;
	Vec2 velocity;
	SDL_Rect rect;

	Ball(Vec2 position, Vec2 velocity)
		:position(position), velocity(velocity) {
		rect.x = static_cast<int>(position.x);
		rect.y = static_cast<int>(position.y);
		rect.w = BALL_WIDTH;
		rect.h = BALL_HEIGHT;
	}

	void update(float dt) {
		position += velocity * dt;
	}

	void CollideWithWall(Contact const& contact, float xConstant, float yConstant) {
		int negFlag = rand() % 2;
		if (negFlag) {
			negFlag = -negFlag;
		}
		else {
			negFlag = 1;
		}
		if ((contact.type == CollisionType::Top) || ((contact.type == CollisionType::Bottom))) {
			position.y += contact.penetration;
			velocity.y = -velocity.y;
		}
		else if (contact.type == CollisionType::Left) {
			position.x = SCREEN_WIDTH / 2.0f;
			position.y = SCREEN_HEIGHT / 2.0f;
			velocity.x = xConstant * BALL_SPEED;
			velocity.y = negFlag * yConstant * BALL_SPEED;
		}
		else if (contact.type == CollisionType::Right) {
			position.x = SCREEN_WIDTH / 2.0f - (float)BALL_WIDTH;
			position.y = SCREEN_HEIGHT / 2.0f - (float)BALL_HEIGHT;
			velocity.x = -xConstant * BALL_SPEED;
			velocity.y = negFlag * yConstant * BALL_SPEED;
		}
	}

	void CollideWithPaddle(Contact const& contact, float RandSpeed) {
		position.x += contact.penetration;
		if (velocity.x < 0) {
			velocity.x = BALL_SPEED;
		}
		else {
			velocity.x = -BALL_SPEED;
		}

		if (contact.type == CollisionType::Top) {
			velocity.y = RandSpeed * -0.75 * BALL_SPEED;
		}
		else if (contact.type == CollisionType::Bottom) {
			velocity.y = RandSpeed * 0.75 * BALL_SPEED;
		}
	}

	void Draw(SDL_Renderer* renderer) {
		rect.x = static_cast<int>(position.x);
		rect.y = static_cast<int>(position.y);
		SDL_RenderFillRect(renderer, &rect);
	}
};



class Paddle {
public:
	Vec2 position;
	Vec2 velocity;
	SDL_Rect rect;

	Paddle(Vec2 position, Vec2 velocity)
		:position(position), velocity(velocity) {
		rect.x = static_cast<int>(position.x);
		rect.y = static_cast<int>(position.y);
		rect.w = PADDLE_WIDTH;
		rect.h = PADDLE_HEIGHT;
	}

	void reset2() {
		position.x = SCREEN_WIDTH - 50.0f;
		position.y = (SCREEN_HEIGHT / 2.0f) - (PADDLE_HEIGHT / 2.0f);
	}
	void reset1() {
		position.x = 50.0f;
		position.y = (SCREEN_HEIGHT / 2.0f) - (PADDLE_HEIGHT / 2.0f);
	}

	void update(float dt) {
		position += velocity * dt;
		if (position.y < 0) {
			position.y = 0;
		}
		else if (position.y > (SCREEN_HEIGHT - PADDLE_HEIGHT)) {
			position.y = SCREEN_HEIGHT - PADDLE_HEIGHT;
		}
	}

	void Draw(SDL_Renderer* renderer) {
		rect.y = static_cast<int>(position.y);

		SDL_RenderFillRect(renderer, &rect);
	}
};

Contact collisionCheck(Ball const& ball, Paddle const& paddle);

Contact collisionCheck(Ball const& ball, Paddle const& paddle) {
	float ballLeft = ball.position.x;
	float ballRight = ball.position.x + BALL_WIDTH;
	float ballTop = ball.position.y;
	float ballBottom = ball.position.y + BALL_HEIGHT;

	float paddleLeft = paddle.position.x;
	float paddleRight = paddle.position.x + PADDLE_WIDTH;
	float paddleTop = paddle.position.y;
	float paddleBottom = paddle.position.y + PADDLE_HEIGHT;

	Contact contact{};

	if (ballLeft >= paddleRight || ballRight <= paddleLeft || ballTop >= paddleBottom || ballBottom <= paddleTop) {
		return contact;
	}

	float paddleRangeUpper = paddleBottom - (2.0f * PADDLE_HEIGHT / 3.0f);
	float paddleRangeMiddle = paddleBottom - (PADDLE_HEIGHT / 3.0f);

	if (ball.velocity.x < 0) {
		contact.penetration = paddleRight - ballLeft;
	}
	else if (ball.velocity.x > 0) {
		contact.penetration = paddleLeft - ballRight;
	}

	if ((ballBottom > paddleTop) && (ballBottom < paddleRangeUpper)) {
		contact.type = CollisionType::Top;
	}
	else if ((ballBottom > paddleRangeUpper) && (ballBottom < paddleRangeMiddle)) {
		contact.type = CollisionType::Middle;
	}
	else {
		contact.type = CollisionType::Bottom;
	}

	return contact;
}

Contact wallCollision(Ball const& ball);

Contact wallCollision(Ball const& ball) {
	float ballLeft = ball.position.x;
	float ballRight = ball.position.x + BALL_WIDTH;
	float ballTop = ball.position.y;
	float ballBottom = ball.position.y + BALL_HEIGHT;

	Contact contact;
	contact.type = CollisionType::None;
	if (ballLeft < 0.0f) {
		contact.type = CollisionType::Left;
	}
	else if (ballRight > SCREEN_WIDTH) {
		contact.type = CollisionType::Right;
	}
	else if (ballTop < 0.0f) {
		contact.type = CollisionType::Top;
		contact.penetration = -ballTop;
	}
	else if (ballBottom > SCREEN_HEIGHT) {
		contact.type = CollisionType::Bottom;
		contact.penetration = SCREEN_HEIGHT - ballBottom;
	}

	return contact;
}

class PlayerScore {
public:
	SDL_Renderer* renderer;
	TTF_Font* font;
	SDL_Surface* surface{};
	SDL_Texture* texture{};
	SDL_Rect rect{};

	PlayerScore(Vec2 position, SDL_Renderer* renderer, TTF_Font* font)
		:renderer(renderer), font(font) {
		surface = TTF_RenderText_Solid(font, "0", { 0xFF,0xFF,0xFF,0xFF });
		texture = SDL_CreateTextureFromSurface(renderer, surface);

		int width, height;
		SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);

		rect.x = static_cast<int>(position.x);
		rect.y = static_cast<int>(position.y);
		rect.w = width;
		rect.h = height;
	}

	~PlayerScore() {
		SDL_FreeSurface(surface);
		SDL_DestroyTexture(texture);
	}

	void setScore(int score) {
		SDL_FreeSurface(surface);
		SDL_DestroyTexture(texture);

		surface = TTF_RenderText_Solid(font, std::to_string(score).c_str(), { 0xFF,0xFF,0xFF,0xFF });
		texture = SDL_CreateTextureFromSurface(renderer, surface);

		int width, height;
		SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);
		rect.w = width;
		rect.h = height;
	}

	void Draw() {
		SDL_RenderCopy(renderer, texture, nullptr, &rect);
	}
};



int main(int argc, char* args[]) {
	srand(time(NULL));
	if (!(init())) {
		std::cout << "Could not initialize." << std::endl;
	}
	else {
		Ball ball(
			Vec2((SCREEN_WIDTH / 2.0f) - (BALL_WIDTH / 2.0f), (SCREEN_HEIGHT / 2.0f) - (BALL_WIDTH / 2.0f)),
			Vec2(BALL_SPEED, 0.0f)
		);
		Paddle paddleOne(
			Vec2(50.0f, (SCREEN_HEIGHT / 2.0f) - (PADDLE_HEIGHT / 2.0f)),
			Vec2(0.0f, 0.0f)
		);
		Paddle paddleTwo(
			Vec2(SCREEN_WIDTH - 50.0f, (SCREEN_HEIGHT / 2.0f) - (PADDLE_HEIGHT / 2.0f)),
			Vec2(0.0f, 0.0f)
		);
		PlayerScore playerOneScoreText(Vec2(SCREEN_WIDTH / 4, 20), renderer, typeFont);
		PlayerScore playerTwoScoreText(Vec2(3 * SCREEN_WIDTH / 4, 20), renderer, typeFont);

		int playerOneScore = 0;
		int playerTwoScore = 0;

		bool running = true;
		bool buttons[4] = {};
		float dt = 0.0f;

		while (running) {
			auto StartTime = std::chrono::high_resolution_clock::now();
			SDL_Event event;
			while (SDL_PollEvent(&event) != 0) {
				if (event.type == SDL_QUIT) {
					running = false;
				}
				else if (event.type == SDL_KEYDOWN) {
					if (event.key.keysym.sym == SDLK_ESCAPE) {
						running = false;
					}
					else if (event.key.keysym.sym == SDLK_w) {
						buttons[Buttons::PaddleOneUp] = true;
					}
					else if (event.key.keysym.sym == SDLK_s) {
						buttons[Buttons::PaddleOneDown] = true;
					}
					else if (event.key.keysym.sym == SDLK_UP) {
						buttons[Buttons::PaddleTwoUp] = true;
					}
					else if (event.key.keysym.sym == SDLK_DOWN) {
						buttons[Buttons::PaddleTwoDown] = true;
					}
				}
				else if (event.type == SDL_KEYUP) {
					if (event.key.keysym.sym == SDLK_w) {
						buttons[Buttons::PaddleOneUp] = false;
					}
					else if (event.key.keysym.sym == SDLK_s) {
						buttons[Buttons::PaddleOneDown] = false;
					}
					else if (event.key.keysym.sym == SDLK_UP) {
						buttons[Buttons::PaddleTwoUp] = false;
					}
					else if (event.key.keysym.sym == SDLK_DOWN) {
						buttons[Buttons::PaddleTwoDown] = false;
					}
				}
			}

			if (buttons[Buttons::PaddleOneUp]) {
				paddleOne.velocity.y = -PADDLE_SPEED;
			}
			else if (buttons[Buttons::PaddleOneDown]) {
				paddleOne.velocity.y = PADDLE_SPEED;
			}
			else {
				paddleOne.velocity.y = 0.0f;
			}
			if (buttons[Buttons::PaddleTwoUp]) {
				paddleTwo.velocity.y = -PADDLE_SPEED;
			}
			else if (buttons[Buttons::PaddleTwoDown]) {
				paddleTwo.velocity.y = PADDLE_SPEED;
			}
			else {
				paddleTwo.velocity.y = 0.0f;
			}

			paddleOne.update(dt);
			paddleTwo.update(dt);
			ball.update(dt);

			float xVelConstant = Randomizer(X_VEL_HI, X_VEL_LO);
			float yVelConstant = Randomizer(Y_VEL_HI, Y_VEL_LO);

			Contact contact1 = collisionCheck(ball, paddleOne);
			Contact contact2 = collisionCheck(ball, paddleTwo);
			Contact contactWall = wallCollision(ball);

			if (contact1.type != CollisionType::None) {
				ball.CollideWithPaddle(contact1, yVelConstant);
			}
			else if (contact2.type != CollisionType::None) {
				ball.CollideWithPaddle(contact2, yVelConstant);
			}
			else if (contactWall.type != CollisionType::None) {
				ball.CollideWithWall(contactWall, xVelConstant, yVelConstant);

				if (contactWall.type == CollisionType::Left) {
					playerTwoScore++;
					playerTwoScoreText.setScore(playerTwoScore);
					paddleOne.reset1();
					paddleTwo.reset2();
				}
				else if (contactWall.type == CollisionType::Right) {
					playerOneScore++;
					playerOneScoreText.setScore(playerOneScore);
					paddleOne.reset1();
					paddleTwo.reset2();
				}
			}
			SDL_SetRenderDrawColor(renderer, 0x0, 0x0, 0x0, 0xFF);
			SDL_RenderClear(renderer);
			DrawNet();
			ball.Draw(renderer);
			paddleOne.Draw(renderer);
			paddleTwo.Draw(renderer);
			playerOneScoreText.Draw();
			playerTwoScoreText.Draw();
			SDL_RenderPresent(renderer);


			auto StopTime = std::chrono::high_resolution_clock::now();
			dt = std::chrono::duration<float, std::chrono::milliseconds::period>(StopTime - StartTime).count();
			if (contactWall.type == CollisionType::Left || contactWall.type == CollisionType::Right) {
				SDL_Delay(2000);
			}
		}
	}

	destroy();
	return 0;
}

bool init() {
	bool functional = true;
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cout << "Could not initialize video. SDL error: " << SDL_GetError() << std::endl;
		functional = false;
	}
	else {
		window = SDL_CreateWindow("Pong", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (window == NULL) {
			std::cout << "Could not create window. SDL error: " << SDL_GetError() << std::endl;
			functional = false;
		}
		else {
			renderer = SDL_CreateRenderer(window, -1, 0);
			if (renderer == NULL) {
				std::cout << "Could not create renderer. SDL error: " << SDL_GetError() << std::endl;
				functional = false;
			}
			else {
				if (TTF_Init() < 0) {
					std::cout << "Could not intialize TTF. TTF error: " << TTF_GetError() << std::endl;
					functional = false;
				}
				else {
					typeFont = TTF_OpenFont("Roboto-Black.ttf", 36);
					if (typeFont == NULL) {
						std::cout << "Could not initialize font. SDL error: " << TTF_GetError() << std::endl;
						functional = false;
					}
				}
			}

		}
	}
	return functional;
}

void DrawNet() {
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	for (int y = 0; y < SCREEN_HEIGHT; y++) {
		if (y % 5) {
			SDL_RenderDrawPoint(renderer, SCREEN_WIDTH / 2, y);
		}
	}
}

float Randomizer(float HI, float LO) {
	float ans = LO + static_cast<float> (rand()) / (static_cast<float>(RAND_MAX / (HI - LO)));
	return ans;
}

void destroy() {

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	TTF_CloseFont(typeFont);
	window = NULL;
	renderer = NULL;
	typeFont = NULL;

	TTF_Quit();
	SDL_Quit;
}
