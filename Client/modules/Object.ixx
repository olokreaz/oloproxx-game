module;

export module GameObject;

namespace Game
{
	export class GameObject {
	public:
		GameObject(double x, double y, double vx, double vy) : 
		    x(x), y(y), 
		    vx(vx), vy(vy) {}

		void applyGravity(double gravity, double dt) {
			vy += gravity * dt;
		}

		void handleCollision() {
			// Добавить логику обработки столкновений
		}

		void jump(double jumpSpeed) {
			vy = -jumpSpeed;
		}

		void move(double dt) {
			x += vx * dt;
			y += vy * dt;
		}

		double getX() { return x; }
		double getY() { return y; }
		double getVx() { return vx; }
		double getVy() { return vy; }

	private:
		double x, y;   // Позиция объекта
		double vx, vy; // Скорость объекта
	};
}
