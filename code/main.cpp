#include "ComplexPlane.h"

int main() {

	int pixelWidth = VideoMode::getDesktopMode().width;
	int pixelHeight = VideoMode::getDesktopMode().height;

	VideoMode vm(pixelWidth, pixelHeight);
	RenderWindow window(vm, "Mandelbrot Set", Style::Default);

	ComplexPlane plane(pixelWidth, pixelHeight);

	Text messageText;
	Font font;
	font.loadFromFile("Fonts/arial.ttf");
	messageText.setFont(font);
	plane.loadText(messageText);
	messageText.setCharacterSize(50);
	messageText.setFillColor(Color::White);
	messageText.setPosition(0, 0);

	bool update = true;

	while (window.isOpen()) {

		// USER INPUT

		Event event;
		while (window.pollEvent(event)) {
			if (event.type == Event::Closed) { 
				window.close(); 
			}

			if (event.type == Event::MouseButtonPressed) {
				if (event.mouseButton.button == Mouse::Left) {
					cout << "the left button was pressed" << endl;
					cout << "mouse x: " << event.mouseButton.x << endl;
					cout << "mouse y: " << event.mouseButton.y << endl;
					plane.setCenter(Vector2i(event.mouseButton.x, event.mouseButton.y));
					plane.zoomIn();
					update = true;
				}
				else if (event.mouseButton.button == Mouse::Right) {
					::cout << "the right button was pressed" << endl;
					cout << "mouse x: " << event.mouseButton.x << endl;
					cout << "mouse y: " << event.mouseButton.y << endl;
					plane.setCenter(Vector2i(event.mouseButton.x, event.mouseButton.y));
					plane.zoomOut();
					update = true;
				}
			}

			if (event.type == Event::MouseMoved) {
				plane.setMouseLocation(Vector2i(event.mouseMove.x, event.mouseMove.y));
			}
		}

		if (Keyboard::isKeyPressed(Keyboard::Escape)) {
			window.close();
		}

		// UPDATE
		if (update) {
			plane.updateRender();
			update = false;
		}
		plane.loadText(messageText);

		// DRAW

		window.clear();
		
		window.draw(plane);
		window.draw(messageText);

		window.display();

	}
}